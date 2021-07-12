/**
 * @file WatchdogManager.cpp
 * @author Vinicius de Sá (vinicius@vido-la.com)
 * @brief Gerenciador de comunicação com o watchdog
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "WatchdogManager.h"

/**
 * @brief Inicializar o objeto WatchdogManagerClass.
 * 
 */
WatchdogManagerClass::WatchdogManagerClass()
{}

/**
 * @brief Inicializar o watchdog.
 * 
 * @param watchdogPin Pino a ser inicializado.
 */
void WatchdogManagerClass::begin(gpio_num_t watchdogPin)
{
    m_pin = watchdogPin;
    m_pinState = LOW;
    m_lastFeedTime = 0;
    m_lastRestart = 0;

    pinMode(watchdogPin, OUTPUT);
}

/**
 * @brief Alimentar o watchdog.
 * 
 */
void WatchdogManagerClass::feed()
{
    if ((millis() - m_lastFeedTime) > FEED_INTERVAL)
    {
        if (CacheStatic.SystemStatus.SDWorking && CacheStatic.SystemStatus.GSMWorking)
        {
            m_pinState = m_pinState == LOW ? HIGH : LOW;
            digitalWrite(m_pin, m_pinState);
            m_lastFeedTime = millis();
        }
        else
        {
            executeRestart(millis());
        }
    }
}

/**
 * @brief Manipular o restart do sistema caso haja necessidade. 
 * 
 */
void WatchdogManagerClass::restartIfNecessary()
{
    time_t currentTime = Utils.getUnixTime();

    if (isRestartNecessary(currentTime))
    {
        executeRestart(currentTime);
    }
}

/**
 * @brief Verificar se a reinicialização do sistema via watchdog é necessária.
 * 
 * @param timeValue Horário.
 * 
 * @return true Se o restart for necessário.
 * @return false Caso contrário.
 */
bool WatchdogManagerClass::isRestartNecessary(time_t timeValue)
{
    if (m_lastRestart == 0)
    {
        Preferences pref = Preferences();
        pref.begin(PREF_NAMESPACE, false);
        m_lastRestart = pref.getULong(PREF_KEY_LAST_RESTART, timeValue);
        pref.end();
    }

    return (timeValue - m_lastRestart) > ONE_DAY_MILLIS;
}

/**
 * @brief Executar a reinicialização do sistema via watchdog.
 * 
 */
void WatchdogManagerClass::executeRestart(time_t currentTime)
{
    // "[%lu] Horario: %lu | Ultimo Restart: %lu"
    ESP_LOGD(TAG_WATCHDOG, WATCH_LOGD_0003, millis(), currentTime, m_lastRestart);
    LogManager.logEvent(EVENT_WATCHDOG_RESTART, String(currentTime));
    
    Preferences pref = Preferences();
    pref.begin(PREF_NAMESPACE, false);
    pref.putULong(PREF_KEY_LAST_RESTART, currentTime);
    pref.end();

    time_t startTime = millis();

    while((millis() - startTime) < RESTART_TIME)
    {
        // "[%lu] Aguardando para reiniciar."
        ESP_LOGD(TAG_WATCHDOG, WATCH_LOGD_0001, millis());
        delay(5000);
    }

    // "[%lu] Sistema nao reiniciado."
    ESP_LOGD(TAG_WATCHDOG, WATCH_LOGD_0002, millis());
}

WatchdogManagerClass WatchdogManager;