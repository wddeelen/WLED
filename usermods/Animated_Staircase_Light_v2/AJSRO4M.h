#pragma once
#include "wled.h"

class AJSRO4M {
    private:
        uint8_t pinNrTrigger = -1;
        uint8_t pinNrEcho = -1;
        long lastPing = -1;
        long pingDuration = -1;
        int distance = -1;
        void Ping();

    public:
        AJSRO4M(int8_t _pinNrTrigger, int8_t _pinNrEcho);
        int GetDistance();
};