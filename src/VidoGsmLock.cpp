#include "VidoGsmLock.h"

static SemaphoreHandle_t m_semaphoreVidoGsm;
static volatile int m_count = 0;

VidoGsmLockClass::VidoGsmLockClass()
{
    if (m_semaphoreVidoGsm == nullptr)
    {
        ESP_LOGD("", "[%ld] Criando semaphoro %02d", millis(), ++m_count);
        m_semaphoreVidoGsm = xSemaphoreCreateMutex();            
    }
}

void VidoGsmLockClass::waitOne(TickType_t timeout, const char * source)
{
    xSemaphoreTake(m_semaphoreVidoGsm, timeout);
}

void VidoGsmLockClass::releaseMutex(const char * source)
{
    xSemaphoreGive(m_semaphoreVidoGsm);
}

VidoGsmLockClass VidoGsmLock;