/************************************************************************
* Arquivo: Enums.h
* Autor: Vinicius de Sa
* Data: 14/02/2019
* Objetivo: 
************************************************************************/
#pragma once

/**
 * Tipos dos pontos de interesse.
 */
enum PointType_e
{
    POINT_TYPE_TURN_ALERT = 0,
    POINT_TYPE_TURN_ALARM = 1,
    POINT_TYPE_SPEED_CONTROL = 2,
    POINT_TYPE_HIGHWAY_POLICE = 3,
    POINT_TYPE_CITY_SPEED_BUMP = 4,
    POINT_TYPE_HIGHWAY_SPEED_BUMP = 5,
    POINT_TYPE_DANGER = 6,
    POINT_TYPE_LEVEL_CROSSING = 7,
    POINT_TYPE_RESTRICTED_HEIGHT = 8,
    POINT_TYPE_NONE = -1
};

/**
 * Sentido dos pontos de interesse.
 */
enum PointOfInterestWay
{
    POI_WAY_ALL = 0,
    POI_WAY_AZIMUTH = 1,
    POI_WAY_AZIMUTH_AND_REVERSE = 2,
    POI_WAY_NONE = -1
};

/**
 * Tipo de curva do ponto de interesse.
 */
enum PointOfInterestTurnType
{
    POI_TURN_LEFT = 0,
    POI_TURN_RIGHT = 1,
    POI_TURN_NONE = -1
};

/**
 * Tipo de curva.
 */
enum TurnType_e
{
    TURN_LEFT = 1,      // Curva para a esquerda
    TURN_RIGHT = 2,     // Curva para a direita
    TURN_SEQUENCE = 3   // Sequência de curvas
};

enum EventType_e
{
    // GPS iniciado
    EVENT_GPS_START = 0x00,
    // Falha de GPS
    EVENT_GPS_FAILURE = 0x01,
    // Sinal de GPS perdido
    EVENT_GPS_NO_FIX = 0x02,
    // Sinal de GPS recuperado
    EVENT_GPS_FIX = 0x03,
    // MPU inicializada
    EVENT_MPU_START = 0x04,
    // Erro na MPU
    EVENT_MPU_ERROR = 0x05,
    // Curva identificada, mas abaixo da velocidade para alerta
    EVENT_POI_LOWSPEED = 0x06,
    // Curva identificada, mas acima da distancia para alerta ou alarme
    EVENT_POI_HIGHDISTANCE = 0x07,
    // SD card conectado
    EVENT_SD_CONNECTED = 0x08,
    // Leds inicializados
    EVENT_LED_STARTED = 0x09,
    // Áudio iniciado
    EVENT_WAV_STARTED = 0x0A,
    // Quadrante carregado
    EVENT_QUADRANT_LOADED = 0x0B,
    // Curva carregada
    EVENT_TURN_LOADED = 0x0C,
    // Botão pressionado
    EVENT_BUTTON_PRESSED = 0x0D,
    // Ativação do modo noturno
    EVENT_LED_NIGHT = 0x0E,
    // Ativação de leds no modo diurno
    EVENT_LED_DAY = 0x0F,
    // Sistema reiniciado via watchdog
    EVENT_WATCHDOG_RESTART = 0x10,
    // Falha ao configurar o horario
    EVENT_SET_TIME_FAILURE = 0x11
};

/**
 * @brief Tipos de mensagem.
 * 
 */
enum MessageType_e
{
    /**
     * @brief Report de posicionamento
     * 
     */
	MSG_POSITION_REPORT = 0x01,
    /**
     * @brief Report de evento.
     * 
     */
	MSG_EVENT_REPORT = 0x02
};