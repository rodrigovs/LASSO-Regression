#include "VidoGsmClientSim800.h"

/**
 * @brief Inicializar o client.
 * 
 * @param modem Modem utilizado pelo client.
 * @param mux Id do mux.
 * @return true Caso a inicialização seja bem sucedida.
 * @return false Caso contrário.
 */
bool VidoGsmClientSim800::init(TinyGsmSim800* modem, uint8_t mux)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::init");
    bool result = GsmClientSim800::init(modem, mux);
    m_mux = mux;
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::init");

    return result;
}

/**
 * @brief Conectar a um host remoto.
 * 
 * @param host Host remoto.
 * @param port Porta do host.
 * @param timeout_s Timeout de conexão.
 * @return int 1 caso tenha conectado, 0 caso contrário.
 */
int VidoGsmClientSim800::connect(const char* host, uint16_t port, int timeout_s)
{
    ESP_LOGD("", "[%lu] Conectando o client %d ao host %s porta %d timeout %d", millis(), getMux(), host, port, timeout_s);

    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::connect");
    ESP_LOGD("", "[%lu] Iniciando conexao...", millis());
    int result = GsmClientSim800::connect(host, port, timeout_s);
    ESP_LOGD("", "[%lu] Conexao realizada. Resultado: %d...", millis(), result);
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::connect");

    return result;
}


/**
 * @brief Conectar a um host remoto.
 * 
 * @param host Host remoto.
 * @param port Porta do host.
 * @return int 1 caso tenha conectado, 0 caso contrário.
 */
int VidoGsmClientSim800::connect(const char* host, uint16_t port)
{
    return connect(host, port, 120);
}


void VidoGsmClientSim800::stopSafe(uint32_t maxWaitMs)
{
    ESP_LOGD("", "[%lu] Parando o client", millis());
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::stopSafe");
    GsmClientSim800::stop(maxWaitMs);
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::stopSafe");
}

void VidoGsmClientSim800::stopSafe()
{
    ESP_LOGD("", "[%lu] Parando o client", millis());
    stopSafe(15000L);
}

int VidoGsmClientSim800::getMux()
{
    return m_mux;
}

/**
 * @brief Verificar se o client está conectado no host remoto.
 * 
 * @return uint8_t Retorna 1 se estiver conectado ou 0 caso contrário.
 */
uint8_t VidoGsmClientSim800::connected()
{
    uint8_t result = 0;

    ESP_LOGD("", "[%lu] Verificando se o client %d esta conectado", millis(), getMux());

    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::connected");
    result = GsmClientSim800::connected();
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::connected");

    return result;
}

/**
 * @brief 
 * 
 * @param buf 
 * @param size 
 * @return size_t 
 */
size_t VidoGsmClientSim800::write(const uint8_t* buf, size_t size)
{
    size_t result = 0;

    ESP_LOGD("", "[%lu] Escrevendo no client", millis());
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::write");
    result = GsmClientSim800::write(buf, size);
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::write");

    return result;
}

/**
 * @brief 
 * 
 * @param str 
 * @return size_t 
 */
size_t VidoGsmClientSim800::write(uint8_t c)
{
    return write(&c, 1);
}

/**
 * @brief 
 * 
 * @param str 
 * @return size_t 
 */
size_t VidoGsmClientSim800::write(const char* str)
{
    return write((const uint8_t*)str, strlen(str));
}

/**
 * @brief 
 * 
 * @param buf 
 * @param size 
 * @return int 
 */
int VidoGsmClientSim800::read(uint8_t *buf, size_t size)
{
    int result = 0;

    ESP_LOGD("", "[%lu] Lendo do client", millis());
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::read");
    result = GsmClientSim800::read(buf, size);
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::read");

    return result;
}

/**
 * @brief 
 * 
 * @return int 
 */
int VidoGsmClientSim800::read()
{
    uint8_t c;
    if (read(&c, 1) == 1) { return c; }
    return -1;
}

/**
 * @brief 
 * 
 */
bool VidoGsmClientSim800::httpGet(String host, String resource)
{   

    ESP_LOGD("", "## Baixando Arquivo: [%lu] Client Mux: %d | Connected: %d ##", millis(), getMux(), connected());
    
                // //"https://anjoapi01.azurewebsites.net/api/Firmwares/last?serialNumber=99000001"
                // if (VidoGsmManager.client(id)->connect("anjoapi01.azurewebsites.net", 80, 75) <= 0)

    //write("GET / HTTP/1.1\r\n");
    String message = String("GET ") + resource + " HTTP/1.1\r\n";
    write(message.c_str());
    //write("Host: www.google.com.br\r\n");
    message = String("Host: ") + host.c_str() + "\r\n";
    write(message.c_str());
    write("Connection: close\r\n\r\n");

    ESP_LOGD("", "[%lu] Get solicitado, aguardando resposta...", millis());

    // Wait for data to arrive
    uint32_t start = millis();
    int bytesToRead = 0;
    while (millis() - start < 10000L) 
    {
        VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::available");
        bytesToRead = GsmClientSim800::available();
        VidoGsmLock.releaseMutex("VidoGsmClientSim800::available");

        ESP_LOGD("", "[%lu] Bytes disponiveis p/ leitura: %d", millis(), bytesToRead);

        if (bytesToRead > 0)
        {
            break;
        }

        delay(100);
    };

    

    if (millis() - start < 10000L)
    {
        ESP_LOGD("", "[%lu] Lendo resposta...", millis());

        // Read data
        start = millis();

        String data = "";

        while (millis() - start < 5000L) {
            
            while (availableSafe()) {
                data += String((char) read());
                
                start = millis();
            }
        }

        ESP_LOGD("", "[%lu] Dados lidos: %s", millis(), data.c_str());

        return true;
    }
    else
    {
        ESP_LOGD("", "[%lu] Sem resposta...", millis());

        return false;
    }    
}


int VidoGsmClientSim800::availableSafe()
{
    int result = 0;

    ESP_LOGD("", "[%lu] Verificando bytes disponiveis para leitura...", millis());

    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmClientSim800::available");
    result = GsmClientSim800::available();
    VidoGsmLock.releaseMutex("VidoGsmClientSim800::available");

    return result;
}