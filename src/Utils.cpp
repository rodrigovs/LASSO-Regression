/**
 * @file Utils.cpp
 * @author your name (you@domain.com)
 * @brief Funções úteis do sistema.
 * @version 0.2
 * @date 2019-10-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "Utils.h"

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

/**
* @brief Inicializar o objeto Utils Class.
* 
*/
UtilsClass::UtilsClass()
{
}

/**
 * @brief Retornar a temperatura do ESP32.
 * 
 * @return double Temperatura do ESP32.
 */
uint8_t UtilsClass::getESPTemperature()
{
    uint8_t temperature = (uint8_t)((temprature_sens_read() - 32) / 1.8);

    ESP_LOGD("", "[%lu] Temperatura: %d", millis(), temperature);

    return temperature;
}

/**
 * @brief Obter o arco a partir do raio da curva.
 * 
 * @param turnRadius 
 * @return double 
 */
double UtilsClass::getArch(double turnRadius) 
{
	return turnRadius / 3 * 3.1416;
}

/**
 * @brief Configurar o horário do sistema no formato Unix.
 * 
 * @param value 
 * @return esp_err_t 
 */
esp_err_t UtilsClass::setUnixTime(time_t value)
{
    struct timeval now = {.tv_sec = value};
    if (settimeofday(&now, NULL) == 0)
    {
        return ESP_OK;
    }
    else
    {
        return ESP_ERR_INVALID_RESPONSE;
    }
    
}

/**
 * @brief Obter o horário do sistema no formato Unix.
 * 
 * @return time_t 
 */
time_t UtilsClass::getUnixTime()
{
    time_t myTime;
    time(&myTime);
    
    return myTime;
}

/**
 * @brief Configurar o horário do sistema.
 * 
 * @param value 
 * @return esp_err_t 
 */
esp_err_t UtilsClass::setTime(struct tm value)
{
    ESP_LOGD("UTILS", "Configurando o horario: Data/Hora: %02d/%02d/%d %02d:%02d:%02d", 
        value.tm_mday, value.tm_mon + 1, value.tm_year + 1900, 
        value.tm_hour, value.tm_min, value.tm_sec);

    time_t t = mktime(&value);
    return setUnixTime(t);
}

/**
 * @brief Obter o horário do sistema.
 * 
 * @param value 
 * @return esp_err_t 
 */
esp_err_t UtilsClass::getTime(struct tm *value)
{
    time_t rawtime;

    rawtime = getUnixTime();
    value = localtime(&rawtime);

    return ESP_OK;
}

/**
 * @brief Converter o horário do GPS em horário do sistema.
 * 
 * @param context 
 * @return tm 
 */
tm UtilsClass::getTime(GPSContext_t context)
{
    struct tm value;
    value.tm_year = context.Date_Year - 1900;
    value.tm_mon = context.Date_Month - 1;
    value.tm_mday = context.Date_Day;
    value.tm_hour = context.Time_Hours;
    value.tm_min = context.Time_Minutes;
    value.tm_sec = context.Time_Seconds;

    ESP_LOGD(TAG_GPS, "Data/Hora: %02d/%02d/%04d %02d:%02d:%02d", 
        context.Date_Day, context.Date_Month, context.Date_Year,
        context.Time_Hours, context.Time_Minutes, context.Time_Seconds);

    return value;
}

/**
 * @brief Converter um array de bytes para uma string hexa.
 * 
 * @param inBuffer Array de bytes a ser convertido.
 * @param inputLength Número de bytes que o array contém.
 * @param outBuffer String resultante com tamanho já pré-alocado.
 * @param outputLength Tamanho da string resultante.
 * @return int Número de bytes convertidos.
 */
int UtilsClass::toHexString(uint8_t *inBuffer, int inputLength, char *outBuffer, int outputLength)
{
    int offset = 0;
    int i;

    for (i = 0; i < inputLength; i++) {
        offset += snprintf(outBuffer + offset, outputLength - offset, "%02x", inBuffer[i]);
    }

    outBuffer[offset+1] = '\0';

    return offset;
}

/**
 * @brief Converter um array de bytes para uma string hexa.
 * 
 * @param inBuffer Array de bytes a ser convertido.
 * @param inputLength Número de bytes que o array contém.
 * @return String Resultado da conversão.
 */
String UtilsClass::toHexString(uint8_t *inBuffer, int inputLength)
{
    String result = "";
    char aux[3];

    for (int i = 0; i < inputLength; i++) 
    {
        memset(aux, 0, sizeof(aux));
        sprintf(aux, "%02X", inBuffer[i]);
        result += String(aux);
    }

    return result;
}

/**
 * @brief Obter um array de bytes a partir de uma string hexa.
 * 
 * @param hexstr 
 * @return uint8_t* 
 */
uint8_t* UtilsClass::fromHexString(const char* hexstr)
{
    size_t len = strlen(hexstr);
    if(len % 2 != 0)
        return NULL;

    size_t final_len = len / 2;
    uint8_t* chrs = (unsigned char*)malloc((final_len+1) * sizeof(*chrs));
    for (size_t i=0, j=0; j<final_len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;

    chrs[final_len] = '\0';
    return chrs;
}

/**
 * @brief Converter uma data e hora para unix time.
 * 
 * @param day Dia
 * @param month Mês
 * @param year Ano
 * @param hours Hora
 * @param minutes Minuto
 * @param seconds Segundo
 * @return time_t Data e hora no formato POSIX.
 */
time_t UtilsClass::toUnixTime(uint8_t day, uint8_t month, uint16_t year, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    struct tm timevalue;

    timevalue.tm_mday = day;
    timevalue.tm_mon = month - 1;
    timevalue.tm_year = year - 1900;
    timevalue.tm_hour = hours;
    timevalue.tm_min = minutes;
    timevalue.tm_sec = seconds;

    return mktime(&timevalue);
}

/**
 * @brief Converter uma data/hora proveniente do GPS para unix time.
 * 
 * @param context Contexto do GPS.
 * @return time_t Data e hora no formato POSIX.
 */
time_t UtilsClass::toUnixTime(GPSContext_t context)
{
    return toUnixTime(context.Date_Day, context.Date_Month, context.Date_Year, context.Time_Hours, context.Time_Minutes, context.Time_Seconds);
}

/**
 * @brief Transformar um uint16_t em 2 bytes e copiar para dentro de um array.
 * 
 * @param sourceValue 
 * @param destinationBuffer 
 * @param destinationOffset 
 */
void UtilsClass::copy2BytesTo(uint16_t sourceValue, uint8_t *destinationBuffer, int *destinationOffset)
{
	destinationBuffer[(*destinationOffset)++] = (sourceValue >> 8) & 0xFF;
	destinationBuffer[(*destinationOffset)++] = sourceValue & 0xFF;
}

/**
 * @brief Transformar um int em 4 bytes e copiar para dentro de um array.
 * 
 * @param sourceValue 
 * @param destinationBuffer 
 * @param destinationOffset 
 */
void UtilsClass::copy4BytesTo(int sourceValue, uint8_t *destinationBuffer, int *destinationOffset)
{
	destinationBuffer[(*destinationOffset)++] = (sourceValue >> 24) & 0xFF;
	destinationBuffer[(*destinationOffset)++] = (sourceValue >> 16) & 0xFF;
	destinationBuffer[(*destinationOffset)++] = (sourceValue >> 8) & 0xFF;
	destinationBuffer[(*destinationOffset)++] = sourceValue & 0xFF;
}

/**
 * @brief Converter um array de bytes em um uint16_t.
 * 
 * @param buffer 
 * @param offset 
 * @return uint16_t 
 */
uint16_t UtilsClass::getShort(uint8_t * buffer, int offset)
{
	return (buffer[offset] << 8) + (buffer[offset+1]);
}

/**
 * @brief Traduzir a qualidade de sinal em uma informação fácil de entender.
 * 
 * @param signalQualityIndex Índice da qualidade de sinal.
 * @return String Informação sobre a qualidade de sinal.
 */
String UtilsClass::parseGSMSignalQuality(int16_t signalQualityIndex)
{
    switch(signalQualityIndex)
    {
        case 0:
            return "Sem sinal";
        case 1:
            return "Muito fraco (RSSI: -111)";
        default:
        {
            int rssi = 0;
            if (signalQualityIndex <= 30)
                rssi = -110 + (2 * (signalQualityIndex - 2));
            else
                rssi = -120;

            String result = "";

            if (rssi >= -70)
                result = "Excelente";
            else if ((rssi < -70) && (rssi >= -85))
                result = "Bom";
            else if ((rssi < -85) && (rssi >= -100))
                result = "Razoavel";
            else 
                result = "Ruim";

            result += " (RSSI: " + String(rssi) + ")";

            return result;
        }
    }
}

String UtilsClass::toString(tm dateTime)
{
    String result = "";

    result += dateTime.tm_mday < 10 ? "0" : "";
    result += String(dateTime.tm_mday);
    result += dateTime.tm_mon < 10 ? "/0" : "/";
    result += String(dateTime.tm_mon + 1);
    result += "/";
    result += String(dateTime.tm_year + 1900);
    result += dateTime.tm_hour < 10 ? " 0" : " ";
    result += String(dateTime.tm_hour);
    result += dateTime.tm_min < 10 ? ":0" : ":";
    result += String(dateTime.tm_min);
    result += dateTime.tm_sec < 10 ? ":0" : ":";
    result += String(dateTime.tm_sec);

    return result;
}

UtilsClass Utils;