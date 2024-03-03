#pragma once
#include "wled.h"

class Sensor {
    private:
        uint8_t pinNrTrigger = -1;
        uint8_t pinNrEcho = -1;
        long lastPing = -1;
        long pingDuration = -1;
        int distance = -1;
        int minTriggerDistance = 50;
        int maxTriggerDistance = 70;
        bool objectInTriggerRange = false;
        void Ping();

    public:
        Sensor(int8_t TriggerPinNumber, int8_t EchoPinNumber, int TriggerMinDistance, int TriggerMaxDistance);
        int GetDistance();
        uint8_t GetTriggerPin();
        void SetTriggerPin(uint8_t pinNumber);
        uint8_t GetEchoPin();
        void SetEchoPin(uint8_t pinNumber);
        int GetMinTriggerDistance();
        void SetMinTriggerDistance(int CmDistance);
        int GetMaxTriggerDistance();
        void SetMaxTriggerDistance(int CmDistance);
        bool IsObjectInTriggerRange();
};