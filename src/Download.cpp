/**
 * @file Download.cpp
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief 
 * @version 0.1
 * @date 16/04/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "Download.h"

/**
 * @brief Construct a new Download Class:: Download Class object
 * 
 */
Download::Download()
{
    //Variables
    this->id = 0;   
    this->downloaded = 0;
    this->fileSize = 0;
    this->started = "";
    this->fileURL = "";
    this->destinationFile = "";
    this->directoryName = "";
    this->priority = PRIORITY_NONE;
    //Error
    this->idError = 0;
    this->msgError = "";
    //Initialized
    this->isinitialized = false; 
    this->status = DOWNLOAD_STATUS_NONE;    
}
/**
 * @brief 
 * 
 * @param fileURL 
 * @param destinationFile 
 * @param priority 
 * @return true 
 * @return false 
 */
bool Download::initialize(String fileURL, String destinationFile, Priority_e priority, Flag_e flag)
{
    //Verifica parametros
    if (fileURL == "" || destinationFile == "" || priority < PRIORITY_NONE || priority > PRIORITY_CRITICAL || flag < FLAG_NO_REPLACE || flag > FLAG_MERGE)
    {
        //"Falha ao criar download. Parametro incorreto."
        this->setError(DW_ERR_0001, DW_ERR_MSG_0001);
        return false;
    }

    if(fileURL.startsWith(String("http://")))            
        fileURL.remove(0, 7);
    else
        if(fileURL.startsWith(String("https://")))            
            fileURL.remove(0, 8);    

    this->fileURL = fileURL;
    this->destinationFile = destinationFile;
    this->priority = priority;
    this->flag = flag;
    this->isinitialized = true;

    return true;
}
/**
 * @brief 
 * 
 * @param idStatus 
 * @param msgStatus 
 */
void Download::setError(int idError, String msgError)
{
    this->idError = idError;
    this->msgError = msgError;
}
/**
 * @brief 
 * 
 * @param id 
 */
void Download::setId(int id)
{
    this->id = id;
}
/**
 * @brief 
 * 
 * @param priority 
 */
void Download::setPriority(Priority_e priority)
{
    this->priority = priority;
}
/**
 * @brief 
 * 
 * @param flag 
 */
void Download::setFlag(Flag_e flag)
{
    this->flag = flag;
}
/**
 * @brief 
 * 
 * @param downloaded 
 */
void Download::setDownloaded(size_t downloaded)
{
    this->downloaded = downloaded;
}
/**
 * @brief 
 * 
 * @param fileSize 
 */
void Download::setFileSize(size_t fileSize)
{
    this->fileSize = fileSize;
}
/**
 * @brief 
 * 
 * @param started 
 */
void Download::setStarted(String started)
{
    this->started = started;
}
/**
 * @brief 
 * 
 * @param fileURL 
 */
void Download::setFileURL(String fileURL)
{
    this->fileURL = fileURL;
}
/**
 * @brief 
 * 
 * @param destinationFile 
 */
void Download::setDestinationFile(String destinationFile)
{
    this->destinationFile = destinationFile;
}
/**
 * @brief 
 * 
 * @param directoryName 
 */
void Download::setDirectoryName(String directoryName)
{
    this->directoryName = directoryName;
}
/**
 * @brief 
 * 
 * @param downloadStatus 
 */
void Download::setStatus(DownloadStatus_e status)
{
    this->status = status;
}
/**
 * @brief 
 * 
 * @return int 
 */
int Download::getId()
{
    return this->id;
}
/**
 * @brief 
 * 
 * @return int 
 */
int Download::getIdError()
{
    return this->idError;
}
/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool Download::isInitialized()
{
    return this->isinitialized;
}
/**
 * @brief 
 * 
 * @return Priority_e 
 */
Priority_e Download::getPriority()
{
    return this->priority;
}
/**
 * @brief 
 * 
 * @return Flag_e 
 */
Flag_e Download::getFlag()
{
    return this->flag;
}
/**
 * @brief 
 * 
 * @return size_t 
 */
size_t Download::getDownloaded()
{
    return this->downloaded;
}
/**
 * @brief 
 * 
 * @return size_t 
 */
size_t Download::getFileSize()
{
    return this->fileSize;
}
/**
 * @brief 
 * 
 * @return String 
 */
String Download::getMsgError()
{
    return this->msgError;
}
/**
 * @brief 
 * 
 * @return String 
 */
String Download::getStarted()
{
    return this->started;
}
/**
 * @brief 
 * 
 * @return String 
 */
String Download::getFileURL()
{
    return this->fileURL;
}
/**
 * @brief 
 * 
 * @return String 
 */
String Download::getDestinationFile()
{
    return this->destinationFile;
}
/**
 * @brief 
 * 
 * @return String 
 */
String Download::getDirectoryName()
{
    return this->directoryName;
}
/**
 * @brief 
 * 
 * @param downloadStatus 
 */
DownloadStatus_e Download::getStatus()
{
    return this->status;
}