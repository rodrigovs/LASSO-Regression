/**
 * @file main.cpp
 * @author Diego França Tozetto (diego@geolumini.com.br)
 * @brief 
 * @version 0.1
 * @date 10/02/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "DownloadManager.h"

#define OPERATOR_TIM

//////////////////////////////////
//           VARIÁVEIS          //
//////////////////////////////////
#if defined(OPERATOR_TIM)
const char apn[] = "timbrasil.br";
const char user[] = "tim";
const char pass[] = "tim";
#elif defined(OPERATOR_OI)
const char apn[] = "gprs.oi.com.br";
const char user[] = "oi";
const char pass[] = "oi";
#elif defined(OPERATOR_ARQIA)
const char apn[] = "iot4u.br";
const char user[] = "";
const char pass[] = "";
#else
const char apn[] = "timbrasil.br";
const char user[] = "tim";
const char pass[] = "tim";
#endif



bool hasUpdateAvailable(int idClient, int currentVersion, FirmwareUpdateInfo_t *update);
void OTATask(void* parameters);

String introduction = "ANJO - DOWNLOAD MANAGER\n \
                      Comandos: \n \
                      -> \"ADD:<Priority>\": Adicionar download na pasta QUEUE.\n \
                      -> \"START:<IdClient>\": Iniciar os downloads adicionados.\n \
                      -> \"STOP\": Parar download dos arquivos.\n\
                      -> \"TEST_UTILS\": Testar rotinas.\n";

/**
 * @brief 
 * 
 * @param result 
 */
void OnConnected(esp_err_t result)
{
    if (result == ESP_OK)
        ESP_LOGD(TAG_MAIN, "Conectado!");
    else
        ESP_LOGE(TAG_MAIN, "Erro ao conectar: %04X", result);
}

void setup() 
{
    Serial.begin(115200);
    
    VidoGsmSettings_t settings(apn, user, pass);
    VidoGsmManager.begin(settings);

    VidoGsmManager.start();
   
    if(!DownloadManager.begin())
        ESP_LOGE("SETUP", "Detalhes[%d]: %s", DownloadManager.getLastError().Code, DownloadManager.getLastError().Message);

    ESP_LOGI("SETUP", "%s", introduction.c_str());    
}

void showStatus(DownloadStatus_e downloadStatus)
{    
    switch (downloadStatus)
    {
        case DOWNLOAD_STATUS_WAIT:
            ESP_LOGI("showStatus", "Download em espera.");
            break;
        case DOWNLOAD_STATUS_DOWNLOADING:
            ESP_LOGI("showStatus", "Download em andamento.");
            break;
        case DOWNLOAD_STATUS_FINISHED:
            ESP_LOGI("showStatus", "Download finalizado.");
            break;
        case DOWNLOAD_STATUS_ERROR:
            ESP_LOGI("showStatus", "Download com erro.");
            break;    
        default:
            break;
    }
}

void loop() 
{
    ESP_LOGD(TAG_MAIN,"Comeco do loop \n");
    //int id = 234;
    
    
    String text = Serial.readStringUntil('\n');

    if (text.length() > 0)
    {
        if (text.startsWith(String("STATUS:")))
        {
             text.remove(0, (text.indexOf(':') + 1));
            if (text.length() <= 0)
                ESP_LOGE("", "Comando invalido!");
            else
            {
                int idClient = atoi(text.c_str()); 
                showStatus(DownloadManager.getDownload(idClient).getStatus());
            }
        }
    else if (text.startsWith(String("ADD:")))
        {
            text.remove(0, (text.indexOf(':') + 1));
            if (text.length() <= 0)
                ESP_LOGE("", "Comando invalido!");
            else
            {
                int priority = atoi(text.c_str());

                Download ms_download;

                String fileURL = "https://anjostorage.blob.core.windows.net/anjoapi01/firmwares/teste/firmware.bin";
                if (!ms_download.initialize(fileURL, "/Update/firmware", (Priority_e)priority, FLAG_REPLACE))
                {
                    ESP_LOGE("Download", "Detalhes[%d]: %s", ms_download.getIdError(), ms_download.getMsgError().c_str());
                }
                else
                {
                    if (DownloadManager.add(ms_download))
                        ESP_LOGD("ADD", "Download adicionado com sucesso!");
                    else
                        ESP_LOGE("ADD", "Detalhes[%d]: %s", DownloadManager.getLastError().Code, DownloadManager.getLastError().Message);
                }
            }
        }
        else if(text.startsWith(String("START:")))
        {
            text.remove(0, (text.indexOf(':') + 1));
            if (text.length() <= 0)
                ESP_LOGE("", "Comando invalido!");
            else
            {
                int idClient = atoi(text.c_str()); 
                if(!DownloadManager.start(idClient)) 
                    ESP_LOGE("START", "Detalhes[%d]: %s", DownloadManager.getLastError().Code, DownloadManager.getLastError().Message);   
            }
        }
        else if(text.compareTo("STOP") == 0)
        {
            if(!DownloadManager.stop())
                ESP_LOGE("STOP", "Detalhes[%d]: %s", DownloadManager.getLastError().Code, DownloadManager.getLastError().Message);
        }
        else if(text.compareTo("TEST_UTILS") == 0)
        {            
            xTaskCreate(OTATask, "OTATask_task", 10240, NULL, 1, NULL);   
            //FirmwareUpdateInfo_t update;  
            //hasUpdateAvailable(0, 0, &update);                 
        }
        else
        {
            ESP_LOGE("ERROR", "Comando invalido!");
        }        
    }

    delay(10);
}

void OTATask(void* parameters)
{
    ESP_LOGD("OTATask", "Iniciando Task!");
    Download m_download;

    while (true)
    { 
        if(!DownloadManager.isRunning())
        {    
            FirmwareUpdateInfo_t update;
            int idClient = 0; 
            if(!hasUpdateAvailable(idClient, 1, &update))        
                break;        
            else
            {
                ESP_LOGD("OTATask", "Finalizou hasUpdateAvailable!");
                ESP_LOGD("OTATask", "Versao: %d | URL: %s | Tamanho: %ld", update.Version, update.URL.c_str(), update.FileSize);

                if(!m_download.initialize(update.URL, "/Update/firmware", PRIORITY_LOW, FLAG_REPLACE))
                {
                    ESP_LOGE("Download", "Detalhes[%d]: %s", m_download.getIdError(), m_download.getMsgError());
                    break;
                }
                else
                {
                    if(!DownloadManager.add(m_download))
                    {
                        ESP_LOGE("ADD", "Detalhes[%d]: %s", DownloadManager.getLastError().Code, DownloadManager.getLastError().Message);
                        break;
                    }
                    else
                    {
                        ESP_LOGD("ADD", "Download adicionado com sucesso!");
                        DownloadManager.start(idClient); 
                    }
                } 
            }
        }
        else
        { 
            if(m_download.getId() == 0)
                break;
            else
            {                       
                Download download =  DownloadManager.getDownload(m_download.getId());
                DownloadStatus_e status = download.getStatus();
                if(status == DOWNLOAD_STATUS_DOWNLOADING)
                    ESP_LOGI("showStatus", "Download em andamento.");
                else if (status == DOWNLOAD_STATUS_WAIT)
                    ESP_LOGI("showStatus", "Download em espera.");
                else if(status == DOWNLOAD_STATUS_FINISHED)
                {
                    ESP_LOGI("showStatus", "Download finalizado.");
                    ESP_LOGI("showStatus", "Andamento: %d", download.getDownloaded());
                    break;
                }
                else if(status == DOWNLOAD_STATUS_ERROR)
                {
                    ESP_LOGI("showStatus", "Download com erro.");
                    break;
                }
            }
        }    
    
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

bool hasUpdateAvailable(int idClient, int currentVersion, FirmwareUpdateInfo_t *update)
{
    bool result = false;

    String m_OTASettings_Host = "anjoapi01.azurewebsites.net";
    String URL = "/api/Firmwares/last?serialNumber=99000001";

    String httpGetResult = "";
    //esp_err_t error;
    //int retries = 0;
    int err = 0; 

    ESP_LOGI(TAG_OTA, "URL: %s", String(URL)); 

    VidoHttpClient http(*VidoGsmManager.client(idClient), m_OTASettings_Host, 80);
    
    http.beginRequest();
    err = http.get(URL);
    http.endRequest();

    if (err != 0)
    {
        //"Falha ao obter os dados da atualizacao. Falha de conexao ao servidor."
        //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0001, OTA_ERR_MSG_0001);
        result = false;
    }

    else
    {
        int status = http.responseStatusCode();
        if (!status)
        {
            if (status == -3)
            {
                //"Falha ao obter os dados da atualizacao. Ocorreu Timeout ao tentar acessar o servidor"
                //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0002, OTA_ERR_MSG_0002);
            }
            else if (status == -4)
            {
                //"Falha ao obter os dados da atualizacao. Resposta invalida do servidor"
                //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0003, OTA_ERR_MSG_0003);
            }
            else
            {
                //"Falha ao obter os dados da atualizacao. Status Code: "
                //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0004, OTA_ERR_MSG_0004 + status);
            }
            result = false;
        }
        else
        {
            ESP_LOGD(TAG_OTA, "Status Ok...");
            ESP_LOGD(TAG_OTA, "Response Headers...");

            //size_t fileSizeHeader = -1;
            while (http.headerAvailable())
            {
                String headerName = http.readHeaderName();
                String headerValue = http.readHeaderValue();
                Serial.println("    " + headerName + " : " + headerValue);
            }

            ESP_LOGD(TAG_OTA, "Headers Ok...");
            
            int contentLength = http.contentLength();
            ESP_LOGD(TAG_OTA, "Content length: %d", contentLength);

            // if (contentLength <= 0)
            // {
            //     //"Falha ao obter os dados da atualizacao. Conteudo vazio."
            //     setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0005, OTA_ERR_MSG_0005);
            //     result = false;
            // }
            // else
            // {
                String payload = http.responseBody();
                if (payload.isEmpty())
                {
                    //"Falha ao obter os dados da atualizacao. Conteudo vazio."
                    //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0005, OTA_ERR_MSG_0005);
                    result = false;
                }
                else
                {
                    ESP_LOGD(TAG_OTA, "Content Length: %d, Body Size: %d", contentLength, payload.length());
                    ESP_LOGD(TAG_OTA, "Deserializando Payload: \n%s", payload.c_str());

                    StaticJsonDocument<512> doc;
                    DeserializationError error = deserializeJson(doc, payload.c_str());
                
                    if (error)
                    {
                        //"Erro ao deserializar o conteudo."
                        //setLastError(OTA_ERR_SRC_HAS_UPDATE_AVAILABLE, OTA_ERR_0006, OTA_ERR_MSG_0006);
                        result = false;
                    }
                    else
                    {
                        update->URL = String((const char *)doc["fileURL"]);
                        update->Version = doc["versionNumber"].as<int>();
                        update->FileSize = doc["fileSize"].as<int>();
                        update->InformationReceived = true;
                        update->HasUpdate = update->Version > currentVersion;

                        // "Versao: %d | URL: %s | Tamanho: %ld"
                        ESP_LOGD(TAG_OTA, "Versao: %d | URL: %s | Tamanho: %ld", update->Version, update->URL.c_str(), update->FileSize);

                        result = true;
                    }
                }
            //}
        }
    }

    http.stop();

    return result;    
}