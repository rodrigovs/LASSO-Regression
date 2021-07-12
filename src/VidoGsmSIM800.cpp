/**
 * @file VidoGsmSIM800.cpp
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief 
 * @version 0.1
 * @date 27/04/2020
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "VidoGsmSIM800.h"

/**
 * @brief Executar o procedimento de restart do módulo GSM.
 * 
 * @return true Caso tenha sido bem sucedido.
 * @return false Caso contrário
 * @remarks Este comando testa a comunicação entre o módulo GSM e o ESP através de comandos AT.
 *          Caso tenha comunicação, desativa as funções do módulo e depois as reativa, e aí executa o init;
 *          Comandos AT: AT, AT&W, AT+CFUN=0, AT+CFUN=1,1
 */
bool VidoGsmSim800::restart()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::restart");
    bool result = TinyGsmSim800::restart();
    VidoGsmLock.releaseMutex("VidoGsmSim800::restart");

    return result;
}

/**
 * @brief Executar o procedimento de inicialização do módulo GSM.
 * 
 * @return true Caso tenha sido bem sucedido.
 * @return false Caso contrário
 * @remarks Este comando testa a comunicação entre o módulo GSM e o ESP através de comandos AT.
 *          Caso tenha comunicação, desliga o echo, habilita o time stamp local, habilita verificações de bateria, 
 * e desbloqueia o SIM card por PIN (se for o caso).
 *          Comandos AT: AT, ATE0, AT+CMEE=0, AT+CLTS=1, AT+CBATCHK=1, AT+CPIN?
 */
bool VidoGsmSim800::init()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::init");
    bool result = TinyGsmSim800::init();
    VidoGsmLock.releaseMutex("VidoGsmSim800::init");

    return result;
}

/**
 * @brief Obter informações do modem.
 * 
 * @return String com o resultado da operação.
 * @remarks Normalmente este comando retorna o modelo e a versão do módulo. Ex: SIM800 R14.18.
 *          Comandos AT: ATI
 */
String VidoGsmSim800::getModemInfo()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getModemInfo");
    String result = TinyGsmSim800::getModemInfo();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getModemInfo");

    return result;
}

/**
 * @brief Retornar o status de registro na rede.
 * 
 * @return RegStatus Status de registro na rede.
 * @remarks Este comando verifica se o GSM está registrado na rede, ou seja, se tem sinal suficiente e uma antena (ERB) próxima.
 *          Caso o modem esteja com restrição na Anatel, o GSM não se registrará.
 *          Comandos AT: AT+CREG?
 */
RegStatus VidoGsmSim800::getRegistrationStatus() 
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getRegistrationStatus");
    RegStatus result = TinyGsmSim800::getRegistrationStatus();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getRegistrationStatus");

    return result;
}

/**
 * @brief Converter o status de registro para string.
 * 
 * @param status Status de registro.
 * @return String Resultado da conversão.
 */
String VidoGsmSim800::toString(RegStatus status)
{
    switch(status)
    {
        case REG_NO_RESULT: return "No Result";
        case REG_UNREGISTERED: return "Unregistered";
        case REG_SEARCHING: return "Searching";
        case REG_DENIED: return "Denied";
        case REG_OK_HOME: return "Registered: Home";
        case REG_OK_ROAMING: return "Registered: Roaming";
        case REG_UNKNOWN: 
        default: return "Unknown";
    }
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
bool VidoGsmSim800::isNetworkConnected()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::isNetworkConnected");
    bool result = TinyGsmSim800::isNetworkConnected();
    VidoGsmLock.releaseMutex("VidoGsmSim800::isNetworkConnected");

    return result;
}

/**
 * @brief Aguardar o módulo se registrar em uma rede GSM por X tempo.
 * 
 * @param timeout_ms Tempo a aguardar para se registrar em uma rede GSM.
 * @return true Caso tenha conseguido se registrar no tempo determinado.
 * @return false Caso contrário.
 * @remarks Executa o isNetworkConnected a cada 250ms até dar o timeout.
 */
bool VidoGsmSim800::waitForNetwork(uint32_t timeout_ms)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::waitForNetwork");
    bool result = TinyGsmSim800::waitForNetwork(timeout_ms);
    VidoGsmLock.releaseMutex("VidoGsmSim800::waitForNetwork");

    return result;
}

/**
 * @brief Desligar o módulo GSM.
 * 
 * @return true Caso o módulo tenha sido desligado.
 * @return false Caso contrário.
 * @remarks Executa o comando AT+CPOWD=1 (power down, timeout 10 segundos)
 */
bool VidoGsmSim800::powerOff()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::powerOff");
    bool result = TinyGsmSim800::poweroff();
    VidoGsmLock.releaseMutex("VidoGsmSim800::powerOff");

    return result;
}

/**
 * @brief Habilitar o modo sleep do módulo.
 * 
 * @param enable True se é para habilitar, false caso contrário.
 * @return true Caso o comando tenha sido bem sucedido.
 * @return false Caso contrário.
 * @remarks Durante o sleep, o módulo SIM800 tem sua comunicação serial desabilitada.
 * Para restabelecê-la, é necessário setar o pino DRP para nível baixo por pelo menos 50ms.
 * Então use esta função para desabilitar o modo sleep. O pino DRP pode então ser liberado novamente.
 */
bool VidoGsmSim800::sleepEnable(bool enable)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::sleepEnable");
    bool result = TinyGsmSim800::sleepEnable(enable);
    VidoGsmLock.releaseMutex("VidoGsmSim800::sleepEnable");

    return result;
}

/**
 * @brief Retornar a operadora atual.
 * 
 * @return String Operadora atual.
 * @remarks Executa os seguintes comandos AT:
 * -> AT+COPS? (obter o operador de rede atual, timeout 1 segundo)
 */
String VidoGsmSim800::getOperator()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getOperator");
    String result = TinyGsmSim800::getOperator();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getOperator");

    return result;
}

/**
 * @brief Retornar a banda atual.
 * 
 * @return String banda atual.
 * @remarks Executa os seguintes comandos AT:
 * -> AT+CBAND? (obter o operador de rede atual, timeout 1 segundo)
 */
String VidoGsmSim800::getBand()
{
    String res;

    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getBand");

    sendAT(GF("+BAND?"));
    if (waitResponse(GF(GSM_NL "+CBAND:")) != 1)
    {
        res = "";
    }
    else
    {
        res = stream.readStringUntil('\n');
        waitResponse();
    }

    VidoGsmLock.releaseMutex("VidoGsmSim800::getBand");

    return res;
}

/**
 * @brief Conectar o GPRS.
 * 
 * @param apn APN da operadora.
 * @param user Usuário do APN.
 * @param pwd Senha do APN.
 * @return true Caso tenha conseguido se conectar.
 * @return false Caso contrário.
 * @remarks Este comando primeiro faz o gprsDisconnect pra garantir que está desconectado da rede.
 * Depois disso, executa os seguintes comandos AT:
 * -> AT+SAPBR=3,1,"Contype","GPRS" (definir o tipo de conexão para GPRS, timeout 1 segundo)
 * -> AT+SAPBR=3,1,"APN","<apn>" (define o APN, timeout 1 segundo)
 * -> AT+SAPBR=3,1,"USER","<user>" (define o usuário do APN, timeout 1 segundo)
 * -> AT+SAPBR=3,1,"PWD","<pwd>" (define a senha do APN, timeout 1 segundo)
 * -> AT+CGDCONT=1,"IP","<apn>" (aponta o contexto PDP para o APN, timeout 1 segundo)
 * -> AT+CGACT=1,1 (ativa o contexto PDP, timeout 60 segundos)
 * -> AT+SAPBR=1,1 (abre o contexto bearer GPRS, timeout 85 segundos)
 * -> AT+SAPBR=2,1 (lê o status do contexto bearer GPRS, timeout 30 segundos)
 * -> AT+CGATT=1 (attacha à conexão GPRS, timeout 60 segundos)
 * -> AT+CIPMUX=1 (define que o sistema irá trabalhar em multi-ip, com mux, timeout 1 segundo)
 * -> AT+CIPQSEND=1 (colocar no modo quick-send, desativando resposta 'Send OK', timeout 1 segundo)
 * -> AT+CIPRXGET=1 (colocar no modo para ler dados manualmente, timeout 1 segundo)
 * -> AT+CSTT="<apn>","<user>","<pwd>" (iniciar Task e definir APN, USER NAME e PASSWORD, timeout 60 segundos)
 * -> AT+CIICR (subir conexão GPRS ou CSD, timeout 60 segundos)
 * -> AT+CIFSR;E0 (obter IP local, timeout 10 segundos)
 * -> AT+CDNSCFG="8.8.8.8","8.8.4.4" (configurar servidores DNS do Google, timeout 1 segundo)
 */
bool VidoGsmSim800::gprsConnect(const char *apn, const char *user, const char *pwd)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::gprsConnect");
    bool result = TinyGsmSim800::gprsConnect(apn, user, pwd);
    VidoGsmLock.releaseMutex("VidoGsmSim800::gprsConnect");

    return result;
}

/**
 * @brief Desconectar o GPRS.
 * 
 * @return true Caso o GPRS tenha sido desconectado.
 * @return false Caso contrário.
 * @remarks Executa os seguintes comandos AT:
 * -> AT+CIPSHUT (shutdown da conexão TCP/IP, timeout 60 segundos)
 * -> AT+CGATT=0 (desatacha da conexão GPRS, timeout 60 segundos)
 */
bool VidoGsmSim800::gprsDisconnect()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::gprsDisconnect");
    bool result = TinyGsmSim800::gprsDisconnect();
    VidoGsmLock.releaseMutex("VidoGsmSim800::gprsDisconnect");

    return result;
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
bool VidoGsmSim800::isGprsConnected()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::isGprsConnected");
    ESP_LOGD("", "[%lu] Verificando se o GPRS esta conectado", millis());
    bool result = TinyGsmSim800::isGprsConnected();
    VidoGsmLock.releaseMutex("VidoGsmSim800::isGprsConnected");

    return result;
}

/**
 * @brief Obter o IP local do dispositivo.
 * 
 * @return String com o IP local.
 * @remarks Se não estiver com o GPRS conectado, retornará um IP inválido.
 * Executa os seguintes comandos:
 * -> AT+CIFSR;E0 (solicitar o IP local, verificar se o IP é válido, timeout 10 segundos)
 */
String VidoGsmSim800::getLocalIP()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getLocalIP");
    String result = TinyGsmSim800::getLocalIP();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getLocalIP");

    return result;
}

/**
 * @brief Obter o CCID do SIM Card.
 * 
 * @return String com o CCID do SIM Card.
 * @remarks Se não estiver com um SIM Card, retornará vazio.
 * Executa os seguintes comandos:
 * -> AT+CCID (solicitar o CCID do SIM, timeout 1 segundo)
 */
String VidoGsmSim800::getSimCCID()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getSimCCID");
    String result = TinyGsmSim800::getSimCCID();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getSimCCID");

    return result;
}

/**
 * @brief Obter o IMEI do dispositivo.
 * 
 * @return String com o IMEI do dispositivo.
 * @remarks
 */
String VidoGsmSim800::getIMEI()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getIMEI");
    String result = TinyGsmSim800::getIMEI();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getIMEI");

    return result;
}

/**
 * @brief Obter qualidade do sinal.
 * 
 * @return int16_t com qualidade do sinal.
 */
int16_t VidoGsmSim800::getSignalQuality()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getSignalQuality");
    int16_t result = TinyGsmSim800::getSignalQuality();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getSignalQuality");

    return result;
}

/**
 * @brief Obter o nivel de bateria.
 * 
 * @return int8_t com nivel de bateria, em %.
 */
int8_t VidoGsmSim800::getBattPercent()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getBattPercent");
    int8_t result = TinyGsmSim800::getBattPercent();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getBattPercent");

    return result;   
}

/**
 * @brief Obter a tensao da bateria.
 * 
 * @return uint16_t com a tensao da bateria.
 */
uint16_t VidoGsmSim800::getBattVoltage()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getBattVoltage");
    int8_t result = TinyGsmSim800::getBattVoltage();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getBattVoltage");

    return result;     
}

/**
 * @brief Obter a data e hora da rede GSM.
 * 
 * @return String com data e hora.
 */
String VidoGsmSim800::getGSMDateTime(TinyGSMDateTimeFormat format)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getGSMDateTime");
    String result = TinyGsmSim800::getGSMDateTime(format);
    VidoGsmLock.releaseMutex("VidoGsmSim800::getGSMDateTime");

    return result;       
}

/**
 * @brief Obter a localizacao por GSM.
 * 
 * @return String com a localizacao.
 */
String VidoGsmSim800::getGsmLocation()
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::getGsmLocation");
    String result = TinyGsmSim800::getGsmLocation();
    VidoGsmLock.releaseMutex("VidoGsmSim800::getGsmLocation");

    return result; 
}

/**
 * @brief Sincronizar com o relógio atômico.
 * 
 * @param server 
 * @param TimeZone 
 * @return byte 
 * @remarks Executa os seguintes comandos:
 * -> AT+CNTPCID=1 (Definir o perfil bearer do GPRS para associar com o NTP sync, timeout 10 segundos)
 * -> AT+CNTP=<server>,<timezone> (Definir servidor NTP e fuso horário, timeout 10 segundos)
 * -> AT+CNTP (Solitar sincronização de rede, timeout 10 segundos)
 */
byte VidoGsmSim800::NTPServerSync(String server, byte TimeZone)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::NTPServerSync");
    byte result = TinyGsmSim800::NTPServerSync(server, TimeZone);
    VidoGsmLock.releaseMutex("VidoGsmSim800::NTPServerSync");

    return result;
}

/*################################ FUNÇÕES RELACIONADAS COM O CLIENT ###########################################*/
/**
 * @brief Conectar o modem em um servidor de internet. Depende da conexão GPRS ativa.
 * 
 * @param host Host a se conectar.
 * @param port Porta de destino.
 * @param mux Id do mux.
 * @param ssl Flag que indica se a conexão será segura (SSL).
 * @param timeout_s Timeout de conexão, em segundos.
 * @return true Caso a conexão seja bem sucedida.
 * @return false Caso contrário.
 * 
 * @remarks Esta função é usada pelo client para se conectar em um servidor acessível via internet. Por isso, depende da conexão GPRS.
 * Ela executa os seguintes comandos AT:
 * -> AT+CIPSTART=<mux>,"TCP",<host>,<port> (iniciar uma conexão TCP ou UDP(timeout conforme parametro timeout_s)
 */
bool VidoGsmSim800::modemConnect(const char* host, uint16_t port, uint8_t mux, bool ssl, int timeout_s)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::modemConnect");
    bool result = TinyGsmSim800::modemConnect(host, port, mux, ssl, timeout_s);
    VidoGsmLock.releaseMutex("VidoGsmSim800::modemConnect");

    return result;
}

/**
 * @brief Transmitir um conjunto de dados para o servidor.
 * 
 * @param buff Conjunto de dados a ser transmitido.
 * @param len Tamanho do conjunto de dados.
 * @param mux Id do mux.
 * @return int16_t Quantidade de bytes transmitidos.
 * 
 * @remarks Esta função é usada pelo client para enviar daods para um servidor acessível via internet. Por isso, depende da conexão GPRS.
 * Ela executa os seguintes comandos AT:
 * -> AT+CIPSEND=<mux>,<len> (envia dados através de conexão TCP ou UDP, timeout 1 segundo)
 */
int16_t VidoGsmSim800::modemSend(const void* buff, size_t len, uint8_t mux)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::modemSend");
    int16_t result = TinyGsmSim800::modemSend(buff, len, mux);
    VidoGsmLock.releaseMutex("VidoGsmSim800::modemSend");

    return result;
}

/**
 * @brief Realiza a leitura de um conjunto de dados da conexão aberta e coloca dentro da FIFO.
 * 
 * @param size Número de bytes a serem lidos.
 * @param mux Id do mux.
 * @return size_t Número de bytes lidos.
 * 
 * @remarks Esta função é usada pelo client para ler daods de um servidor acessível via internet. Por isso, depende da conexão GPRS.
 * Ela executa os seguintes comandos AT:
 * -> AT+CIPRXGET=2,<mux>,<size> (ler dados através de conexão TCP ou UDP, timeout 1 segundo)
 */
size_t VidoGsmSim800::modemRead(size_t size, uint8_t mux)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::modemRead");
    size_t result = TinyGsmSim800::modemRead(size, mux);
    VidoGsmLock.releaseMutex("VidoGsmSim800::modemRead");

    return result;
}

/**
 * @brief Retornar o número de bytes disponíveis para leitura.
 * 
 * @param mux Id do mux.
 * @return size_t Número de bytes a serem lidos.
 * 
 * @remarks Esta função é usada pelo client para verificar quantos bytes existem disponíveis no buffer de leitura.
 * Ela executa os seguintes comandos AT:
 * -> AT+CIPRXGET=4,<mux> (o 4 indica perguntar quantos dados não foram lidos no mux especificado, timeout 1 segundo)
 * Caso não retorne nada, verifica se continua conectado ao servidor (modemGetConnected).
 */
size_t VidoGsmSim800::modemGetAvailable(uint8_t mux)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::modemGetAvailable");
    size_t result = TinyGsmSim800::modemGetAvailable(mux);
    VidoGsmLock.releaseMutex("VidoGsmSim800::modemGetAvailable");

    return result;
}

/**
 * @brief Verificar se o client está conectado no servidor.
 * 
 * @param mux Id do mux.
 * @return true Caso esteja conectado.
 * @return false Caso contrário.
 * 
 * @remarks Executa os seguintes comandos AT:
 * -> AT+CIPSTATUS=<mux> (verificar o status da conexão atual, timeout 1 segundo)
 */
bool VidoGsmSim800::modemGetConnected(uint8_t mux)
{
    VidoGsmLock.waitOne(portMAX_DELAY, "VidoGsmSim800::modemGetConnected");
    bool result = TinyGsmSim800::modemGetConnected(mux);
    VidoGsmLock.releaseMutex("VidoGsmSim800::modemGetConnected");

    return result;
}