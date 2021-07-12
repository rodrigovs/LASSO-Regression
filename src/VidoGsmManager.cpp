#include "VidoGsmManager.h"

// Defines
#define DEBUG_GSM
#define SerialAT Serial1

// Constantes
const gpio_num_t GSM_TX = GPIO_NUM_13;
const gpio_num_t GSM_RX = GPIO_NUM_14;

// Variáveis
#ifdef DEBUG_GSM
#include <StreamDebugger.h>
#define SerialMon Serial
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
LastError g_lastError;
TaskHandle_t m_gsmMonitorTaskHandle;
static SemaphoreHandle_t g_semaphoreGSM;
static volatile bool g_isGsmMonitorRunning = false;
static volatile bool g_stopRequested = false;
time_t g_gprsConnectedTime;    

void GsmMonitor(void *args)
{
    String taskName = "GsmMonitor";
    time_t gprsConnectionStartTime = 0;
    VidoGsmSettings_t settings = VidoGsmManager.getSettings();
    bool stopRequested = false;

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    g_isGsmMonitorRunning = true;
    xSemaphoreGive(g_semaphoreGSM);

    while(true)
    {
        xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
        stopRequested = g_stopRequested;
        g_stopRequested = false;
        xSemaphoreGive(g_semaphoreGSM);

        if (stopRequested)
        {
            ESP_LOGD("", "Finalizando o monitoramento GSM");
            break;
        }

        ESP_LOGD("", "[%s] Verificando se esta conectado o GPRS", taskName.c_str());
        if (modem.isGprsConnected() == false)
        {
            xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
            g_gprsConnectedTime = 0;
            xSemaphoreGive(g_semaphoreGSM);

            ESP_LOGD("", "[%s] Nao conectado GPRS. Verificando rede...", taskName.c_str());
            bool hasNetwork = modem.isNetworkConnected();
            if (!hasNetwork)
            {
                ESP_LOGD("", "[%s] Sem rede. Aguardando rede por ate %lu ms...", taskName.c_str(), settings.WaitForNetworkTimeout);
                hasNetwork = modem.waitForNetwork(settings.WaitForNetworkTimeout);
                if (!hasNetwork)
                {
                    ESP_LOGD("", "[%s] Timeout aguardando rede. Resetando modem...", taskName.c_str());
                    if (!modem.restart())
                    {
                        VidoGsmManager.setLastError(LastError(GSM_SRC_MONITOR, GSM_ERR_0003, GSM_ERR_0003_MSG));
                    }
                }
            }

            if (hasNetwork)
            {
                ESP_LOGD("", "[%s] Conectando o GPRS...", taskName.c_str());
                if (!modem.gprsConnect(settings.APN, settings.User, settings.Password))
                {
                    ESP_LOGD("", "[%s] Falha ao conectar o GPRS...", taskName.c_str());
                    VidoGsmManager.setLastError(LastError(GSM_SRC_MONITOR, GSM_ERR_0002, GSM_ERR_0002_MSG));
                }
                else
                {
                    ESP_LOGD("", "[%s] GPRS conectado!...", taskName.c_str());
                    gprsConnectionStartTime = millis();
                }
            }   
        }
        else
        {
            xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
            g_gprsConnectedTime = millis() - gprsConnectionStartTime;
            xSemaphoreGive(g_semaphoreGSM);
            
            ESP_LOGD("", "[%s] GPRS conectado ha %lu ms...", taskName.c_str(), millis() - gprsConnectionStartTime);
        }
        
        delay(10000);
    }

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    g_isGsmMonitorRunning = false;
    xSemaphoreGive(g_semaphoreGSM);

    vTaskDelete(m_gsmMonitorTaskHandle);
}

/**
 * @brief Construtor da classe VidoGsmManagerClass.
 * 
 */
VidoGsmManagerClass::VidoGsmManagerClass()
{
}

/**
 * @brief Inicializar o gerenciador Gsm.
 * 
 * @param settings Configurações Gsm.
 */
void VidoGsmManagerClass::begin(VidoGsmSettings_t settings)
{
    m_existingClients = vector<int>();

    SerialAT.begin(115200, SERIAL_8N1, GSM_TX, GSM_RX, false);
    delay(500);

    g_semaphoreGSM = xSemaphoreCreateMutex();
    g_isGsmMonitorRunning = false;

    setLastError(LastError());
    settings.copyTo(m_settings);
}

esp_err_t VidoGsmManagerClass::start()
{
    bool isRunning = false;

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    isRunning = g_isGsmMonitorRunning;
    xSemaphoreGive(g_semaphoreGSM);

    if (isRunning)
    {
        setLastError(LastError(GSM_SRC_START, GSM_ERR_0001, GSM_ERR_0001_MSG));
        return ESP_ERR_INVALID_STATE;
    }

    xTaskCreate(GsmMonitor, "GsmMonitor", 10240, NULL, NULL, &m_gsmMonitorTaskHandle);

    return ESP_OK;
}

esp_err_t VidoGsmManagerClass::stop()
{
    bool isRunning = false;

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    isRunning = g_isGsmMonitorRunning;
    xSemaphoreGive(g_semaphoreGSM);

    if (!isRunning)
        return ESP_ERR_INVALID_STATE;

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    g_stopRequested = true;
    xSemaphoreGive(g_semaphoreGSM);

    return ESP_OK;
}

bool VidoGsmManagerClass::isRunning()
{
    ESP_LOGD("", "[%lu] Verificando se esta rodando...", millis());
    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    bool result = g_isGsmMonitorRunning;
    xSemaphoreGive(g_semaphoreGSM);

    return result;
}

/**
 * @brief Verificar se o GSM está registrado na rede.
 * 
 * @return true Caso esteja registrado.
 * @return false Caso contrário
 * @remarks Este comando verifica se o GSM está registrado na rede, ou seja, se tem sinal suficiente e uma antena (ERB) próxima.
 *          Caso o modem esteja com restrição na Anatel, o GSM não se registrará.
 *          Comandos AT: AT+CREG?
 */
bool VidoGsmManagerClass::isNetworkConnected()
{
    ESP_LOGD("", "[%lu] Verificando se esta conectado a rede...", millis());
    return modem.isNetworkConnected();
}

/**
 * @brief Verificar se o modem está conectado ao GPRS.
 * 
 * @return true Caso esteja conectado.
 * @return false Caso contrário.
 * @remarks Executa os seguintes comandos:
 * -> AT+CGATT? (verificar se está attachado o GPRS, timeout 1 segundo)
 * -> AT+CIFSR;E0 (solicitar o IP local, verificar se o IP é válido, timeout 10 segundos)
 */
bool VidoGsmManagerClass::isGprsConnected()
{
    ESP_LOGD("", "[%lu] Verificando se esta conectado ao GPRS", millis());
    return modem.isGprsConnected();
}

/**
 * @brief Obter o IP local do dispositivo.
 * 
 * @return String com o IP local.
 * @remarks Se não estiver com o GPRS conectado, retornará um IP inválido.
 * Executa os seguintes comandos:
 * -> AT+CIFSR;E0 (solicitar o IP local, verificar se o IP é válido, timeout 10 segundos)
 */
String VidoGsmManagerClass::getLocalIP()
{
    ESP_LOGD("", "[%lu] Obtendo o IP local do dispositivo", millis());
    return modem.getLocalIP();
}

/**
 * @brief Obter o CCID do SIM Card.
 * 
 * @return String com o CCID do SIM Card.
 * @remarks Se não estiver com um SIM Card, retornará vazio.
 * Executa os seguintes comandos:
 * -> AT+CCID (solicitar o CCID do SIM, timeout 1 segundo)
 */
String VidoGsmManagerClass::getSimCCID()
{
    ESP_LOGD("", "[%lu] Obtendo o CCID do SIM Card", millis());
    return modem.getSimCCID();
}

/**
 * @brief Obter o IMEI do dispositivo.
 * 
 * @return String com o IMEI do dispositivo.
 */
String VidoGsmManagerClass::getIMEI()
{
    ESP_LOGD("", "[%lu] Obtendo o IMEI do dispositivo", millis());
    return modem.getIMEI();
}

/**
 * @brief Obter qualidade do sinal.
 * 
 * @return int16_t com qualidade do sinal.
 */
int16_t VidoGsmManagerClass::getSignalQuality()
{
    ESP_LOGD("", "[%lu] Obtendo o qualidade do sinal", millis());
    return modem.getSignalQuality();
}

/**
 * @brief Obter o nivel de bateria.
 * 
 * @return int8_t com o nivel de bateria,em %.
 */
int8_t VidoGsmManagerClass::getBatteryLevel()
{
    ESP_LOGD("", "[%lu] Obtendo o nivel de bateria, em %", millis());
    return modem.getBattPercent();    
}

/**
 * @brief Obter a tensao da bateria.
 * 
 * @return double com a tensao da bateria. 
 */
double VidoGsmManagerClass::getBatteryVoltage()
{
    ESP_LOGD("", "[%lu] Obtendo a tensao da bateria", millis());
    return modem.getBattVoltage() / 1000.0F;    
}

/**
 * @brief Obter a data e hora da rede GSM.
 * 
 * @return tm com a data e hora.
 */
tm VidoGsmManagerClass::getDateTime()
{
    ESP_LOGD("", "[%lu] Obtendo a data e hora da rede GSM", millis());
    String dateTimeString = modem.getGSMDateTime(DATE_FULL);
    String aux;
    tm dateTime;

    aux = dateTimeString.substring(0,2).c_str();
    ESP_LOGD(TAG, "Year: %s", aux.c_str());
    dateTime.tm_year = atoi(aux.c_str()) + 100;

    aux = dateTimeString.substring(3,5).c_str();
    ESP_LOGD(TAG, "Month: %s", aux.c_str());
    dateTime.tm_mon = atoi(aux.c_str()) - 1;

    aux = dateTimeString.substring(6,8).c_str();
    ESP_LOGD(TAG, "Day: %s", aux.c_str());
    dateTime.tm_mday = atoi(aux.c_str());

    aux = dateTimeString.substring(9,11).c_str();
    ESP_LOGD(TAG, "Hour: %s", aux.c_str());
    dateTime.tm_hour = atoi(aux.c_str());

    aux = dateTimeString.substring(12,14).c_str();
    ESP_LOGD(TAG, "Hour: %s", aux.c_str());
    dateTime.tm_min = atoi(aux.c_str());

    aux = dateTimeString.substring(15,17).c_str();
    ESP_LOGD(TAG, "Second: %s", aux.c_str());
    dateTime.tm_sec = atoi(aux.c_str());

    return dateTime;
}

/**
 * @brief Obter a localizacao por GSM.
 * 
 * @return String com  a localizacao.
 */
String VidoGsmManagerClass::getGsmLocation()
{
    ESP_LOGD("", "[%lu] Obtendo a localizacao por GSM", millis());
    return modem.getGsmLocation();   
}

/**
 * @brief Obter a operadora.
 * 
 * @return String com a operadora.
 */
String VidoGsmManagerClass::getOperator()
{
    ESP_LOGD("", "[%lu] Obtendo a operadora", millis());
    return modem.getOperator();     
}

LastError VidoGsmManagerClass::getLastError()
{
    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    LastError error;
    g_lastError.copyTo(error);
    xSemaphoreGive(g_semaphoreGSM); 
    
    return error;
}

VidoGsmSettings_t VidoGsmManagerClass::getSettings()
{
    return m_settings;
}

void VidoGsmManagerClass::setLastError(LastError error)
{
    ESP_LOGD("", "[%lu] Erro [%s:%d]: %s", 
                                            millis(), 
                                            error.getSource().c_str(), 
                                            error.getCode(), 
                                            error.getMessage().c_str());

    xSemaphoreTake(g_semaphoreGSM, portMAX_DELAY);
    error.copyTo(g_lastError);
    xSemaphoreGive(g_semaphoreGSM);    
}

esp_err_t VidoGsmManagerClass::createClient(VidoGsmClientSim800& client)
{
    if (m_existingClients.size() >= 5)
    {
        return ESP_ERR_NOT_SUPPORTED;
    }

    int id;
    
    if (m_existingClients.size() == 0)
        id = 0;
    else
        id = m_existingClients.back() + 1;

    m_existingClients.push_back(id);
    client = VidoGsmClientSim800(modem, id);

    ESP_LOGD("", "[%lu] Client criado! Id: %d", millis(), id);
    
    return ESP_OK;
}

VidoGsmClientSim800* VidoGsmManagerClass::client(int id)
{
    if (id < 0 || id > 5)
        return NULL;
    
    ESP_LOGD("", "Obtendo o client id: %d", id);

    if (m_clients.count(id) <= 0)
    {
        ESP_LOGD("", "Criando o client id: %d", id);
        //m_clients[id] = VidoGsmClientSim800(modem, id);
        m_clients[id].init(&modem, id);
    }

    return &m_clients[id];
}

VidoGsmManagerClass VidoGsmManager;