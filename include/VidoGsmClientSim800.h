#pragma once

#include <TinyGsmClientSIM800.h>
#include "VidoGsmLock.h"

class VidoGsmClientSim800 : public TinyGsmSim800::GsmClientSim800
{
public:
    VidoGsmClientSim800(){}

    explicit VidoGsmClientSim800(TinyGsmSim800& modem, uint8_t muxId = 0) : GsmClientSim800(modem, muxId)
    {
        ESP_LOGD("", "Criando VidoGsmClientSim800. Mux: %d", mux);
        m_mux = muxId;        
    }

    bool init(TinyGsmSim800* modem, uint8_t mux = 0);

    virtual int connect(const char* host, uint16_t port, int timeout_s);

    int connect(const char* host, uint16_t port);


    void stopSafe(uint32_t maxWaitMs);

    void stopSafe();    

    int getMux();

    /**
     * @brief Verificar se o client está conectado no host remoto.
     * 
     * @return uint8_t Retorna 1 se estiver conectado ou 0 caso contrário.
     */
    uint8_t connected() override;

    int availableSafe();

    size_t write(const uint8_t* buf, size_t size);
    size_t write(uint8_t c);
    size_t write(const char* str);

    int read(uint8_t *buf, size_t size) override;
    int read() override;

    bool httpGet(String host, String resource);
private:
    int m_mux;
};