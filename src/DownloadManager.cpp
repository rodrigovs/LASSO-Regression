/**
 * @file DownloadManager.cpp
 * @author Diego França Tozetto (diego@geolumini.com.br)
 * @brief 
 * @version 0.1
 * @date 10/02/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */

////////////////////////////////////////////////////////////////////////
//                          INCLUDES
////////////////////////////////////////////////////////////////////////
#include "DownloadManager.h"

////////////////////////////////////////////////////////////////////////
//                          PROTÓTIPOS
////////////////////////////////////////////////////////////////////////
void vTaskDownload(void *pvParameters);
bool loadDownloadFile();
bool loadDownloadVector(String nameFolder);
FileSystemOperationResult_e getDownload(Download &download);
DownloadStatusExecute_e executeDownload(Download &download);

//TASK
void setErrorDownload(Download &download,  FileSystemOperationResult_e statusGet);

//GETDOWNLOAD
FileSystemOperationResult_e getNextDownloadFolder(Download &download);
FileSystemOperationResult_e findNextDownload(Download &download);
FileSystemOperationResult_e loadDownloadInfo(Download &download);
bool createFolder(String path);
bool saveQueuedDownloads(bool reset);

//EXECUTEDOWNLOAD
bool stopRequested();
bool saveDownloadProgress(Download &download, String content);
bool finishedDownload(Download &download);

//GETDOWNLOAD | EXECUTEDOWNLOAD
bool moveTo(Download &download, DownloadStatusMoveTo_e statusMoveTo);
bool updateFile(Download &download, DownloadUpdateType_e updateType);
bool saveHasDownloadInProgress(bool inProgress);

////////////////////////////////////////////////////////////////////////
//                          VARIÁVEIS
////////////////////////////////////////////////////////////////////////
volatile bool g_isStarted = false;
volatile bool g_stopRequested = false;
SemaphoreHandle_t xSemaphore = NULL;
SemaphoreHandle_t xSemaphoreList = NULL;
SemaphoreHandle_t xSemaphoreLock = NULL;

////////////////////////////////////////////////////////////////////////
//                          FUNÇÕES ESTÁTICAS
////////////////////////////////////////////////////////////////////////

///
/// TASK
///

/**
 * @brief Tarefa responsavel por rodar a sequencia de downloads.
 * 
 * @param pvParameters Parametro passa para a tarefa.
 */
void vTaskDownload(void *pvParameters)
{
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    g_isStarted = true;
    xSemaphoreGive(xSemaphore);

    while (true)
    {
        if (stopRequested())
            break;
        
        if (VidoGsmManager.isGprsConnected())
        {
            Download download;
            FileSystemOperationResult_e fileSystemOperationResult = getDownload(download);
            if(fileSystemOperationResult != FS_RESULT_OK && fileSystemOperationResult != FS_RESULT_DIRECTORY_EMPTY)
                setErrorDownload(download, fileSystemOperationResult); 
            else if (fileSystemOperationResult == FS_RESULT_OK)
            {
                DownloadStatusExecute_e statusExecute = executeDownload(download);                
                if(statusExecute == STATUS_EXECUTE_ERROR)
                    setErrorDownload(download, FS_RESULT_ERROR_MOVE_FROM_CURRENT);
                else if (statusExecute == STATUS_EXECUTE_BREAK)
                    break;
            }  
        }
                
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    //ESP_LOGD(TAG_DOWNLOAD_MANAGER, "Download Manager parado...");
    vTaskDelete(NULL);
}

/**
 * @brief Determina o que fazer com o erro ocorrido.
 * 
 * @param download Informacoes do download da vez.
 * @param status Determina a movimentacao do arquivo.
 */
void setErrorDownload(Download &download, FileSystemOperationResult_e fileSystemOperationResult)
{    
    download.setStatus(DOWNLOAD_STATUS_ERROR);
    DownloadManager.updateDownload(download);

    DownloadStatusMoveTo_e statusMoveTo = MOVE_NONE;
    if (fileSystemOperationResult == FS_RESULT_ERROR_MOVE_FROM_CURRENT)
        statusMoveTo = MOVE_CURRENT_TO_ERROR;
    else if (fileSystemOperationResult == FS_RESULT_ERROR_MOVE_FROM_QUEUE)
        statusMoveTo = MOVE_QUEUE_TO_ERROR;

    bool isStop = true;
    if (statusMoveTo != MOVE_NONE)
    {
        if (!moveTo(download, statusMoveTo)) //Move o arquivo para a pasta ERROR.
        {
            //Falha ao setar erro. Erro ao mover arquivo.
            DownloadManager.setLastError(DWN_ERR_SRC_SET_ERROR_DOWNLOAD, DWN_ERR_0049, DWN_ERR_MSG_0049);
        }
        else
        {            
            if(!saveHasDownloadInProgress(false)) //Atualizar Prefs.
            {
                //"Falha ao setar erro. Erro ao salvar preferences 'downlaodInProgress'."
                DownloadManager.setLastError(DWN_ERR_SRC_SET_ERROR_DOWNLOAD, DWN_ERR_0050, DWN_ERR_MSG_0050);
            }
            else
            {                
                if (!updateFile(download, UPDATE_TYPE_ERROR)) //Atualizar informacoes do arquivo JSON no diretorio ERROR.
                {
                    //"Falha ao setar erro. Erro na atualizacao do arquivo."
                    DownloadManager.setLastError(DWN_ERR_SRC_SET_ERROR_DOWNLOAD, DWN_ERR_0051, DWN_ERR_MSG_0051);
                }
                else
                {
                    isStop = false;

                    //TODO: Atualizar Logs do Sistema. 

                    if(statusMoveTo == MOVE_CURRENT_TO_ERROR)
                    {              
                        StorageController.waitOne(portMAX_DELAY);      
                        if(!SD.remove(download.getDestinationFile().c_str())) //Remover o arquivo que stava sendo baixado.
                        {
                            //"Falha ao setar erro. Erro ao remover o arquivo baixado."
                            DownloadManager.setLastError(DWN_ERR_SRC_SET_ERROR_DOWNLOAD, DWN_ERR_0052, DWN_ERR_MSG_0052);
                        } 
                        StorageController.releaseMutex();          
                    }         
                }                
            }             
        }
    }
    
    //Erro critico (Parar Download Manager).
    if (isStop)
        DownloadManager.stop();        
}

///
/// GETDOWNLOAD
///

/**
 * @brief Obtem o download da vez.
 *
 * @param download Informacoes do download da vez.
 * @return FileSystemOperationResult_e Resultado da Operacao.
 *      FS_RESULT_OK Download obtido com sucesso.
 *      FS_RESULT_FOLDER_EMPTY Nao existe um download para ser feito.
 *      FS_RESULT_ERROR Erro ao obter nome da pasta.
 *      FS_RESULT_ERROR_MOVE_QUEUE Erro ao carregar informacoes do download, mover da pasta QUEUEU para ERROR.
 *      FS_RESULT_ERROR_MOVE_CURRENT Erro ao carregar informacoes do download, mover da pasta CURRENT para ERROR. 
 */
FileSystemOperationResult_e getDownload(Download &download)
{    
    FileSystemOperationResult_e result = getNextDownloadFolder(download); //Obtem o nome da pasta do download da vez.
    if (result == FS_RESULT_OK)
    {
        result = findNextDownload(download); //Encontra o nome do arquivo do download da vez.
        if (result == FS_RESULT_OK)        
            result = loadDownloadInfo(download); //Carrega as informacoes do arquivo de download da vez.   
    }

    return result;
}

/**
 * @brief Obtem o nome da pasta do download da vez. 
 * 
 * @param download Struct com as informacoes do download.
 * @return FileSystemOperationResult_e Resultado da operacao.
 *      FS_RESULT_OK Nome da pasta obtido com sucesso.
 *      FS_RESULT_ERROR Erro ao obter nome da pasta.
 *      FS_RESULT_FOLDER_EMPTY Nao existe um download para ser feito.
 */
FileSystemOperationResult_e getNextDownloadFolder(Download &download)
{
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_PREFS, false))
    {
        //"Falha ao obter download. Erro ao carregar preferences."
        DownloadManager.setLastError(DWN_ERR_SRC_GET_NEXT_DOWNLOAD_FOLDER, DWN_ERR_0023, DWN_ERR_MSG_0023);
        return FS_RESULT_ERROR;
    }

    //Verifica em qual diretorio o arquivo deve ser lido.
    if (prefs.getBool(KEY_HAS_DOWNLOAD_IN_PROGRESS, false)) 
        download.setDirectoryName(String(CURRENT_DIR));
    else
    {
        if (prefs.getInt(KEY_QUEUED_DOWNLOADS, 0) == 0)
        {
            prefs.end();

            return FS_RESULT_DIRECTORY_EMPTY;
        }
        else
            download.setDirectoryName(String(QUEUE_DIR));
    }
    prefs.end();

    return FS_RESULT_OK;
}

/**
 * @brief Encontra o nome do arquivo do download da vez.
 * 
 * @param download Struct com as informacoes do download da vez.
 * @return FileSystemOperationResult_e Resultad da operacao.  
 *      FS_RESULT_OK O download da vez foi encontrado com sucesso.
 *      FS_RESULT_DIRECTORY_EMPTY O diretorio esta vazio (corrompeu a pasta).
 *      FS_RESULT_ERROR Erro ao encorntrar o download da vez.
 */
FileSystemOperationResult_e findNextDownload(Download &download)
{
    StorageController.waitOne(portMAX_DELAY);
    SdFile sdRoot;
    if (!sdRoot.open(download.getDirectoryName().c_str()))
    {
        //"Falha ao obter download. Erro ao abrir diretorio."        
        DownloadManager.setLastError(DWN_ERR_SRC_FIND_NEXT_DOWNLOAD, DWN_ERR_0024, DWN_ERR_MSG_0024);        
        StorageController.releaseMutex();
        return FS_RESULT_ERROR;
    }

    //Verifica o download da vez.
    char name[255];
    int currentPriority = 0;
    int currentId = 0;

    SdFile sdFile;
    while (sdFile.openNext(&sdRoot, O_RDONLY))
    {
        sdFile.getName(name, 255);

        int priority = String(name).substring(0, 1).toInt();
        int id = String(name).substring(1).toInt();

        if (priority >currentPriority)
        {
            currentPriority = priority;
            currentId = id;
        }
        else if (priority == currentPriority)
            if (id < currentId)
                currentId = id;

        sdFile.close();
    }    

    if (sdRoot.getError())
    {
        //"Falha ao carregar informacoes do download. Erro ao processar diretorio."
        DownloadManager.setLastError(DWN_ERR_SRC_FIND_NEXT_DOWNLOAD, DWN_ERR_0025, DWN_ERR_MSG_0025);
        StorageController.releaseMutex();
        return FS_RESULT_ERROR;
    }
    sdRoot.close();

    StorageController.releaseMutex();
        
    download.setId(currentId);
    download.setPriority((Priority_e) currentPriority);

    if (download.getId() <= 0 || download.getPriority() <= 0) //So vai entrar aqui se corromper a pasta.
    {        
        if(download.getDirectoryName() == String(QUEUE_DIR))
        {
            if(!saveQueuedDownloads(true))
            {
                //"Falha ao carregar informacoes do download. Erro ao salvar parametros no preferences 'queueDownloads'."
                DownloadManager.setLastError(DWN_ERR_SRC_FIND_NEXT_DOWNLOAD, DWN_ERR_0026, DWN_ERR_MSG_0026);
                return FS_RESULT_ERROR;   
            }
        }
        else
        {
            if(!saveHasDownloadInProgress(false))
            {
                //"Falha ao carregar informacoes do download. Erro ao salvar parametros no preferences 'DownloadInProgress'."
                DownloadManager.setLastError(DWN_ERR_SRC_FIND_NEXT_DOWNLOAD, DWN_ERR_0027, DWN_ERR_MSG_0027);
                return FS_RESULT_ERROR;
            }
        }

        return FS_RESULT_DIRECTORY_EMPTY;
    }

    return FS_RESULT_OK;
}

/**
 * @brief Carrega as informacoes do arquivo de download da vez.   
 * 
 * @param download Struct com as informacoes do download da vez.
 * @return FileSystemOperationResult_e Resultado da operacao. 
 *      FS_RESULT_OK Informacoes do downlaod carregadas com sucesso.
 *      FS_RESULT_ERROR_MOVE_QUEUE Erro ao carregar informacoes do download, mover da pasta QUEUEU para ERROR.
 *      FS_RESULT_ERROR_MOVE_CURRENT Erro ao carregar informacoes do downloas, mover da pasta CURRENT para ERROR.
 */
FileSystemOperationResult_e loadDownloadInfo(Download &download)
{
    //Verifica diretorio atual.
    FileSystemOperationResult_e statusResultError = FS_RESULT_ERROR_MOVE_FROM_QUEUE;
    if (download.getDirectoryName().compareTo(String(CURRENT_DIR)) == 0)
        statusResultError = FS_RESULT_ERROR_MOVE_FROM_CURRENT;

    //Monta nome do arquivo.
    String name = download.getDirectoryName() + "/" + String(download.getPriority()) + String(download.getId());
    
    StorageController.waitOne(portMAX_DELAY);
    File file = SD.open(name, O_RDONLY);
    if (!file)
    {
        //"Falha ao carregar informacoes do download. Error ao abrir arquivo."
        DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0028, DWN_ERR_MSG_0028);
        download.setError(DWN_ERR_0028, DWN_ERR_MSG_0028);
        StorageController.releaseMutex();
        return statusResultError;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);

    file.close();
    StorageController.releaseMutex();

    if (error)
    {
        //"Falha ao carregar informacoes do download. Erro ao desirelizar o arquivo JSON."
        DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0029, DWN_ERR_MSG_0029);
        download.setError(DWN_ERR_0029, DWN_ERR_MSG_0029);
        return statusResultError;
    }
    else
    {
        download.setFileURL(String((const char*) doc["fileURL"]));
        download.setDestinationFile(String((const char*) doc["destinationFile"]));  
        int flag = (int) doc["flag"];
        download.setFlag((Flag_e) flag);  

        Serial.println(String(download.getFlag()));   

        if (download.getDirectoryName().compareTo(String(CURRENT_DIR)) == 0)
        {
            download.setFileSize(doc["fileSize"]); 
            download.setDownloaded(doc["downloaded"]);
            download.setStarted(String((const char*) doc["started"]));            
        }        
        
        //Verifica erro nas informacoes deserelizadas
        if (download.getFileURL() == "" || download.getDestinationFile() == "" || download.getFileSize() < 0 || download.getDownloaded() < 0)
        {
            //"Falha ao carregar informacoes do download. Informacoes contidas no arquivo estao incorretas."
            DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0030, DWN_ERR_MSG_0030);
            download.setError(DWN_ERR_0030, DWN_ERR_MSG_0030);
            return statusResultError;
        }

        if (download.getDownloaded() > 0 && download.getFileSize() == 0)
        {
            //"Falha ao executar download. Falha ao indentificar o tamanho do arquivo."
            DownloadManager.setLastError(DWN_ERR_SRC_EXECUTE_DOWNLOAD, DWN_ERR_0031, DWN_ERR_MSG_0031);
            download.setError(DWN_ERR_0041, DWN_ERR_MSG_0031);
            return statusResultError;
        }

        //Verifica se o arquivo já existe
        if(download.getDownloaded() == 0)
        { 
            StorageController.waitOne(portMAX_DELAY);
            if(SD.exists(download.getDestinationFile().c_str()))
            {
                StorageController.releaseMutex();

                Flag_e flag = download.getFlag();
                if(flag == FLAG_NO_REPLACE)
                {
                    //"Falha ao adicionar download. Arquivo ja existe."
                    DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0032, DWN_ERR_MSG_0032);
                    download.setError(DWN_ERR_0032, DWN_ERR_MSG_0032);
                    return statusResultError;
                }
                else if(flag == FLAG_REPLACE)
                {
                    StorageController.waitOne(portMAX_DELAY);
                    if(!SD.remove(download.getDestinationFile().c_str()))
                    {
                        //"Falha ao carregar informacoes do download. Não conseguiu remover o arquivo de destino com o mesmo nome."
                        DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0033, DWN_ERR_MSG_0033);
                        download.setError(DWN_ERR_0033, DWN_ERR_MSG_0033);
                        StorageController.releaseMutex();
                        return statusResultError;   
                    }
                    StorageController.releaseMutex();
                }
            }
            else
            {
                StorageController.releaseMutex();

                //Cria pasta de destino
                if (!createFolder(download.getDestinationFile()))
                {
                    //"Falha ao carregar informacoes do download. Erro ao criar diretorio de destino."
                    DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0034, DWN_ERR_MSG_0034);
                    download.setError(DWN_ERR_0034, DWN_ERR_MSG_0034);
                    return statusResultError;
                }
            }            
        }            

        //Move download da pasta QUEUE para a pasta CURRENT.
        if (download.getDirectoryName().compareTo(String(QUEUE_DIR)) == 0)
        {
            if (!moveTo(download, MOVE_QUEUE_TO_CURRENT))
            {
                //"Falha ao carregar informacoes do download. Erro ao mover arquivo."
                DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0035, DWN_ERR_MSG_0035);
                download.setError(DWN_ERR_0035, DWN_ERR_MSG_0035);
                return FS_RESULT_ERROR_MOVE_FROM_QUEUE;
            }
            else
            {
                //Atualiza preferences
                if (!saveHasDownloadInProgress(true))
                {
                    //"Falha ao carregar informacoes do download. Erro no preferences 'downloadInProgress'."
                    DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0036, DWN_ERR_MSG_0036);
                    download.setError(DWN_ERR_0036, DWN_ERR_MSG_0036);               
                    return FS_RESULT_ERROR_MOVE_FROM_CURRENT;
                }
                
                if (!saveQueuedDownloads(false))
                {
                    //"Falha ao carregar informacoes do download. Erro no preferences 'queuedDownloads'."
                    DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0037, DWN_ERR_MSG_0037);
                    download.setError(DWN_ERR_0037, DWN_ERR_MSG_0037); 
                    return FS_RESULT_ERROR_MOVE_FROM_CURRENT;
                }
                
                if (!updateFile(download, UPDATE_TYPE_IN_PROGRESS))
                {
                    //"Falha ao carregar informacoes do download. Erro ao atualizar arquivo."
                    DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0038, DWN_ERR_MSG_0038);
                    download.setError(DWN_ERR_0038, DWN_ERR_MSG_0038); 
                    return FS_RESULT_ERROR_MOVE_FROM_CURRENT;
                }
            }
        }
       
        ESP_LOGD(TAG_DOWNLOAD_MANAGER, "Obteu as informacoes do arquivo.");
        return FS_RESULT_OK;        
    }    
}

/**
 * @brief cria pasta de destino caso nao exista.
 * 
 * @param path Nome da pasta de destino.
 * @return true Verificacao ocorreu com sucesso.
 * @return false Ocorreu um erro ao verificar diretorio.
 */
bool createFolder(String path)
{
    int lastSlashIndex = path.lastIndexOf("/");
    path.remove(lastSlashIndex, path.length() - lastSlashIndex);

    bool result = false;
    StorageController.waitOne(portMAX_DELAY);
    SdFile sdRoot;    
    if (!sdRoot.open(path.c_str()))
        result = SD.mkdir(path.c_str());
    else 
        result = true;
    sdRoot.close();
    StorageController.releaseMutex();

    return result;
}

/**
 * @brief Salva parametro de quantidade de downloads na pasta QUEUE no prefs.
 * 
 * @param reset Indica se o parametro prefs precisa ser resetado.
 * @return true Parametro salvo com sucesso.
 * @return false Erro ao salvar parametro.
 */
bool saveQueuedDownloads(bool reset)
{
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_PREFS, false))
        return false;
    
    int queuedDownloads = 0;
    if(reset == false)
    {
        queuedDownloads = prefs.getInt(KEY_QUEUED_DOWNLOADS, 0);
        queuedDownloads--;
    }

    prefs.putInt(KEY_QUEUED_DOWNLOADS, queuedDownloads);
    prefs.end();

    return true;
}

///
/// EXECUTEDOWNLOAD
///

/**
 * @brief Executa o download da vez.
 * 
 * @param download Struct com as informacoes do download da vez.
 * @return DownloadStatusExecute_e Resultado da operacao.
 *      STATUS_EXECUTE_OK Download finzalido com sucesso. 
 *      STATUS_EXECUTE_DISCONNECTED_NETWORK Ocorreu um erro na conexao da rede.  
 *      STATUS_EXECUTE_DISCONNECTED_CLIENT Ocorreu um erro na conexao do cliente.
 *      STATUS_EXECUTE_BREAK Para a execucao do download.
 *      STATUS_EXECUTE_ERROR Ocorreu um erro ao executar download.          
 */
DownloadStatusExecute_e executeDownload(Download &download)
{
    ESP_LOGD(TAG_MAIN, "Baixando...");
    
    //Obtem server, resource.
    char separator = '/';
    String server = String(download.getFileURL()).substring(0, String(download.getFileURL()).indexOf(separator));
    String resource = String(download.getFileURL()).substring(String(download.getFileURL()).indexOf(separator), download.getFileURL().length());

    //Conecta com o cliente.
    VidoHttpClient http(*VidoGsmManager.client(DownloadManager.getIdClient()), server);
    ESP_LOGD(TAG_MAIN, "HTTP GET Request...");

    //Monta Header    
    http.beginRequest();
    int err = http.get(resource);

    if (download.getDownloaded() != 0)
    {
        String headerValue = "bytes=" + String(download.getDownloaded()) + "-" + String(download.getFileSize());
        http.sendHeader("Range", headerValue.c_str());        
    }

    http.endRequest(); 

    //TODO: Verificar se o SIM possui crédito.

    if (err != 0)
    {
        //"Falha ao executar download. Falha de conexao ao servidor."
        DownloadManager.setLastError(DWN_ERR_SRC_EXECUTE_DOWNLOAD, DWN_ERR_0039, DWN_ERR_MSG_0039);
        download.setError(DWN_ERR_0039, DWN_ERR_MSG_0039); 
        return STATUS_EXECUTE_ERROR;
    }

    ESP_LOGD(TAG_MAIN, "Header Ok...");

    int status = http.responseStatusCode();
    if (!status)
    {
        if (status == -3)
        {
            //"Falha ao executar download. Ocorreu Timeout ao tentar acessar o servidor"
            DownloadManager.setLastError(DWN_ERR_SRC_EXECUTE_DOWNLOAD, DWN_ERR_0040, DWN_ERR_MSG_0040);
            download.setError(DWN_ERR_0040, DWN_ERR_MSG_0040); 
        }
        else if (status == -4)
        {
            //"Falha ao executar download. Resposta invalida do servidor"
            DownloadManager.setLastError(DWN_ERR_SRC_EXECUTE_DOWNLOAD, DWN_ERR_0041, DWN_ERR_MSG_0041);
            download.setError(DWN_ERR_0041, DWN_ERR_MSG_0041); 
        }
        else
        {
            //"Falha ao executar download. Status Code: "
            DownloadManager.setLastError(DWN_ERR_SRC_EXECUTE_DOWNLOAD, DWN_ERR_0042, DWN_ERR_MSG_0042 + status);
            download.setError(DWN_ERR_0042, DWN_ERR_MSG_0042); 
        }
        return STATUS_EXECUTE_ERROR;
    }

    ESP_LOGD(TAG_MAIN, "Status Ok...");
    ESP_LOGD(TAG_MAIN, "Response Headers...");

    while (http.headerAvailable())
    {
        String headerName = http.readHeaderName();
        String headerValue = http.readHeaderValue();
        Serial.println("    " + headerName + " : " + headerValue);
    }

    int contentLength = http.contentLength();
    if(download.getDownloaded() == 0)
    {
        //Atualiza Arquivo, antes do seu inicio.
        String datetime = Utils.toString(VidoGsmManager.getDateTime()).c_str();
        download.setStarted(datetime);
        ESP_LOGD(TAG_MAIN, "contentLength: %d", contentLength);
        download.setFileSize(contentLength);

        if (!updateFile(download, UPDATE_TYPE_IN_PROGRESS))
        {
            //"Falha ao executar download. Erro ao atualizar arquivo com tamanho do download."
            DownloadManager.setLastError(DWN_ERR_SRC_LOAD_DOWNLOAD_INFO, DWN_ERR_0043, DWN_ERR_MSG_0043);
            download.setError(DWN_ERR_0043, DWN_ERR_MSG_0043); 
            return STATUS_EXECUTE_ERROR;
        }
    }     

    int bufferSize = 1024;
    int bytesRead = 0;
    int currentBytesRead = 0;
    uint8_t buffer[bufferSize + 1];
    uint32_t readLength = 0;

    ESP_LOGD(TAG_MAIN, "Iniciou Download...");

    DownloadStatusExecute_e statusExecute = STATUS_EXECUTE_OK;
    while (bytesRead < contentLength)
    {
        if (stopRequested())
        {
            statusExecute = STATUS_EXECUTE_BREAK;
            break;
        }

        if (!VidoGsmManager.isGprsConnected())
        {
            ESP_LOGD(TAG_MAIN, "Conexao com a Rede Perdida.");
            statusExecute = STATUS_EXECUTE_DISCONNECTED_NETWORK;
            break;
        }

        if (!http.connected())
        {
            ESP_LOGD(TAG_MAIN, "Conexao com o Cliente Perdida.");
            statusExecute = STATUS_EXECUTE_DISCONNECTED_CLIENT;
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        currentBytesRead = http.read(buffer, bufferSize);

        if (currentBytesRead <= 0)
            ESP_LOGD(TAG_MAIN, "leitura Incompleta...Buscando Pacote...");
        else
        {                        
            bytesRead += currentBytesRead;
            String content = "";
            for (int i = 0; i < currentBytesRead; i++)
            {
                content += (char)buffer[i];
                readLength++;
            }

            download.setDownloaded(download.getDownloaded() + currentBytesRead);  

            if(download.getStatus() != DOWNLOAD_STATUS_DOWNLOADING)            
                download.setStatus(DOWNLOAD_STATUS_DOWNLOADING);            

            if(!saveDownloadProgress(download, content))
            {
                statusExecute = STATUS_EXECUTE_ERROR;   
                break;
            }
            else
            {
                DownloadManager.updateDownload(download); 
                ESP_LOGD(TAG_MAIN, "Baixado [bytes]: %d", download.getDownloaded());
                ESP_LOGD(TAG_MAIN, "Baixado: %.2f%%", (100.0 * readLength) / contentLength);
            }              
        }
        delay(500);
    }

    ESP_LOGD(TAG_MAIN, "Dados do Download...bytes Lidos: %d | Tamanho do Arquivo Final: %d", bytesRead, download.getDownloaded());
    //ESP_LOGD(TAG_DOWNLOAD_MANAGER, "Dados do Download...Status: %d", download.getStatus());
    //Finalizou download
    if (download.getDownloaded() == download.getFileSize())    
        if (!finishedDownload(download))
            statusExecute = STATUS_EXECUTE_ERROR;

    if(statusExecute == STATUS_EXECUTE_OK || statusExecute == STATUS_EXECUTE_BREAK)
    {
        if (statusExecute == STATUS_EXECUTE_OK) 
            download.setStatus(DOWNLOAD_STATUS_FINISHED); //Download Finalizado
        else
            download.setStatus(DOWNLOAD_STATUS_WAIT); //Download Em Espera     

        DownloadManager.updateDownload(download);
    }

    return statusExecute;

    return STATUS_EXECUTE_OK;
}

/**
 * @brief Verifica se download precisa parar.
 * 
 * @return true Para download.
 * @return false Nao para download.
 */
bool stopRequested()
{
    bool stopRequested = false;

    xSemaphoreTake(xSemaphore, portMAX_DELAY);

    stopRequested = g_stopRequested;
    if (stopRequested)
        g_isStarted = false;

    g_stopRequested = false;

    xSemaphoreGive(xSemaphore);

    return stopRequested;
}

/**
 * @brief Salva o progresso do download.
 * 
 * @param download Struct com as informacoes do download da vez.
 * @param content Conteudo a ser salvo no arquivo.
 * @return true Progresso salvo com sucesso.
 * @return false Erro ao salvar progresso.
 */
bool saveDownloadProgress(Download &download, String content)
{
    StorageController.waitOne(portMAX_DELAY);
    File file = SD.open(download.getDestinationFile(), FILE_WRITE);

    if (!file)
    {
        //"Falha ao executar download. Erro ao abrir arquivo."
        DownloadManager.setLastError(DWN_ERR_SRC_SAVE_PROGRESS, DWN_ERR_0044, DWN_ERR_MSG_0044);
        download.setError(DWN_ERR_0044, DWN_ERR_MSG_0044); 
        StorageController.releaseMutex();
        return false;
    }

    file.print(content);
    file.close();
    StorageController.releaseMutex();

    if (!updateFile(download, UPDATE_TYPE_IN_PROGRESS))
    {
        //"Falha ao executar download. Erro ao atualizar arquivo em execucao."
        DownloadManager.setLastError(DWN_ERR_SRC_SAVE_PROGRESS, DWN_ERR_0045, DWN_ERR_MSG_0045);
        download.setError(DWN_ERR_0045, DWN_ERR_MSG_0045); 
        return false;
    }

    return true;
}

/**
 * @brief Move e salva o download finalizado.
 * 
 * @param download Struct com as informacoes do download da vez.
 * @return true Download finalizado com sucesso.
 * @return false Erro ao finalizar download.
 */
bool finishedDownload(Download &download)
{
    if (!moveTo(download, MOVE_CURRENT_TO_FINISHED))
    {
        //"Falha ao executar download. Erro ao mover arquivo."
        DownloadManager.setLastError(DWN_ERR_SRC_FINISHED_DOWNLOAD, DWN_ERR_0046, DWN_ERR_MSG_0046);
        download.setError(DWN_ERR_0046, DWN_ERR_MSG_0046); 
        return false;
    }
    else
    {
        if(!saveHasDownloadInProgress(false))
        {
            //"Falha ao executar download. Erro no preferences 'downloadInProgress'."
            DownloadManager.setLastError(DWN_ERR_SRC_FINISHED_DOWNLOAD, DWN_ERR_0047, DWN_ERR_MSG_0047);
            download.setError(DWN_ERR_0047, DWN_ERR_MSG_0047); 
            return false;
        }

        if (!updateFile(download, UPDATE_TYPE_FINISHED))
        {
            //"Falha ao executar download. Erro ao atualizar arquivo finalizado."
            DownloadManager.setLastError(DWN_ERR_SRC_FINISHED_DOWNLOAD, DWN_ERR_0048, DWN_ERR_MSG_0048);
            download.setError(DWN_ERR_0048, DWN_ERR_MSG_0048);
            return false;
        }
    }

    return true;
}

///
/// GETDOWNLOAD | EXECUTEDOWNLOAD
///

/**
 * @brief Move o arquivo para uma determinada pasta.
 * 
 * @param download Informacoes do download da vez.
 * @param statusMoveTo Determina o sentido da movimentacao do arquivo.
 * @return true Moveu o arquivo com sucesso.
 * @return false Ocorreu um erro ao mover aqruivo.
 */
bool moveTo(Download &download, DownloadStatusMoveTo_e statusMoveTo)
{
    String oldPath;
    String newPath;

    String nameFile = String(download.getPriority()) + String(download.getId());

    if (statusMoveTo == MOVE_QUEUE_TO_CURRENT)
    {
        oldPath = String(QUEUE_DIR) + "/" + nameFile;
        newPath = String(CURRENT_DIR) + "/" + nameFile;
    }
    else if (statusMoveTo == MOVE_CURRENT_TO_FINISHED)
    {
        oldPath = String(CURRENT_DIR) + "/" + nameFile;
        newPath = String(FINISHED_DIR) + "/" + nameFile;
    }
    else if (statusMoveTo == MOVE_CURRENT_TO_ERROR)
    {
        oldPath = String(CURRENT_DIR) + "/" + nameFile;
        newPath = String(ERROR_DIR) + "/" + nameFile;
    }
    else if (statusMoveTo == MOVE_QUEUE_TO_ERROR)
    {
        oldPath = String(QUEUE_DIR) + "/" + nameFile;
        newPath = String(ERROR_DIR) + "/" + nameFile;
    }
    else
    {
        //"Falha ao mover arquivo. Parametro 'statusMoveTo' incorreto."
        DownloadManager.setLastError(DWN_ERR_SRC_MOVE_TO, DWN_ERR_0018, DWN_ERR_MSG_0018);
        return false;
    }

    StorageController.waitOne(portMAX_DELAY);
    if (!SD.rename(oldPath.c_str(), newPath.c_str()))
    {
        //"Falha ao mover arquivo. Caminho incorreto ao tentar mover arquivo."
        DownloadManager.setLastError(DWN_ERR_SRC_MOVE_TO, DWN_ERR_0019, DWN_ERR_MSG_0019);
        StorageController.releaseMutex();
        return false;
    }
    StorageController.releaseMutex();

    return true;
}

/**
 * @brief Atualiza as informacoes no arquivo.
 * 
 * @param download Informacoes do download da vez.
 * @param updateType Determina o tipo de atualizacao.
 * @return true Atualizacao ocorreu com sucesso.
 * @return false Ocorreu um erro ao atualizar arquivo.
 */
bool updateFile(Download &download, DownloadUpdateType_e updateType)
{
    String namePath;
    String nameFile = String(download.getPriority()) + String(download.getId());
    if (updateType == UPDATE_TYPE_IN_PROGRESS)
        namePath = String(CURRENT_DIR) + "/" + nameFile;
    else if (updateType == UPDATE_TYPE_FINISHED)
        namePath = String(FINISHED_DIR) + "/" + nameFile;
    else if (updateType == UPDATE_TYPE_ERROR)
        namePath = String(ERROR_DIR) + "/" + nameFile;
    else
    {
        //"Falha ao atualizar arquivo. Parametro 'updateType' incorreto."
        DownloadManager.setLastError(DWN_ERR_SRC_UPDATE_FILE, DWN_ERR_0020, DWN_ERR_MSG_0020);
        return false;
    }

    StaticJsonDocument<1024> downloadJson;
    String downloadJsonBuffer;

    downloadJson["fileURL"] = download.getFileURL();
    downloadJson["destinationFile"] = download.getDestinationFile();
    downloadJson["flag"] = download.getFlag();
    downloadJson["fileSize"] = download.getFileSize();
    downloadJson["idStatus"] = download.getIdError();
    downloadJson["message"] = download.getMsgError();
    downloadJson["downloaded"] = download.getDownloaded();
    downloadJson["started"] = download.getStarted();

    if (updateType != UPDATE_TYPE_IN_PROGRESS)
    {        
        String datetime = Utils.toString(VidoGsmManager.getDateTime()).c_str();
        downloadJson["finished"] = datetime;
    }

    //Salva Download no SD
    StorageController.waitOne(portMAX_DELAY);
    File file = SD.open(namePath, FILE_WRITE | O_TRUNC);

    if (!file)
    {
        //"Falha ao atualizar download. Erro ao abrir o arquivo."
        DownloadManager.setLastError(DWN_ERR_SRC_UPDATE_FILE, DWN_ERR_0021, DWN_ERR_MSG_0021);
        StorageController.releaseMutex();
        return false;
    }

    if (serializeJson(downloadJson, file) == 0)
    {
        //"Falha ao atualizar download. Erro ao serelizar arquivo JSON."
        DownloadManager.setLastError(DWN_ERR_SRC_UPDATE_FILE, DWN_ERR_0022, DWN_ERR_MSG_0022);
        StorageController.releaseMutex();
        return false;
    }

    file.close();
    StorageController.releaseMutex();

    return true;
}

/**
 * @brief Salva parametro que indica se existe um download em progresso.
 *  
 * @return true Parametro salvo com sucesso.
 * @return false Erro ao salvar parametro.
 */
bool saveHasDownloadInProgress(bool inProgress)
{
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_PREFS, false))
        return false;

    prefs.begin(NAMESPACE_PREFS, false);
    prefs.putBool(KEY_HAS_DOWNLOAD_IN_PROGRESS, inProgress);
    prefs.end();

    return true;
}


////////////////////////////////////////////////////////////////////////
//                          CLASSE DOWNLOAD MANAGER
////////////////////////////////////////////////////////////////////////

/**
 * @brief Construct a new Download Manager Class:: Download Manager Class object
 * 
 */
DownloadManagerClass::DownloadManagerClass()
{
}

///
/// BEGIN
///

/**
 * @brief Inicia o DownloadManager.
 * 
 * @return true Iniciado com sucesso.
 * @return false Ocorreu um erro ao iniciar.
 */
bool DownloadManagerClass::begin()
{    
    xSemaphoreLock = xSemaphoreCreateMutex();
    if (xSemaphoreLock == NULL)
    {
        //"Falha ao iniciar Download Manager. Semaphore 'xSemaphoreLock' nao foi criado."
        //setLastError(DWN_ERR_SRC_BEGIN, DWN_ERR_0001, DWN_ERR_MSG_0001);
        return false;
    }

    if (!StorageController.begin(GPIO_NUM_5))
    {
        //"Falha ao iniciar DownloadManager. StorageController nao foi inicializado."
        setLastError(DWN_ERR_SRC_BEGIN, DWN_ERR_0002, DWN_ERR_MSG_0002);
        return false;
    }  

    if (!createDefaultFolder())
    {
        //"Falha ao iniciar DownloadManager. Diretorio nao existe."
        setLastError(DWN_ERR_SRC_BEGIN, DWN_ERR_0003, DWN_ERR_MSG_0003);
        return false;
    }

    xSemaphore = xSemaphoreCreateMutex();
    if (xSemaphore == NULL)
    {
        //"Falha ao iniciar download Manager. Semaphore 'xSemaphore' nao foi criado."
        setLastError(DWN_ERR_SRC_BEGIN, DWN_ERR_0004, DWN_ERR_MSG_0004);
        return false;
    }

    xSemaphoreList = xSemaphoreCreateMutex();
    if (xSemaphoreList == NULL)
    {
        //"Falha ao iniciar download Manager. Semaphore 'xSemaphoreList' nao foi criado."
        setLastError(DWN_ERR_SRC_BEGIN, DWN_ERR_0005, DWN_ERR_MSG_0005);
        return false;
    }

    return isinitialized = true;
}

/**
 * @brief Verifica se as pastas padroes existem.
 * 
 * @return true Pastas existem ou foram criadas.
 * @return false Pastas nao existem e nao foram criadas.
 */
bool DownloadManagerClass::createDefaultFolder()
{
    vector<String> directory;

    directory.push_back(CURRENT_DIR);
    directory.push_back(ERROR_DIR);
    directory.push_back(FINISHED_DIR);
    directory.push_back(QUEUE_DIR);
  
    for(int i=0; i<directory.size(); i++)
    {
        String directoryName = directory.at(i);
        StorageController.waitOne(portMAX_DELAY);
        SdFile sdRoot;
        if (!sdRoot.open(directoryName.c_str())) 
        {       
            if(!SD.mkdir(directoryName.c_str()))  
            {           
                StorageController.releaseMutex();   
                return false; 
            } 
        }
        sdRoot.close();
        StorageController.releaseMutex();
    }

    return true;
}

///
/// ADD
///

/**
 * @brief Adicionar um novo download.
 * 
 * @param download Download a ser adicionado.
 * @return true O download foi adicionado com sucesso.
 * @return false Ocorreu um erro ao adicionar o download.
 */
bool DownloadManagerClass::add(Download &download)
{
    if (!isinitialized)
    {
        //"Falha ao adicionar download. Gerenciador nao inicializado."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0006, DWN_ERR_MSG_0006);
        return false;
    }

    if (!download.isInitialized())
    {
        //"Falha ao adicionar download. Download nao foi criado."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0007, DWN_ERR_MSG_0007);
        return false;
    }

    StorageController.waitOne(portMAX_DELAY);
    if(SD.exists(download.getDestinationFile().c_str()) && download.getFlag() == FLAG_NO_REPLACE)
    { 
        //"Falha ao adicionar download. Arquivo ja existe."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0008, DWN_ERR_MSG_0008); 
        StorageController.releaseMutex();
        return false;
    }
    StorageController.releaseMutex();

    int lastId;
    int queuedDownloads;
    if (!loadParameters(lastId, queuedDownloads))
    {
        //"Falha ao adicionar download. Erro ao carregar preferences."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0009, DWN_ERR_MSG_0009);
        return false;
    }

    download.setId(lastId);

    //Salva informacoes do download no SD.
    String name = String(QUEUE_DIR) + "/" + String(download.getPriority()) + String(lastId);

    StorageController.waitOne(portMAX_DELAY);
    File file = SD.open(name, FILE_WRITE);

    if (!file)
    {
        //"Falha ao adicionar download. Erro ao abrir o arquivo."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0010, DWN_ERR_MSG_0010);
        StorageController.releaseMutex();
        return false;
    }

    StaticJsonDocument<1024> downloadJson;
    String downloadJsonBuffer;

    downloadJson["fileURL"] = download.getFileURL();
    downloadJson["destinationFile"] = download.getDestinationFile() + String(lastId) + ".bin";
    //downloadJson["destinationFile"] = download.getDestinationFile();
    downloadJson["flag"] = download.getFlag();

    if (serializeJson(downloadJson, file) == 0)
    {
        //"Falha ao adicionar download. Erro ao serelizar o arquivo."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0011, DWN_ERR_MSG_0011);
        StorageController.releaseMutex();
        return false;
    }
    file.close();
    StorageController.releaseMutex();

    download.setStatus(DOWNLOAD_STATUS_WAIT);
    listDownload.push_back(download);

    //Salva parametros.
    if (!saveParameters(lastId, queuedDownloads))
    {
        //"Falha ao adicionar download. Erro ao salvar parametros no preferences."
        setLastError(DWN_ERR_SRC_ADD, DWN_ERR_0012, DWN_ERR_MSG_0012);
        return false;
    }

    return true;
}

/**
 * @brief Salva os parametros no preferences.
 * 
 * @param lastId Ultimo 'id' a ser registrado.
 * @param queuedDownloads Numero de arquivos na pasta QUEUE.
 * @return true Parametros salvos com sucesso.
 * @return false Erro ao salvar parametros.
 */
bool DownloadManagerClass::saveParameters(int lastId, int queuedDownloads)
{
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_PREFS, false))
        return false;

    prefs.putInt(KEY_LAST_DOWNLOAD_ID, lastId);
    prefs.putInt(KEY_QUEUED_DOWNLOADS, queuedDownloads);

    prefs.end();

    return true;
}

/**
 * @brief Carrega os parametros do preferences.
 * 
 * @param lastId Proximo 'id' do download.
 * @param queuedDownloads Proximo numero de arquivos na pasta QUEUE.
 * @return true Parametros carregados com sucesso.
 * @return false Erro ao carregar parametros.
 */
bool DownloadManagerClass::loadParameters(int &lastId, int &queuedDownloads)
{
    Preferences prefs;

    if (!prefs.begin(NAMESPACE_PREFS, false))
        return false;

    lastId = prefs.getInt(KEY_LAST_DOWNLOAD_ID, 0) + 1;
    queuedDownloads = prefs.getInt(KEY_QUEUED_DOWNLOADS, 0) + 1;

    prefs.end();

    return true;
}

///
/// START
///

/**
 * @brief Iniciar o download especificado.
 * 
 * @return true O download foi iniciado com sucesso.
 * @return false Ocorreu um erro ao iniciar o download.
 */
bool DownloadManagerClass::start(int idClient)
{
    if (!isinitialized)
    {
        //"Falha ao comercar download. Gerenciador nao inicializado."
        setLastError(DWN_ERR_SRC_START, DWN_ERR_0013, DWN_ERR_MSG_0013);
        return false;
    }

    bool isStarted = false;
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    isStarted = g_isStarted;
    xSemaphoreGive(xSemaphore);

    if (isStarted)
    {
        //"Falha ao comecar download. Download ja comecou."
        setLastError(DWN_ERR_SRC_START, DWN_ERR_0014, DWN_ERR_MSG_0014);
        return false;
    }

    if (idClient < 0 || idClient > 5)
    {
        //"Falha ao comecar download. IdClient invalido."
        return false;
    }

    if (!loadDownloadVector())
    {
        //"Falha ao comecar download. Nao foi possivel carregar downloads do diretorio."
        setLastError(DWN_ERR_SRC_START, DWN_ERR_0015, DWN_ERR_MSG_0015);
        return false;
    }

    m_idClient = idClient;
    xTaskCreate(vTaskDownload, "download_task", 10240, NULL, 1, NULL);

    return true;
}

/**
 * @brief Carrega downloads no vector.
 * 
 * @return true Carregou corretamente.
 * @return false Nao conseguiu cerregar.
 */
bool DownloadManagerClass::loadDownloadVector()
{
    int count = 0;
    String nameFolder = String(CURRENT_DIR);

    this->listDownload.clear();

    StorageController.waitOne(portMAX_DELAY);
    SdFile sdRoot;
    while(count != 2)
    {
        if (!sdRoot.open(nameFolder.c_str()))
        {
            StorageController.releaseMutex();
            return false;
        }

        //Verifica o download da vez.
        char name[255];
        SdFile sdFile;
        while (sdFile.openNext(&sdRoot, O_RDONLY))
        {
            sdFile.getName(name, 255);

            Download download;

            download.setPriority((Priority_e) String(name).substring(0, 1).toInt());
            download.setId(String(name).substring(1).toInt());

            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, sdFile);

            if (error)
            {
                StorageController.releaseMutex();
                return false;
            }
            else
            {
                download.setFileURL(String((const char*) doc["fileURL"]));
                download.setDestinationFile(String((const char*) doc["destinationFile"]));  
                int flag = (int) doc["flag"];
                download.setFlag((Flag_e) flag);

                if (nameFolder.compareTo(String(CURRENT_DIR)) == 0)
                {
                    download.setFileSize(doc["fileSize"]); 
                    download.setDownloaded(doc["downloaded"]);
                    download.setStarted(String((const char*) doc["started"]));            
                }  
            }        
            download.setStatus(DOWNLOAD_STATUS_WAIT);
            this->listDownload.push_back(download);            
        
            sdFile.close();
        }    

        if (sdRoot.getError())
        {
            StorageController.releaseMutex();
            return false;
        }

        sdRoot.close();

        count++;
        nameFolder = String(QUEUE_DIR);        
    }  
    StorageController.releaseMutex(); 

    return true;
}

///
/// STOP
///

/**
 * @brief Parar o download.
 * 
 * @return true Download para com sucesso.
 * @return false Ocorreu um erro ao parar o download.
 */
bool DownloadManagerClass::stop()
{
    if (!isinitialized)
    {
        //"Falha ao parar download. Gerenciador nao inicializado."
        setLastError(DWN_ERR_SRC_STOP, DWN_ERR_0016, DWN_ERR_MSG_0016);
        return false;
    }

    bool isStarted = false;
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    isStarted = g_isStarted;
    xSemaphoreGive(xSemaphore);

    if (!isStarted)
    {
        //"Falha ao parar download. Download ainda nao iniciou."
        setLastError(DWN_ERR_SRC_STOP, DWN_ERR_0017, DWN_ERR_MSG_0017);
        return false;
    }

    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    g_stopRequested = true;
    xSemaphoreGive(xSemaphore);

    return true;
}  

///
/// STATUS DOWNLOAD
///

/**
 * @brief Atualiza o status do download.
 * 
 * @param id Identificador do download.
 * @param DownloadStatus_e Status do download.
 */
void DownloadManagerClass::updateDownload(Download download)
{
    pair <int, Download> ret = searchInfo(download.getId());

    if (ret.first != -1)
    {
        xSemaphoreTake(xSemaphoreList, portMAX_DELAY);
        listDownload[ret.first] = download;  
        xSemaphoreGive(xSemaphoreList);
    }          
}

/**
 * @brief Obtem o download.
 * 
 * @param id Identificador do download.
 * @return Download Objeto do download.
 */
Download DownloadManagerClass::getDownload(int id)
{
    pair <int, Download> ret = searchInfo(id);
    return ret.second;
}

/**
 * @brief Busca o download no vector.
 * 
 * @param id Identificador do download.
 * @return pair <int, Download> Posicao e objeto do download.
 */
pair <int, Download> DownloadManagerClass::searchInfo(int id)
{
    vector<Download> lstDownload;
    xSemaphoreTake(xSemaphoreList, portMAX_DELAY);
    lstDownload = listDownload;
    xSemaphoreGive(xSemaphoreList);

    Download m_download;
    int pos = -1;
    if (!listDownload.empty())
    {
        vector<Download>::iterator it;
        pos = 0;        
        for (it = lstDownload.begin(); it != lstDownload.end(); it++, pos++)
        {
            if (id == (*it).getId())
            {
                m_download = *it;
                break;
            }
        }
    }
    return make_pair(pos, m_download);
}

///
/// ISRUNNING
///

/**
 * @brief Verificar se o Download Manager está rodando.
 * 
 * @return true Caso esteja rodando.
 * @return false Caso não esteja rodando.
 */
bool DownloadManagerClass::isRunning()
{
    bool isStarted = false;
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    isStarted = g_isStarted;
    xSemaphoreGive(xSemaphore);

    return isStarted;
}

///
/// ERROR
///

/**
 * @brief Definir o erro mais recente.
 * 
 * @param source Origem do erro.
 * @param id Id do erro.
 * @param message Mensage do erro.
 * @return esp_err_t 
 */
esp_err_t DownloadManagerClass::setLastError(const char *source, int id, const char *message)
{
    // "[%lu] Ocorreu um erro. Origem: [%s] | Codigo: %d | Mensagem: (%s)"
    ESP_LOGE(TAG_CONNMGR, DWN_ERR_MSG, millis(), source, id, message);

    xSemaphoreTake(xSemaphoreLock, portMAX_DELAY);
    lastError.Code = id;
    memset(lastError.Message, 0, sizeof(lastError.Message));
    strcpy(lastError.Message, message);
    xSemaphoreGive(xSemaphoreLock);

    return id;
}

/**
 * @brief Retornar o ultimo erro ocorrido.
 * 
 * @return LastError_t Erro ocorrido.
 */
LastError_t DownloadManagerClass::getLastError()
{
    LastError_t error;

    xSemaphoreTake(xSemaphoreLock, portMAX_DELAY);
    error.Code = lastError.Code;
    memset(error.Message, 0, sizeof(error.Message));
    strcpy(error.Message, lastError.Message);
    xSemaphoreGive(xSemaphoreLock);

    return error;
}

///
///
///
int DownloadManagerClass::getIdClient()
{
    return m_idClient;
}

DownloadManagerClass DownloadManager;