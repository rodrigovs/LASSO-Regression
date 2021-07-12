/**
 * @file Utils.h
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief Funções úteis do sistema.
 * @version 0.2
 * @date 03/09/2019
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#pragma once

#include <Arduino.h>
#include <sys/time.h>
#include "StructLibrary.h"

/**
 * @brief Contém funções de sistema e conversões.
 * 
 */
class UtilsClass
{
public:
    /**
    * @brief Inicializar o objeto Utils Class.
    * 
    */
    UtilsClass();
    /**
     * @brief Retornar a temperatura do ESP32.
     * 
     * @return uint8_t Temperatura do ESP32 em Fahrenheit.
     */
    uint8_t getESPTemperature();
    /**
     * @brief Obter o arco a partir do raio da curva.
     * 
     * @param turnRadius 
     * @return double 
     */
    double getArch(double turnRadius);
    /**
     * @brief Configurar o horário do sistema no formato Unix.
     * 
     * @param value 
     * @return esp_err_t 
     */
    esp_err_t setUnixTime(time_t value);
    /**
     * @brief Obter o horário do sistema no formato Unix.
     * 
     * @return time_t 
     */
    time_t getUnixTime();
    /**
     * @brief Configurar o horário do sistema.
     * 
     * @param value 
     * @return esp_err_t 
     */
    esp_err_t setTime(struct tm value);
    /**
     * @brief Obter o horário do sistema.
     * 
     * @param value 
     * @return esp_err_t 
     */
    esp_err_t getTime(struct tm *value);
    /**
     * @brief Converter o horário do GPS em horário do sistema.
     * 
     * @param context 
     * @return tm 
     */
    tm getTime(GPSContext_t context);
    /**
     * @brief Converter um array de bytes para uma string hexa.
     * 
     * @param inBuffer Array de bytes a ser convertido.
     * @param inputLength Número de bytes que o array contém.
     * @param outBuffer String resultante com tamanho já pré-alocado.
     * @param outputLength Tamanho da string resultante.
     * @return int Número de bytes convertidos.
     */
    int toHexString(uint8_t *byte_array, int byte_array_len, char *hexstr, int hexstr_len);
    /**
     * @brief Converter um array de bytes para uma string hexa.
     * 
     * @param inBuffer Array de bytes a ser convertido.
     * @param inputLength Número de bytes que o array contém.
     * @return String Resultado da conversão.
     */
    String toHexString(uint8_t *inBuffer, int inputLength);
    /**
     * @brief Obter um array de bytes a partir de uma string hexa.
     * 
     * @param hexstr 
     * @return uint8_t* 
     */
    uint8_t *fromHexString(const char *hexstr);
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
    time_t toUnixTime(uint8_t day, uint8_t month, uint16_t year, uint8_t hours, uint8_t minutes, uint8_t seconds);
    /**
     * @brief Converter uma data/hora proveniente do GPS para unix time.
     * 
     * @param context Contexto do GPS.
     * @return time_t Data e hora no formato POSIX.
     */
    time_t toUnixTime(GPSContext_t context);
    /**
     * @brief Transformar um uint16_t em 2 bytes e copiar para dentro de um array.
     * 
     * @param sourceValue 
     * @param destinationBuffer 
     * @param destinationOffset 
     */
    void copy2BytesTo(uint16_t sourceValue, uint8_t *destinationBuffer, int *destinationOffset);
    /**
     * @brief Transformar um int em 4 bytes e copiar para dentro de um array.
     * 
     * @param sourceValue 
     * @param destinationBuffer 
     * @param destinationOffset 
     */
    void copy4BytesTo(int sourceValue, uint8_t *destinationBuffer, int *destinationOffset);
    /**
     * @brief Converter um array de bytes em um uint16_t.
     * 
     * @param buffer 
     * @param offset 
     * @return uint16_t 
     */
    uint16_t getShort(uint8_t *buffer, int offset);
        
    /**
     * @brief Traduzir a qualidade de sinal GSM em uma informação fácil de entender.
     * 
     * @param signalQualityIndex Índice da qualidade de sinal.
     * @return String Informação sobre a qualidade de sinal.
     */
    String parseGSMSignalQuality(int16_t signalQualityIndex);

    String toString(tm dateTime);

private:
    struct tm m_currentTime;
};

extern UtilsClass Utils;