/**
 * @file VidoGsmManager.h
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief 
 * @version 0.1
 * @date 28/04/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <Arduino.h>
#include <vector>
#include <map>
#include "VidoGsmClient.h"
#include "LastError.h"
#include "Structs/VidoGsmSettings.h"

const String GSM_SRC_MONITOR = "GsmMonitor";
const String GSM_SRC_START = "start";

const int GSM_ERR_0001 = 1;
const String GSM_ERR_0001_MSG = "Erro ao iniciar. Monitoramento ja iniciado.";
const int GSM_ERR_0002 = 2;
const String GSM_ERR_0002_MSG = "Falha ao conectar o GPRS.";
const int GSM_ERR_0003 = 3;
const String GSM_ERR_0003_MSG = "Falha de comunicacao com o modem.";

using namespace std;

class VidoGsmManagerClass
{
public:
    /**
     * @brief Construtor da classe VidoGsmManagerClass.
     * 
     */
    VidoGsmManagerClass();
    /**
     * @brief Inicializar o gerenciador Gsm.
     * 
     * @param settings Configurações Gsm.
     */
    void begin(VidoGsmSettings_t settings);
    int start();
    int stop();
    bool isRunning();
    /**
     * @brief Verificar se o GSM está registrado na rede.
     * 
     * @return true Caso esteja registrado.
     * @return false Caso contrário
     * @remarks Este comando verifica se o GSM está registrado na rede, ou seja, se tem sinal suficiente e uma antena (ERB) próxima.
     *          Caso o modem esteja com restrição na Anatel, o GSM não se registrará.
     *          Comandos AT: AT+CREG?
     */
    bool isNetworkConnected();
    /**
     * @brief Verificar se o modem está conectado ao GPRS.
     * 
     * @return true Caso esteja conectado.
     * @return false Caso contrário.
     * @remarks Executa os seguintes comandos:
     * -> AT+CGATT? (verificar se está attachado o GPRS, timeout 1 segundo)
     * -> AT+CIFSR;E0 (solicitar o IP local, verificar se o IP é válido, timeout 10 segundos)
     */
    bool isGprsConnected();
    String getLocalIP();
    String getSimCCID();
    String getIMEI();
    int16_t getSignalQuality();
    int8_t getBatteryLevel();
    double getBatteryVoltage();
    tm getDateTime();
    String getGsmLocation();
    String getOperator();
    LastError getLastError();
    VidoGsmSettings_t getSettings();
    void setLastError(LastError error);
    time_t getGprsConnectedTime();
    esp_err_t createClient(VidoGsmClientSim800& client);
    VidoGsmClientSim800* client(int id);

private:
    VidoGsmSettings_t m_settings;
    bool m_isStarted;
    vector<int> m_existingClients;
    std::map<int, VidoGsmClientSim800> m_clients;
};

extern VidoGsmManagerClass VidoGsmManager;