#pragma once
#include "wled.h"
#include "AniStairs_Devs.h"
#include "AniStairs_Stair_Sensor.h"

class Stair {
    private:
        struct step { 
            int number;
            int leds;
            //int segmentId;
            int onAfter = -1;
            int offAfter = -1;
        };
        String ledsPerStep = ANISTAIRS_LEDS_PER_STEP;
        std::vector<step> steps;
        int totalLedsInStair;
        int maxStepId = -1;
        int timeBetweenEachStepMs = ANISTAIRS_TIME_BETWEEN_EACH_STEP_MS;
        int timeThatEachStepStaysOnMs = ANISTAIRS_TIME_THAT_EACH_STEP_STAYS_ON_MS;
        int transitionTimeMs = ANISTAIRS_TRANSITION_TIME_MS;
        
        void ClearSteps();
        String SetupSteps(String _ledsPerStep);
        void SyncStairStepsWithStripSegments();
        void CheckSensors();
        void UpdateAnimation();
        
    public:
        Sensor SensorTop    = Sensor(ANISTAIRS_SENSOR_TOP_TRIGGER_PIN   , ANISTAIRS_SENSOR_TOP_ECHO_PIN     , ANISTAIRS_SENSOR_TOP_TRIGGER_MIN_DISTANCE     , ANISTAIRS_SENSOR_TOP_TRIGGER_MAX_DISTANCE);
        Sensor SensorBottom = Sensor(ANISTAIRS_SENSOR_BOTTOM_TRIGGER_PIN, ANISTAIRS_SENSOR_BOTTOM_ECHO_PIN  , ANISTAIRS_SENSOR_BOTTOM_TRIGGER_MIN_DISTANCE  , ANISTAIRS_SENSOR_BOTTOM_TRIGGER_MAX_DISTANCE);
        byte GetStepsSize();
        String GetLedsPerStep();
        void SetLedsPerStep(String val);
        void Setup();
        void Loop();
        int GetTimeBetweenEachStepMs();
        void SetTimeBetweenEachStepMs(int miliseconds);
        int GetTimeThatEachStepStaysOnMs();
        void SetTimeThatEachStepStaysOnMs(int miliseconds);
        float GetTimeThatEachStepStaysOnMin();
        void SetTimeThatEachStepStaysOnMin(float minutes);
        int GetTransitionTimeMs();
        void SetTransitionTimeMs(int miliseconds);



         // Temp develop debug voids
        void debugPrint(String val);
        void debugPrint(int val);
        void debugPrintLn(String val);
        void debugPrintLn(int val);
};