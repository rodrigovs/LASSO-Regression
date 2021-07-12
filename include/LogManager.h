/**
 * @file LogManager.h
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief Classe que gerencia o armazenamento em logs.
 * @version 0.2
 * @date 16/07/2018
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#pragma once

#include <Arduino.h>
#include "CacheStatic.h"
#include "CommonStorage.h"
#include "Enums.h"
#include "StructLibrary.h"
#include "Utils.h"

#define LOG_INTERVAL 100

#define MAX_LOG_FILE_SIZE 10485760UL // Tamanho maximo do arquivo, em MB
#define LOG_LINE_END "\r\n"
#define DEFAULT_LOG_FOLDER "/LOG/"
#define LOG_FILE_EXTENSION ".txt"
#define NAMESPACE_LOG "log"
#define KEY_LOG_FILE_NUMBER "file_number"

#define TAG_LOG "[LOG]"
#define LOG_LOGD_0001 "Carregando o arquivo de log."
#define LOG_LOGD_0002 "Numero do arquivo de log: %lu"
#define LOG_LOGD_0003 "Abrindo arquivo de log %s"
#define LOG_LOGD_0004 "Arquivo de log aberto: %s (%lu bytes livres)"
#define LOG_LOGD_0005 "Inicializando o sistema de log."
#define LOG_LOGD_0006 "Criando um arquivo de log numero %lu."

#define LOG_LOGE_0001 "O arquivo de log %s nao existe."
#define LOG_LOGE_0002 "Falha ao escrever o conteudo no arquivo de log."
#define LOG_LOGE_0003 "Erro ao abrir o arquivo %s no modo append."
#define LOG_LOGE_0004 "Nao eh possivel logar [%s]. O SD nao esta funcionando."

class LogManagerClass
{
public:
    /**
     * @brief Inicializar o objeto LogManagerClass.
     * 
     */
    LogManagerClass();
    /**
     * @brief Inicializar o objeto com as configurações adequadas.
     * 
     * @param settings 
     */
    void begin(LogSettings_t settings);
    /**
     * @brief Logar dados de telemetria.
     * 
     * @param gpsContext Contexto do GPS.
     * @param mpuContext Contexto do acelerômetro.
     */
    void logData(GPSContext_t gpsContext, AccelerometerContext_t mpuContext);
    /**
     * @brief Logar dados de um alerta.
     * 
     * @param gpsContext Contexto do GPS.
     * @param turn Dados da curva.
     * @param alertDistance Distância de alerta.
     * @param alertType Tipo de alerta.
     */
    void logAlert(GPSContext_t gpsContext, TurnSummary_t turn, double alertDistance, PointType_e alertType);
    /**
     * @brief Logar um excesso de velocidade.
     * 
     * @param gpsContext Contexto do GPS.
     */
    void logOverspeedAlarm(GPSContext_t gpsContext);
    /**
     * @brief Logar um evento.
     * 
     * @param event Evento a ser logado.
     * @param data Dados ligados ao evento.
     */
    void logEvent(EventType_e event, String data = "");
    /**
     * @brief Logar uma transmissão de dados.
     * 
     * @param packet Pacote a ser transmitido.
     */
    void logTransmit(TrackerPacket_t packet);
    /**
     * @brief Converter um TurnSummary_t em String.
     * 
     * @param summary TurnSummary_t a ser convertido.
     * @return String Resultado da conversão.
     */
    String toString(TurnSummary_t summary);
    /**
     * @brief Logar dados de sistema.
     * 
     * @param info Informações de sistema.
     * @param info Status do sistema.
     */
    void logSystemData(SystemInfo_t info, SystemStatus_t status);

private:    
    uint32_t m_fileNumber;              // Número do arquivo de log atual.
    uint32_t m_remainingBytes;          // Número de bytes remanescentes no arquivo atual.
    unsigned long m_lastLogTimeStamp;   // Último timeStamp do log atual.

    /**
     * @brief Carregar o arquivo de log atual a partir do que foi armazenado em memória.
     * 
     */
    bool loadLogFile();
    /**
     * @brief Retornar o nome do arquivo de log.
     * 
     * @return String 
     */
    String getLogFileName();
    /**
     * @brief Criar um novo arquivo de log.
     * 
     * @return String 
     */
    String createLogFile();
    /**
     * @brief Escrever um conteúdo no arquivo de log atual.
     * 
     * @param content Conteúdo a ser escrito.
     */
    void writeLog(String content);
};

extern LogManagerClass LogManager;