/**
 * @file LogManager.cpp
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief Classe que gerencia o armazenamento em logs.
 * @version 0.2
 * @date 16/07/2018
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#include <Preferences.h>
#include "LogManager.h"

/**
 * @brief Inicializar o objeto LogManagerClass.
 * 
 */
LogManagerClass::LogManagerClass()
{
}

/**
 * @brief Inicializar o objeto com as configurações adequadas.
 * 
 * @param settings 
 */
void LogManagerClass::begin(LogSettings_t settings)
{
    m_remainingBytes = MAX_LOG_FILE_SIZE;
    m_lastLogTimeStamp = millis();

    // "Inicializando o sistema de log."
    ESP_LOGD(TAG_LOG, LOG_LOGD_0005);

    CacheStatic.SystemStatus.LogManagerWorking = loadLogFile();
}

/**
 * @brief Logar dados de telemetria.
 * 
 * @param gpsContext Contexto do GPS.
 * @param mpuContext Contexto do acelerômetro.
 */
void LogManagerClass::logData(GPSContext_t gpsContext, AccelerometerContext_t mpuContext)
{
    if ((millis() - m_lastLogTimeStamp) < LOG_INTERVAL)
        return;

    m_lastLogTimeStamp = millis();

    String result;
    String currentTime = String(Utils.getUnixTime());
    result = "#000," + currentTime + ",";
    result = result + String(gpsContext.Latitude, 6) + ",";
    result = result + String(gpsContext.Longitude, 6) + ",";
    result = result + String(gpsContext.Bearing, 2) + ",";
    result = result + String(gpsContext.Speed, 2) + ",";
    result = result + String(gpsContext.Altitude, 2) + ",";
    result = result + String(gpsContext.Satellites) + ",";
    result = result + String(gpsContext.HDOP, 2) + ",";
    result = result + String(mpuContext.Acc_X) + ",";
    result = result + String(mpuContext.Acc_Y) + ",";
    result = result + String(mpuContext.Acc_Z) + ",";
    result = result + String(mpuContext.Gyro_DPS_X) + ",";
    result = result + String(mpuContext.Gyro_DPS_Y) + ",";
    result = result + String(mpuContext.Gyro_DPS_Z) + LOG_LINE_END;

    writeLog(result);
}

/**
 * @brief Logar dados de um alerta.
 * 
 * @param gpsContext Contexto do GPS.
 * @param turn Dados da curva.
 * @param alertDistance Distância de alerta.
 * @param alertType Tipo de alerta.
 */
void LogManagerClass::logAlert(GPSContext_t gpsContext, TurnSummary_t turn, double alertDistance, PointType_e alertType)
{
    String result;
    String currentTime = String(Utils.getUnixTime());
    result = "#001," + currentTime + ",";
    result = result + String(gpsContext.Latitude, 6) + ",";
    result = result + String(gpsContext.Longitude, 6) + ",";
    result = result + String(gpsContext.Bearing, 2) + ",";
    result = result + String(gpsContext.Speed, 2) + ",";
    result = result + String(gpsContext.Altitude, 2) + ",";
    result = result + turn.TurnData.Id + ",";
    result = result + String((int)alertType) + ",";
    result = result + String((int)turn.TurnData.Type) + ",";
    result = result + String(turn.TurnData.SpeedLimit) + ",";
    result = result + String(turn.TurnPointData.Sequence) + ",";
    result = result + String(alertDistance, 2) + ",";
    result = result + String(turn.TurnPointData.DistanceToCriticalRay, 2) + LOG_LINE_END;

    writeLog(result);
}

/**
 * @brief Logar um excesso de velocidade.
 * 
 * @param gpsContext Contexto do GPS.
 */
void LogManagerClass::logOverspeedAlarm(GPSContext_t gpsContext)
{
    String result;
    String currentTime = String(Utils.getUnixTime());
    result = "#002," + currentTime + ",";
    result = result + String(gpsContext.Latitude, 6) + ",";
    result = result + String(gpsContext.Longitude, 6) + ",";
    result = result + String(gpsContext.Bearing, 2) + ",";
    result = result + String(gpsContext.Speed, 2) + ",";
    result = result + String(gpsContext.Altitude, 2) + LOG_LINE_END;

    writeLog(result);
}

/**
 * @brief Logar um evento.
 * 
 * @param event Evento a ser logado.
 * @param data Dados ligados ao evento.
 */
void LogManagerClass::logEvent(EventType_e event, String data)
{
    String content;
    String currentTime = String(Utils.getUnixTime());
    content = "#003," + currentTime + "," + String((uint8_t)event);

    if (data.length() > 0)
        content += "," + data;

    content += LOG_LINE_END;
    writeLog(content);
}

/**
 * @brief Logar uma transmissão de dados.
 * 
 * @param packet Pacote a ser transmitido.
 */
void LogManagerClass::logTransmit(TrackerPacket_t packet)
{
    int outputLen = packet.Length * 2;
    char output[outputLen + 1];
    String currentTime = String(Utils.getUnixTime());
    
    Utils.toHexString(packet.Buffer, packet.Length, output, outputLen + 1);
    
    String content = "#004," + currentTime + "," + String(output) + LOG_LINE_END;
    writeLog(content);
}


/**
 * @brief Logar dados de sistema.
 * 
 * @param info Informações de sistema.
 * @param info Status do sistema.
 */
void LogManagerClass::logSystemData(SystemInfo_t info, SystemStatus_t status)
{
    String result;
    String currentTime = String(Utils.getUnixTime());
    result = "#005," + currentTime + ",";
    result = result + String(info.UpdatedOn) + ",";
    result = result + String(info.FreeHeap) + ",";
    result = result + String(info.FreePsram) + ",";
    result = result + String(info.GSMVoltage, 2) + ",";
    result = result + String(info.Temperature) + ",";
    result = result + String(status.AudioWorking) + ",";
    result = result + String(status.GPSWorking) + ",";
    result = result + String(status.GSMWorking) + ",";
    result = result + String(status.LogManagerWorking) + ",";
    result = result + String(status.MPUWorking) + ",";
    result = result + String(status.SDWorking) + LOG_LINE_END;

    writeLog(result);
}

/**
 * @brief Converter um TurnSummary_t em String.
 * 
 * @param summary TurnSummary_t a ser convertido.
 * @return String Resultado da conversão.
 */
String LogManagerClass::toString(TurnSummary_t summary)
{
    String result = "";
    String separator = ",";

    result += summary.TurnData.Id + separator;
    result += String(summary.TurnData.SpeedLimit) + separator;
    result += String((uint8_t)summary.TurnData.Type) + separator;
    result += String(summary.TurnData.Radius) + separator;
    result += String(summary.TurnPointData.Sequence) + separator;
    result += String(summary.TurnPointData.Coordinates.toString()) + separator;
    result += String(summary.TurnPointData.DistanceToCriticalRay);

    return result;
}

/**
 * @brief Carregar o arquivo de log atual a partir do que foi armazenado em memória.
 * 
 */
bool LogManagerClass::loadLogFile()
{
    // #define LOG_LOGD_0001 "Carregando o arquivo de log."
    ESP_LOGD(TAG_LOG, LOG_LOGD_0001);

    if (CacheStatic.SystemStatus.SDWorking == false)
        return false;    

    Preferences pref;
    // "log"
    pref.begin(NAMESPACE_LOG, false);
    // "file_number"
    m_fileNumber = pref.getULong(KEY_LOG_FILE_NUMBER, 0);

    // "Numero do arquivo de log: %lu"
    ESP_LOGD(TAG_LOG, LOG_LOGD_0002, m_fileNumber);

    if (m_fileNumber == 0) // eh o primeiro, joga pra 1
    {
        m_fileNumber = 1;
        pref.putULong(KEY_LOG_FILE_NUMBER, m_fileNumber);
    }

    pref.end();

    // montar o nome do arquivo
    String fileName = getLogFileName();

    if (SD.exists(fileName.c_str()))
    {
        File file = SD.open(fileName);
        m_remainingBytes = MAX_LOG_FILE_SIZE - file.size();

        // #define LOG_LOGD_0004 "Arquivo de log aberto: %s (%lu bytes livres)"
        ESP_LOGD(TAG_LOG, LOG_LOGD_0004, fileName.c_str(), m_remainingBytes);

        file.close();
    }
    else
    {
        // #define LOG_LOGE_0001 "O arquivo de log %s nao existe."
        ESP_LOGE(TAG_LOG, LOG_LOGE_0001, fileName.c_str());
    }

    return true;
}

/**
 * @brief Retornar o nome do arquivo de log.
 * 
 * @return String 
 */
String LogManagerClass::getLogFileName()
{
    String fileName = DEFAULT_LOG_FOLDER + String(m_fileNumber) + LOG_FILE_EXTENSION;
    return fileName;
}

/**
 * @brief Criar um novo arquivo de log.
 * 
 * @return String 
 */
String LogManagerClass::createLogFile()
{
    // incrementar o numero do log
    m_fileNumber++;
    // #define LOG_LOGD_0006 "Criando um arquivo de log numero %lu."
    ESP_LOGD(TAG_LOG, LOG_LOGD_0006, m_fileNumber);

    // salvar no preferences
    Preferences pref;
    pref.begin(NAMESPACE_LOG, false);
    pref.putULong(KEY_LOG_FILE_NUMBER, m_fileNumber);
    pref.end();

    // atualizar o m_remainingBytes e o m_fileNumber
    m_remainingBytes = MAX_LOG_FILE_SIZE;

    // retornar o nome do novo arquivo de log
    return getLogFileName();
}

/**
 * @brief Escrever um conteúdo no arquivo de log atual.
 * 
 * @param content Conteúdo a ser escrito.
 */
void LogManagerClass::writeLog(String content)
{
    String fileName = getLogFileName();

    if (!CacheStatic.SystemStatus.SDWorking)
    {
        // "Nao eh possivel logar [%s]. O SD nao esta funcionando."
        ESP_LOGE(TAG_LOG, LOG_LOGE_0004, content.c_str() );
        return;
    }

    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        // LOG_LOGE_0003 - Erro ao abrir o arquivo %s no modo append.
        ESP_LOGE(TAG_LOG, LOG_LOGE_0003, fileName.c_str());
        StorageController.incrementWriteErrorCount();
    }
    else
    {
        StorageController.resetWriteErrorCount();
        
        if (m_remainingBytes < content.length())
            fileName = createLogFile();

        Serial.print(content.c_str());

        byte bytesPrinted = file.print(content.c_str());
        if (bytesPrinted <= 0)
        {
            // #define LOG_LOGE_0002 "Falha ao escrever o conteudo no arquivo de log."
            ESP_LOGE(TAG_LOG, LOG_LOGE_0002);
        }
        else
        {
            // atualizar a quantidade de bytes remanescentes
            m_remainingBytes = m_remainingBytes - bytesPrinted;
        }

        file.close();
    }
}

LogManagerClass LogManager;