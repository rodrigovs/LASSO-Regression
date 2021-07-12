/**
 * @file CommonStorage.cpp
 * @author your name (you@domain.com)
 * @brief Realiza o controle do sistema de armazenamento.
 * @version 0.1
 * @date 2019-11-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "CommonStorage.h"

static SemaphoreHandle_t m_semaphoreStorageController;

/**
 * @brief Inicializar o objeto StorageControllerClass.
 * 
 */
StorageControllerClass::StorageControllerClass()
{
    m_writeErrorCount = 0;
    m_readErrorCount = 0;
    m_freeSpace = 0;

    if (m_semaphoreStorageController == nullptr)
    {
        ESP_LOGD("", "[%ld] Criando semaphoro do StorageController", millis());
        m_semaphoreStorageController = xSemaphoreCreateMutex();            
    }
}

/**
 * @brief Inicializar o gerenciador de armazenamento.
 * 
 * @param gpioNumber Número do GPIO onde o SD está conectado.
 * @return true Caso a inicialização tenha sucesso.
 * @return false Caso a inicialização falhe.
 */
bool StorageControllerClass::begin(gpio_num_t gpioNumber)
{
    m_gpioNumber = gpioNumber;

    CacheStatic.SystemStatus.SDWorking = SD.begin(gpioNumber, SDSPEED);

    if (!CacheStatic.SystemStatus.SDWorking)
    {
        // Falha ao montar o SD card.
        ESP_LOGE("", SCC_LOGE_0001);
    }
    else
    {
        getFreeSpace(false);
    }

    return CacheStatic.SystemStatus.SDWorking;
}

/**
 * @brief Incrementar o contador de erros de escrita.
 * 
 */
void StorageControllerClass::incrementWriteErrorCount()
{
    //ESP_LOGD("", "[%lu] Erros de escrita: %d | Codigo Erro: %d", millis(), m_writeErrorCount + 1, SD.cardErrorCode());
    m_writeErrorCount++;

    if (m_writeErrorCount > 5)
    {
        resetStorage();
    }
}

/**
 * @brief Incrementar o contador de erros de leitura.
 * 
 */
void StorageControllerClass::incrementReadErrorCount()
{
    //ESP_LOGD("", "[%lu] Erros de leitura: %d | Codigo Erro: %d", millis(), m_readErrorCount + 1, SD.cardErrorCode());
    m_readErrorCount++;

    if (m_readErrorCount > 5)
    {
        resetStorage();
    }
}

/**
 * @brief Retornar a quantidade de erros de escrita.
 * 
 * @return int 
 */
int StorageControllerClass::getWriteErrorCount()
{
    return m_writeErrorCount;
}

/**
 * @brief Retornar a quantidade de erros de leitura.
 * 
 * @return int 
 */
int StorageControllerClass::getReadErrorCount()
{
    return m_readErrorCount;
}

/**
 * @brief Resetar o contador de erros de escrita.
 * 
 */
void StorageControllerClass::resetWriteErrorCount()
{
    m_writeErrorCount = 0;
}

/**
 * @brief Resetar o contador de erros de leitura.
 * 
 */
void StorageControllerClass::resetReadErrorCount()
{
    m_readErrorCount = 0;
}

/**
 * @brief Retornar o espaço livre no microSD.
 * 
 * @return int Espaço livre, em MB.
 */
void StorageControllerClass::resetStorage()
{
    // Resetando o armazenamento.
    ESP_LOGD("", SCC_LOGD_0001);

    if (!SD.begin(m_gpioNumber, SDSPEED))
    {
        // Falha ao montar o SD card.
        ESP_LOGE("", SCC_LOGE_0001);
        CacheStatic.SystemStatus.SDWorking = false;
    }
    else
    {
        // Resetando o contador de erros.
        ESP_LOGD("", SCC_LOGD_0002);
        resetWriteErrorCount();
    }
}

/**
 * @brief Retornar o espaço livre no microSD.
 * 
 * @param useCache Define se é para usar o cache ou ler do SD novamente.
 * 
 * @return int Espaço livre, em MB.
 */
int StorageControllerClass::getFreeSpace(bool useCache)
{
    if (useCache)
        return m_freeSpace;

    uint32_t volFree = SD.vol()->freeClusterCount();
    float freeSpace = 0.000512 * volFree * SD.vol()->blocksPerCluster();
    m_freeSpace = (int)freeSpace;

    return m_freeSpace;
}

bool StorageControllerClass::appendFile(String fileName, String content, size_t &bytesAppended)
{
    bool result = false;

    xSemaphoreTake(m_semaphoreStorageController, portMAX_DELAY);

    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        //ESP_LOGE(TAG_SDCARD, LOGE_0012, fileName.c_str());
        incrementWriteErrorCount();
        result = false;
    }
    else
    {
        resetWriteErrorCount();
        bytesAppended = file.print(content.c_str());
        if (bytesAppended <= 0)
            ESP_LOGE(TAG_SDCARD, SCC_LOGE_0003);

        file.close();
        result = true;
    }

    xSemaphoreGive(m_semaphoreStorageController);
    return result;
}

bool StorageControllerClass::writeFile(String fileName, String content, size_t &bytesWritten)
{
    bool result = false;

    xSemaphoreTake(m_semaphoreStorageController, portMAX_DELAY);

    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        //ESP_LOGE(TAG_SDCARD, LOGE_0012, fileName.c_str());
        incrementWriteErrorCount();
    }
    else
    {
        resetWriteErrorCount();
        bytesWritten = file.write(content.c_str());
        if (bytesWritten <= 0)
            ESP_LOGE(TAG_SDCARD, SCC_LOGE_0003);

        file.close();
        result = true;
    }

    xSemaphoreGive(m_semaphoreStorageController);
    return result;
}

bool StorageControllerClass::moveFile(String sourcePath, String destinationPath, bool overwrite)
{
    bool result = false;

    xSemaphoreTake(m_semaphoreStorageController, portMAX_DELAY);

    if (SD.exists(destinationPath.c_str()))
    {
        if (overwrite)
            SD.remove(destinationPath.c_str());
        else
        {
            ESP_LOGE(TAG_SDCARD, "O arquivo %s nao pode ser sobrescrito.", destinationPath.c_str());
            xSemaphoreGive(m_semaphoreStorageController);
            return false;
        }
        
    }

    result = SD.rename(sourcePath.c_str(), destinationPath.c_str());

    xSemaphoreGive(m_semaphoreStorageController);

    return result;
}

void StorageControllerClass::waitOne(TickType_t timeout)
{
    xSemaphoreTake(m_semaphoreStorageController, timeout);
}

void StorageControllerClass::releaseMutex()
{
    xSemaphoreGive(m_semaphoreStorageController);
}

StorageControllerClass StorageController;
SdFat SD;