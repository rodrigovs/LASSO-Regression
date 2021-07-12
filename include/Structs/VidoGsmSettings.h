#pragma once

#include <Arduino.h>

/**
 * @brief Configurações do GSM.
 * 
 */
struct VidoGsmSettings_t
{
public:
    VidoGsmSettings_t()
    {
        reset();
    }

    VidoGsmSettings_t(const char *apn, const char *user, const char *password, uint32_t waitForNetworkTimeout = 60000, uint32_t connectionCheckInterval = 10000)
    {
        reset();

        strcpy(APN, apn);
        strcpy(User, user);
        strcpy(Password, password);
        WaitForNetworkTimeout = waitForNetworkTimeout;
        ConnectionCheckInterval = connectionCheckInterval;
    }

    void reset()
    {
        memset(APN, 0, sizeof(APN));
        memset(User, 0, sizeof(User));
        memset(Password, 0, sizeof(Password));
        WaitForNetworkTimeout = 60000;
        ConnectionCheckInterval = 10000;
    }

    void copyTo(VidoGsmSettings_t &settings)
    {
        memset(settings.APN, 0, sizeof(settings.APN));
        memset(settings.User, 0, sizeof(settings.User));
        memset(settings.Password, 0, sizeof(settings.Password));

        strcpy(settings.APN, APN);
        strcpy(settings.User, User);
        strcpy(settings.Password, Password);
        settings.WaitForNetworkTimeout = WaitForNetworkTimeout;
        settings.ConnectionCheckInterval = ConnectionCheckInterval;
    }

    /**
     * @brief APN do provedor GSM.
     * 
     */
    char APN[128];
    /**
     * @brief Usuário do provedor GSM.
     * 
     */
    char User[32];
    /**
     * @brief Senha do provedor GSM.
     * 
     */
    char Password[32];
    /**
     * @brief Timeout de espera por conexão de rede, em milissegundos.
     * 
     */
    uint32_t WaitForNetworkTimeout;
    
    /**
     * @brief Intervalo de checagem de conexão.
     * 
     */
    uint32_t ConnectionCheckInterval;
};