#pragma once

#define TINY_GSM_YIELD_MS 1
#include <TinyGsmClientSIM800.h>
#include "VidoGsmLock.h"

class VidoGsmSim800 : public TinyGsmSim800
{
public:
    explicit VidoGsmSim800(Stream &stream) : TinyGsmSim800(stream)
    {
        ESP_LOGD("", "Criando VidoGsmSim800");
    }

    /**
     * @brief Ajustar o modem para os padrões de fábrica.
     * 
     * @return true Caso os passos para a execução do comando tenham sido bem sucedidos.
     * @return false Caso contrário.
     * @remarks Executa os seguintes comandos AT:
     * -> AT&FZE0&W (Factory default + reset + desligar echo + escrever na memória, timeout 1 segundo)
     * -> AT+IPR=0 (setar o baud rate local para 0 - auto bauding, timeout 1 segundo)
     * -> AT+IFC=0,0 (setar o controle do fluxo de dados para sem controle de fluxo, timeout 1 segundo)
     * -> AT+ICF=3,3 (setar o controle de framing de caracteres 8 bits de dados, 0 de paridade 1 de stop, paridade even, timeout 1 segundo)
     * -> AT+CSCLK=0 (desabilitar o relógio slow, timeout 1 segundo)
     * -> AT&W (escrever as configurações, timeout 1 segundo)
     */
    bool factoryDefault();

    /**
     * @brief Executar o procedimento de restart do módulo GSM.
     * 
     * @return true Caso tenha sido bem sucedido.
     * @return false Caso contrário
     * @remarks Este comando testa a comunicação entre o módulo GSM e o ESP através de comandos AT.
     *          Caso tenha comunicação, desativa as funções do módulo e depois as reativa, e aí executa o init;
     *          Comandos AT: AT, AT&W, AT+CFUN=0, AT+CFUN=1,1
     */
    bool restart();

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
    bool init();

    /**
     * @brief Obter informações do modem.
     * 
     * @return String com o resultado da operação.
     * @remarks Normalmente este comando retorna o modelo e a versão do módulo. Ex: SIM800 R14.18.
     *          Comandos AT: ATI
     */
    String getModemInfo();

    /**
     * @brief Retornar o status de registro na rede.
     * 
     * @return RegStatus Status de registro na rede.
     * @remarks Este comando verifica se o GSM está registrado na rede, ou seja, se tem sinal suficiente e uma antena (ERB) próxima.
     *          Caso o modem esteja com restrição na Anatel, o GSM não se registrará.
     *          Comandos AT: AT+CREG?
     */
    RegStatus getRegistrationStatus();

    /**
     * @brief Converter o status de registro para string.
     * 
     * @param status Status de registro.
     * @return String Resultado da conversão.
     */
    String toString(RegStatus status);

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
     * @brief Aguardar o módulo se registrar em uma rede GSM por X tempo.
     * 
     * @param timeout_ms Tempo a aguardar para se registrar em uma rede GSM.
     * @return true Caso tenha conseguido se registrar no tempo determinado.
     * @return false Caso contrário.
     * @remarks Executa o isNetworkConnected a cada 250ms até dar o timeout.
     */
    bool waitForNetwork(uint32_t timeout_ms = 60000L);

    /**
     * @brief Desligar o módulo GSM.
     * 
     * @return true Caso o módulo tenha sido desligado.
     * @return false Caso contrário.
     * @remarks Executa o comando AT+CPOWD=1 (power down, timeout 10 segundos)
     */
    bool powerOff();

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
    bool sleepEnable(bool enable = true);

    /**
     * @brief Retornar a operadora atual.
     * 
     * @return String Operadora atual.
     * @remarks Executa os seguintes comandos AT:
     * -> AT+COPS? (obter o operador de rede atual, timeout 1 segundo)
     */
    String getOperator();

    /**
     * @brief Retornar a banda atual.
     * 
     * @return String banda atual.
     * @remarks Executa os seguintes comandos AT:
     * -> AT+CBAND? (obter o operador de rede atual, timeout 1 segundo)
     */
    String getBand();

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
    bool gprsConnect(const char *apn, const char *user = NULL, const char *pwd = NULL);

    /**
     * @brief Desconectar o GPRS.
     * 
     * @return true Caso o GPRS tenha sido desconectado.
     * @return false Caso contrário.
     * @remarks Executa os seguintes comandos AT:
     * -> AT+CIPSHUT (shutdown da conexão TCP/IP, timeout 60 segundos)
     * -> AT+CGATT=0 (desatacha da conexão GPRS, timeout 60 segundos)
     */
    bool gprsDisconnect();

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

    /**
     * @brief Obter o IP local do dispositivo.
     * 
     * @return String com o IP local.
     * @remarks Se não estiver com o GPRS conectado, retornará um IP inválido.
     * Executa os seguintes comandos:
     * -> AT+CIFSR;E0 (solicitar o IP local, verificar se o IP é válido, timeout 10 segundos)
     */
    String getLocalIP();

    /**
     * @brief Obter o CCID do SIM Card.
     * 
     * @return String com o CCID do SIM Card.
     * @remarks Se não estiver com um SIM Card, retornará vazio.
     * Executa os seguintes comandos:
     * -> AT+CCID (solicitar o CCID do SIM, timeout 1 segundo)
     */
    String getSimCCID();

    /**
     * @brief Obter o IMEI do dispositivo.
     * 
     * @return String com o IMEI do dispositivo.
     * @remarks
     */
    String getIMEI();

    /**
     * @brief Obter qualidade do sinal.
     * 
     * @return int16_t com qualidade do sinal.
     */
    int16_t getSignalQuality();

    /**
     * @brief Obter o nivel de bateria.
     * 
     * @return int8_t com nivel de bateria, em %.
     */
    int8_t getBattPercent();

    /**
     * @brief Obter a tensao da bateria.
     * 
     * @return uint16_t com a tensao da bateria.
     */
    uint16_t getBattVoltage();

    /**
     * @brief Obter a data e hora da rede GSM.
     * 
     * @return String com data e hora.
     */
    String getGSMDateTime(TinyGSMDateTimeFormat format);

    /**
     * @brief Obter a localizacao por GSM.
     * 
     * @return String com a localizacao.
     */
    String getGsmLocation();

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
    byte NTPServerSync(String server = "pool.ntp.org", byte TimeZone = 3);

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
    bool modemConnect(const char *host, uint16_t port, uint8_t mux, bool ssl = false, int timeout_s = 75);

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
    int16_t modemSend(const void *buff, size_t len, uint8_t mux);

    /**
     * @brief Realiza a leitura de um conjunto de dados da conexão aberta e coloca dentro da FIFO.
     * 
     * @param size Número de bytes a serem lidos.
     * @param mux Id do mux.
     * @return size_t Número de bytes lidos.
     * 
     * @remarks Esta função é usada pelo client para ler daods de um servidor acessível via internet. Por isso, depende da conexão GPRS.
     * Ela executa os seguintes comandos AT:
     * -> AT+CIPRXGET=2,<mux>,<size> (ler dados através de conexão TCP ou UDP, no máximo 1460 bytes por vez, timeout 1 segundo)
     */
    size_t modemRead(size_t size, uint8_t mux);

    /**
     * @brief Retornar o número de bytes disponíveis para leitura.
     * 
     * @param mux Id do mux.
     * @return size_t Número de bytes disponíveis para leitura.
     * 
     * @remarks Esta função é usada pelo client para verificar quantos bytes existem disponíveis no buffer de leitura.
     * Ela executa os seguintes comandos AT:
     * -> AT+CIPRXGET=4,<mux> (o 4 indica perguntar quantos dados não foram lidos no mux especificado, timeout 1 segundo)
     * Caso não retorne nada, verifica se continua conectado ao servidor (modemGetConnected).
     */
    size_t modemGetAvailable(uint8_t mux);

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
    bool modemGetConnected(uint8_t mux);
};