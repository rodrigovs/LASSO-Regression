/**
 * @file Download.h
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief 
 * @version 0.1
 * @date 16/04/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <Arduino.h>
#include <iostream> 

using namespace std; 

/////////////////////////////////////////////////////////////////////////
//                    DEFINIÇÕES DE MSG
/////////////////////////////////////////////////////////////////////////
#define DW_ERR_0001 0x001
#define DW_ERR_MSG_0001 "Falha ao criar download. Parametro incorreto."
#define DW_ERR_0002 0x002
#define DW_ERR_MSG_0002 "Falha ao criar download. Formato da URL incorreta, retire o 'http://' ou 'https://'."

////////////////////////////////////////////////////////////////////////
//                          ENUMS
////////////////////////////////////////////////////////////////////////
/**
 * @brief Indica o Status do Download Manager.
 * 
 */
enum DownloadStatus_e
{   
    DOWNLOAD_STATUS_NONE = 0x00,   
    DOWNLOAD_STATUS_WAIT,  
    DOWNLOAD_STATUS_DOWNLOADING,  
    DOWNLOAD_STATUS_FINISHED,
    DOWNLOAD_STATUS_ERROR    
};

enum Priority_e
{	
	PRIORITY_NONE = 1,
    PRIORITY_LOW = 2,
    PRIORITY_MEDIUM = 3,
    PRIORITY_HIGH = 4,
    PRIORITY_CRITICAL = 5 
};

enum Flag_e
{
    FLAG_NO_REPLACE = 0x00,
    FLAG_REPLACE,
    FLAG_MERGE
};

////////////////////////////////////////////////////////////////////////
//                          CLASSE
////////////////////////////////////////////////////////////////////////
class Download
{
public:
    /**
     * @brief Construct a new Download Class object
     * 
     */
    Download();
    /**
     * @brief Inicializa informacoes.
     * 
     * @param fileURL 
     * @param destinationFile 
     * @param priority 
     * @return true  
     * @return false 
     */
    bool initialize(String fileURL, String destinationFile, Priority_e priority, Flag_e flag = FLAG_NO_REPLACE);
    /**
     * @brief Adicionar o erro.
     * 
     * @param idStatus 
     * @param msgStatus 
     */
    void setError(int idStatus, String msgStatus);
    /**
     * @brief Adicionar o Id.
     * 
     * @param id 
     */
    void setId(int id);
    /**
     * @brief Adicionar a prioridade.
     * 
     * @param priority 
     */
    void setPriority(Priority_e priority);
    /**
     * @brief Adicionar flag.
     * 
     * @param flag 
     */
    void setFlag(Flag_e flag);
    /**
     * @brief Adicionar andamento do download.
     * 
     * @param downloaded 
     */
    void setDownloaded(size_t downloaded);
    /**
     * @brief Adicionar tamanho do arquivo de download.
     * 
     * @param fileSize 
     */
    void setFileSize(size_t fileSize);
    /**
     * @brief Adicionar data/hora de inicializacao do download.
     * 
     * @param started 
     */
    void setStarted(String started);
    /**
     * @brief Adicionar a URL do download.
     * 
     * @param fileURL 
     */
    void setFileURL(String fileURL);
    /**
     * @brief Adicionar o arquivo de destino do download.
     * 
     * @param destinationFile 
     */
    void setDestinationFile(String destinationFile);
    /**
     * @brief Adicionar nome do diretório atual do download.
     * 
     * @param directoryName 
     */
    void setDirectoryName(String directoryName);
    /**
     * @brief Adicionar status do download.
     * 
     * @param downloadStatus 
     */
    void setStatus(DownloadStatus_e status);
    /**
     * @brief Retornar Id do download.
     * 
     * @return int Id do download.
     */
    int getId();
    /**
     * @brief Retornar Id do erro.
     * 
     * @return int Id do erro.
     */
    int getIdError();
    /**
     * @brief Verificar inicializacao.
     * 
     * @return true Inicializado com sucesso.
     * @return false Erro ao inicializar.
     */
    bool isInitialized();
    /**
     * @brief Retornar prioridade.
     * 
     * @return Priority_e Pro
     */
    Priority_e getPriority();
    /**
     * @brief Get the Flag object
     * 
     * @return Flag_e 
     */
    Flag_e getFlag();
    /**
     * @brief Get the Downloaded object
     * 
     * @return size_t 
     */
    size_t getDownloaded();
    /**
     * @brief Get the File Size object
     * 
     * @return size_t 
     */
    size_t getFileSize();
    /**
     * @brief Get the Msg Status object
     * 
     * @return String 
     */
    String getMsgError();
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    String getStarted();
    /**
     * @brief Get the File URL object
     * 
     * @return String 
     */
    String getFileURL();
    /**
     * @brief Get the Destination File object
     * 
     * @return String 
     */
    String getDestinationFile(); 
    /**
     * @brief Get the Directory Name object
     * 
     * @return String 
     */
    String getDirectoryName(); 
    /**
     * @brief Get the Download Status object
     * 
     * @return DownloadStatus_e 
     */
    DownloadStatus_e getStatus();
private:
    int id;
    int idError;
    bool isinitialized;
    Priority_e priority;
    Flag_e flag;
    size_t downloaded;
    size_t fileSize;
    String msgError;
    String started;
    String fileURL;
    String destinationFile;
    String directoryName;
    DownloadStatus_e status;
};