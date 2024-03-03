#pragma once
#include "wled.h"
#include "AJSRO4M.h"

class Stair {  
    private:
        // Private structure declaration
        struct step { 
            int number;
            int leds;
            Segment* segment = NULL;
        };

        // Private propertie declaration
        static std::vector<step> steps;
        static int totalLedsInStair;

        // Private method declaration
        void ClearSteps();
        
    public:
        // Constructors
        Stair();
        Stair(
            int8_t _sensorTopTriggerPin, int8_t _sensorTopEchoPin,          // Top sensor pins
            int8_t _sensorBottomTriggerPin, int8_t _sensorBottomEchoPin,    // Bottom sensor pins
            String _ledsPerStep                                             // The leds per steps string from the config
        );

        // Public objects
        static AJSRO4M SensorTop;
        static AJSRO4M SensorBottom;

        // Public methods
        byte GetStepsSize();
        String SetupSteps(String _ledsPerStep);
        
        void SyncStairStepsWithStripSegments();
};
