#pragma once
#include "wled.h"
#include "AniStairs_Stair.h"
#include "AniStairs_Devs.h"

class AniStairs : public Usermod {
    private:
        // Strings to reduce flash memory usage (used more than twice)
        static const char _name[];
        static const char _enabled[];
        static const char _sensorTopTriggerPin[];
        static const char _sensorTopEchoPin[];
        static const char _sensorTopMinTriggerDistanceCm[];
        static const char _sensorTopMaxTriggerDistanceCm[];
        static const char _sensorBottomTriggerPin[];
        static const char _sensorBottomEchoPin[];
        static const char _sensorBottomMinTriggerDistanceCm[];
        static const char _sensorBottomMaxTriggerDistanceCm[];
        static const char _timeBetweenEachStepMs[];
        static const char _timeThatEachStepStaysOnMin[];
        static const char _transitionTimeMs[];
        static const char _ledsPerStep[];
        
        // Private class members to store the usermod settings
        Stair stair;
        bool enabled = true;
        bool initDone = false;
        uint8_t ReAllocatePin(uint8_t oldPin, uint8_t newPin);
        
    public:
        // Public methods
        inline void enable(bool enable) { 
            enabled = enable; 
            if (enable) stair.Setup();
        };
        inline bool isEnabled() { return enabled; };
        uint16_t getId() { return USERMOD_ID_ANISTAIRS; };
        void setup();
        void loop();
        void addToJsonInfo(JsonObject& root);
        void addToJsonState(JsonObject& root);
        void readFromJsonState(JsonObject& root);
        void addToConfig(JsonObject &root);
        bool readFromConfig(JsonObject &root);
        void appendConfigData();

        // Temp develop debug voids
        void debugPrint(String val);
        void debugPrint(int val);
        void debugPrintLn(String val);
        void debugPrintLn(int val);
};