/**
 * @file WatchdogManager.h
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief Gerenciador de comunicação com o watchdog
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "CacheStatic.h"
#include "LogManager.h"
#include "Utils.h"

#define FEED_INTERVAL 1000
#define ONE_DAY_MILLIS 86400000
#define RESTART_TIME 240000

#define PREF_NAMESPACE "watchdog"
#define PREF_KEY_LAST_RESTART "lastrestart"

#define TAG_WATCHDOG "[WATCHDOG]"
#define WATCH_LOGD_0001 "[%lu] Aguardando para reiniciar."
#define WATCH_LOGD_0002 "[%lu] Sistema nao reiniciado."
#define WATCH_LOGD_0003 "[%lu] Horario: %lu | Ultimo Restart: %lu"

/**
 * @brief Gerenciador do watchdog.
 * 
 */
class WatchdogManagerClass
{
    public:
        /**
         * @brief Inicializar o objeto WatchdogManagerClass.
         * 
         */
        WatchdogManagerClass();

        /**
         * @brief Inicializar o watchdog.
         * 
         * @param watchdogPin Pino a ser inicializado.
         */
        void begin(gpio_num_t watchdogPin);

        /**
         * @brief Alimentar o watchdog.
         * 
         */
        void feed();
        /**
         * @brief Reiniciar o sistema caso haja necessidade. 
         * 
         */
        void restartIfNecessary();

        /**
         * @brief Executar a reinicialização do sistema via watchdog.
         * 
         * @param currentTime Horário atual.
         * 
         */
        void executeRestart(time_t currentTime);
        /**
         * @brief Verificar se a reinicialização do sistema via watchdog é necessária.
         * 
         * @param timeValue Horário.
         * 
         * @return true 
         * @return false 
         */
        bool isRestartNecessary(time_t timeValue);
    private:
        uint8_t m_pinState;
        time_t m_lastFeedTime;
        gpio_num_t m_pin;
        time_t m_lastRestart;
};

extern WatchdogManagerClass WatchdogManager;