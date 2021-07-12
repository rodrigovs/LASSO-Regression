#pragma once

#include <Arduino.h>

class VidoGsmLockClass
{
    public:
        VidoGsmLockClass();

        void waitOne(TickType_t timeout, const char * source = "");

        void releaseMutex(const char * source = "");
        
};

extern VidoGsmLockClass VidoGsmLock;