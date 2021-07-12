/**
 * @file StructLibrary.h
 * @author Vinicius de Sa (vinicius@vido-la.com)
 * @brief Biblioteca que contém todas as structs usadas pelo sistema.
 * @version 0.1
 * @date 23/07/2019
 * 
 * @copyright Copyright VIDO.LA 2019: Todos os direitos reservados.
 */
#pragma once

#include <Arduino.h>
#include "GeoCoordinate.h"
#include "Enums.h"

/**
 * @brief Estrutura que contem os dados do GPS.
 */
struct GPSContext_t
{
    /**
     * @brief Latitude, em graus decimais.
     * 
     */
    double Latitude;
    /**
     * @brief Longitude, em graus decimais.
     * 
     */
    double Longitude;
    /**
     * @brief Azimute, em graus.
     * 
     */
    double Bearing;
    /**
     * @brief Altitude, em metros.
     * 
     */
    double Altitude;
    /**
     * @brief Velocidade, em km/h.
     * 
     */
    double Speed;
    /**
     * @brief Horas.
     * 
     */
    uint8_t Time_Hours;
    /**
     * @brief Minutos.
     * 
     */
    uint8_t Time_Minutes;
    /**
     * @brief Segundos.
     * 
     */
    uint8_t Time_Seconds;
    /**
     * @brief Centisegundos.
     * 
     */
    uint8_t Time_Centiseconds;
    /**
     * @brief Ano.
     * 
     */
    uint16_t Date_Year;
    /**
     * @brief Mês.
     * 
     */
    uint8_t Date_Month;
    /**
     * @brief Dia.
     * 
     */
    uint8_t Date_Day;
    /**
     * @brief Número de satélites.
     * 
     */
    uint32_t Satellites;
    /**
     * @brief HDOP.
     * 
     */
    double HDOP;
    /**
     * @brief Flag que determina se o contexto é válido.
     * 
     */
    bool IsValid;
    /**
     * @brief Flag que determina se a posição está atualizada.
     * 
     */
    bool IsPositionUpdated;
};

/**
 * @brief Contexto do acelerômetro.
 * 
 */
struct AccelerometerContext_t 
{
    public:
        AccelerometerContext_t()
        {
            Raw_Acc_X = 0;
            Raw_Acc_Y = 0;
            Raw_Acc_Z = 0;
            Raw_T = 0;
            Raw_Gyro_X = 0;
            Raw_Gyro_Y = 0;
            Raw_Gyro_Z = 0;
            Gyro_DPS_X = 0.0;
            Gyro_DPS_Y = 0.0;
            Gyro_DPS_Z = 0.0;
            Acc_Angle_X = 0.0;
            Acc_Angle_Y = 0.0;
            Acc_X = 0.0;
            Acc_Y = 0.0;
            Acc_Y = 0.0;
        }
        /**
         * @brief Eixo X cru do acelerômetro.
         * 
         */
        int16_t Raw_Acc_X;
        /**
         * @brief Eixo Y cru do acelerômetro.
         * 
         */
        int16_t Raw_Acc_Y;
        /**
         * @brief Eixo Z cru do acelerômetro.
         * 
         */
        int16_t Raw_Acc_Z;
        /**
         * @brief Temperatura medida do MPU.
         * 
         */
        int16_t Raw_T;
        /**
         * @brief Eixo X cru do giroscópio.
         * 
         */
        int16_t Raw_Gyro_X;
        /**
         * @brief Eixo Y cru do giroscópio.
         * 
         */
        int16_t Raw_Gyro_Y;
        /**
         * @brief Eixo Z cru do giroscópio.
         * 
         */
        int16_t Raw_Gyro_Z;
        /**
         * @brief Eixo X calculado do giroscópio, em graus por segundo.
         * 
         */
        float Gyro_DPS_X;
        /**
         * @brief Eixo Y calculado do giroscópio, em graus por segundo.
         * 
         */
        float Gyro_DPS_Y;
        /**
         * @brief Eixo Z calculado do giroscópio, em graus por segundo.
         * 
         */
        float Gyro_DPS_Z;
        /**
         * @brief Eixo X calculado do acelerômetro, em g (m/s²).
         * 
         */
        float Acc_X;
        /**
         * @brief Eixo Y calculado do acelerômetro, em g (m/s²).
         * 
         */
        float Acc_Y;
        /**
         * @brief Eixo Z calculado do acelerômetro, em g (m/s²).
         * 
         */
        float Acc_Z;
        /**
         * @brief Ângulo X calculado do acelerômetro, em graus.
         * 
         */
        float Acc_Angle_X;
        /**
         * @brief Ângulo Y calculado do acelerômetro, em graus.
         * 
         */
        float Acc_Angle_Y;

        double Temperature;
};

/**
 * @brief Dados da curva.
 * 
 */
struct TurnData_t
{
    /**
     * @brief ID da curva.
     * 
     */
	String Id;
    /**
     * @brief Tipo da curva.
     * 
     */
	TurnType_e Type;
    /**
     * @brief Velocidade limite da curva, em km/h.
     * 
     */
	int SpeedLimit;
    /**
     * @brief Quantidade de pontos que a curva contém.
     * 
     */
	int PointCount;
    /**
     * @brief Flag que determina se a curva é válida.
     * 
     */
    bool IsValid;
    /**
     * @brief Coordenadas da curva.
     * 
     */
    GeoCoordinate Coordinates;
    /**
     * @brief Raio da curva.
     * 
     */
    double Radius;
};

/**
 * @brief Dados do ponto da curva.
 * 
 */
struct TurnPointData_t
{
    /**
     * @brief Coordenadas do ponto.
     * 
     */
	GeoCoordinate Coordinates;
    /**
     * @brief Número sequencial do ponto.
     * 
     */
	int Sequence;
    /**
     * @brief Distância do ponto para o raio crítico.
     * 
     */
	float DistanceToCriticalRay;
};

/**
 * @brief Informações do alerta.
 * 
 */
struct AlertInfo_t
{
    /**
     * @brief Distância de alerta.
     * 
     */
    float AlertDistance;
    /**
     * @brief Distância de alarme.
     * 
     */
    float AlarmDistance;
    /**
     * @brief Flag que determina se o alerta é válido ou não.
     * 
     */
    bool IsValid;
};

/**
 * @brief Informações de uma curva.
 * 
 */
struct TurnSummary_t
{
    /**
     * @brief Flag que determina se a curva é válida.
     * 
     */
	bool IsValid;
    /**
     * @brief Dados da curva.
     * 
     */
	TurnData_t TurnData;
    /**
     * @brief Dados dos pontos da curva.
     * 
     */
	TurnPointData_t TurnPointData;
};

/**
 * @brief Configurações do dispositivo.
 * 
 */
struct DeviceSettings_t
{
    /**
     * @brief Id do dispositivo.
     * 
     */
    int Id;

    /**
     * @brief Flag que define se o áudio está habilitado.
     * 
     */
    bool AudioEnabled;

    /**
     * @brief Flag que define se os leds estão habilitados.
     * 
     */
    bool LedsEnabled;
};

/**
 * @brief Configurações do Engine.
 * 
 */
struct EngineSettings_t
{
    /**
     * @brief Velocidade mínima a partir da qual o equipamento deve começar a registrar em log.
     * 
     */
    int MinSpeedToLog;
    /**
     * @brief Velocidade máxima para efeitos de alerta.
     * 
     */
    int SpeedLimit;
};

/**
 * @brief Configurações do sistema de GPS.
 * 
 */
struct GPSSettings_t
{
    /**
     * @brief Velocidade de comunicação, em bps.
     * 
     */
    int BaudRate;
    /**
     * @brief Taxa de amostragem. Pode ser 1, 2, 4, 5 ou 10 amostras por segundo.
     * 
     */
    int SampleRate;
    /**
     * @brief HDOP máximo para o equipamento identificar que tem sinal.
     * 
     */
    int MaxHDOP;
    /**
     * @brief Quantidade mínima de satélites para o equipamento identificar que tem sinal.
     * 
     */
    int MinSatellites;
    /**
     * @brief Timeout de resposta do GPS.
     * 
     */
    int Timeout;
    /**
     * @brief Número de mensagens que o GPS deve fornecer para que seja determinado que há sinal.
     * 
     */
    int MessagesReadToFix;
    /**
     * @brief 0 - uBlox Neo6-M / 1 - ATGM332D
     * 
     */
    int Model; 
};

/**
 * @brief Configurações de rede do dispositivo.
 * 
 */
struct Network_t
{
    char Name[32];
    char SSID[32];
    char Password[32];
    double Latitude;
    double Longitude;
    bool IsNew;
};

/**
 * @brief Configurações do sistema de log.
 * 
 */
struct LogSettings_t
{
    /**
     * @brief Tamanho máximo do arquivo, em bytes.
     * 
     */
    unsigned long MaxFileSize;
    /**
     * @brief Caminho da pasta do arquivo de log.
     * 
     */
    const char * Folder;
    /**
     * @brief Extensão dos arquivos de log.
     * 
     */
    const char * FileExtension;
};

/**
 * @brief Configurações da atualização de firmware remota (OTA).
 * 
 */
struct OTASettings_t
{
    /**
     * @brief URL para obtenção dos dados de atualização.
     * 
     */
    char URL[128];

    /**
     * @brief Pasta onde a atualização é armazenada.
     * 
     */
    char UpdateFolder[16];

    /**
     * @brief Intervalo de verificação de atualizações, em milissegundos.
     * 
     */
    int PollingInterval;

    char Host[128];

    char PartialURL[128];
};

/**
 * @brief Configurações do sistema de identificação de curvas.
 * 
 */
struct TurnSettings_t
{
    /**
     * @brief Pasta onde são armazenados os arquivos de quadrantes.
     * 
     */
    const char * Folder;
    /**
     * @brief Delimitador dos registros dos arquivos.
     * 
     */
    const char * Delimiter;
    /**
     * @brief Caractere que determina o início de um registro de curva.
     * 
     */
    char * TurnStartChar;
    /**
     * @brief Faixa de azimute usada para identificar se o veículo está próximo de uma curva.
     * 
     */
    int BearingRange;
    /**
     * @brief Distância mínima em metros entre o veículo e o ponto de curva mais próximo.
     * 
     */
    int DistanceToRecognizePoint;
    /**
     * @brief Fator de redução de velocidade em situação de alerta.
     * 
     */
    float NormalSlowdownFactor;
    /**
     * @brief Fator de redução de velocidade em situação de alarme.
     * 
     */
    float EmergencySlowdownFactor;
    /**
     * @brief Duração da reprodução de uma mensagem de alerta, em segundos.
     * 
     */
    int MessageDuration;
    /**
     * @brief Distância levada em consideração para carregar as curvas.
     * O sistema carrega somente as curvas dentro deste raio de distância.
     */
    int TurnDistance;
};

/**
 * @brief Informações da atualização de firmware.
 * 
 */
struct FirmwareUpdateInfo_t
{
    /**
     * @brief URL para download da atualização.
     * 
     */
    String URL;
    /**
     * @brief Versão do firmware.
     * 
     */
    int Version;

    uint32_t FileSize;

    bool InformationReceived;

    int HasUpdate;
};

/**
 * @brief Informações do firmware baixado.
 * 
 */
struct DownloadedFirmwareInfo_t
{
    /**
     * @brief Caminho em que o firmware foi gravado.
     * 
     */
    char Path[64];
    /**
     * @brief Versão do firmware baixado.
     * 
     */
    int Version;
};

/**
 * @brief Informações de versão.
 * 
 */
struct VersionInfo_t 
{
    /**
     * @brief Versão atual do sistema.
     * 
     */
    int CurrentVersion;
};

/**
 * @brief Configurações do rastreador.
 * 
 */
struct TrackerSettings_t
{
    /**
     * @brief Endereço IP ou domínio do servidor de rastreamento.
     * 
     */
    char Server[128];
    /**
     * @brief Porta do servidor de rastreamento.
     * 
     */
    int Port;
    /**
     * @brief Timeout de conexão ao servidor de rastreamento, em segundos.
     * 
     */
    time_t Timeout;
    /**
     * @brief Intervalo de envio de posições ao servidor quando estiver se movendo, em segundos.
     * 
     */
    int PositionReportMoving;
    /**
     * @brief Intervalo de envio de posições ao servidor quando estiver parado, em segundos.
     * 
     */
    int PositionReportStopped;

    /**
     * @brief Pasta dedicada à fila de transmissão.
     * 
     */
    char TransmissionFolder[128];
    
    /**
     * @brief Nome da pasta onde serão guardados os posicionamentos a serem transmitidos.
     * 
     */
    char PositionsFolder[128];

    /**
     * @brief Nome da pasta onde serão guardados os eventos a serem transmitidos.
     * 
     */
    char EventsFolder[128];
    /**
     * @brief Velocidade para calibração do velocímetro.
     * 
     */
    int SpeedTuning;
};

/**
 * @brief Configurações do GSM.
 * 
 */
struct GSMSettings_t
{
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
     * @brief Flag que determina se é para auto-reconectar.
     * 
     */
    bool AutoReconnect;
};

/**
 * @brief Detalhes do erro de conexão.
 * 
 */
struct LastError_t
{
    /**
     * @brief Mensagem de erro.
     * 
     */
    char Message[256];
    /**
     * @brief Código do erro.
     * 
     */
    int Code;
};

struct TrackerMessageHistoryInfo_t
{
    /**
     * @brief Timespan da última vez que esta mensagem foi enviada.
     * 
     */
    time_t LastSent;
    /**
     * @brief Número de vezes que esta mensagem foi enviada.
     * 
     */
    int SentCount;
    /**
     * @brief Número de vezes que esta mensagem foi enviada seguidamente.
     * 
     */
    int SentCountInARow;
};

/**
 * @brief Pacote padrão de comunicação do rastremento.
 * 
 */
struct TrackerPacket_t
{
    uint8_t Buffer[128];
    size_t Length;
    bool SendPending;
    MessageType_e Type;
};

/**
 * @brief Estado dos componentes do sistema.
 * 
 */
struct SystemStatus_t
{
    public:
        SystemStatus_t()
        {
            MPUWorking = true;
            SDWorking = true;
            GPSWorking = true;
            AudioWorking = true;
            GSMWorking = true;
            VersionManagerWorking = true;
            ConfigurationManagerWorking = true;
            LogManagerWorking = true;
        }

        bool MPUWorking;    // MPU funcionando
        bool SDWorking;     // SD funcionando
        bool SPIFFSWorking; // SPIFFS funcionando
        bool GPSWorking;    // GPS funcionando
        bool AudioWorking;  // Audio funcionando
        bool GSMWorking;    // GSM funcionando
        bool VersionManagerWorking;
        bool ConfigurationManagerWorking;
        bool LogManagerWorking;
};

/**
 * @brief Requisitos de sinal GSM.
 * 
 */
struct GSM_Requirements_t
{
    /**
     * @brief Define se GSM deverá ser usado.
     * 
     */
    bool Use;
    /**
     * @brief Nìvel mínimo de sinal do GSM, na escala retornada pela função ConnectionManager.getSignalQuality().
     * 
     */
    int MinimumSignalLevel;
};

/**
 * @brief Requisitos para a execução de um comando.
 * 
 */
struct CommandRequirements_t
{
    /**
     * @brief Define se o SD deverá ser usado.
     * 
     */
    bool SD;
    /**
     * @brief Define se o GPS deverá ser usado.
     * 
     */
    bool GPS;
    /**
     * @brief Define se o acelerômetro deverá ser usado.
     * 
     */
    bool Accelerometer;
    /**
     * @brief Define se o áudio deverá ser usado.
     * 
     */
    bool Audio;
    /**
     * @brief Requisitos do GSM.
     * 
     */
    GSM_Requirements_t GSM;
    /**
     * @brief Define se o gerenciador de versões deverá ser usado.
     * 
     */
    bool VersionManager;
    /**
     * @brief Define se o gerenciador de configurações deverá ser usado.
     * 
     */
    bool ConfigurationManager;
    /**
     * @brief Define se o gerenciador de logs deverá ser usado.
     * 
     */
    bool LogManager;
};

struct TurnContext_t
{
    public:
        TurnContext_t() 
        {
            IsInATurn = false;
            TurnData.IsValid = false;
            AlertData.Sequence = 0;
            AlarmData.Sequence = 0;
            SpeedAtAlertPoint = 0;
            SpeedAtAlarmPoint = 0;
            SpeedAtCriticalPoint = 0;
        }

        bool IsInATurn;
        TurnData_t TurnData;
        TurnPointData_t AlertData;
        TurnPointData_t AlarmData;
        int SpeedAtAlertPoint;
        int SpeedAtAlarmPoint;
        int SpeedAtCriticalPoint;
};


struct SystemInfo_t
{
    uint8_t Temperature;
    double GSMVoltage;
    time_t UpdatedOn;
    uint32_t FreeHeap;
    uint32_t FreePsram;
};
