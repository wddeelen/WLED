#pragma once
#include "wled.h"
#include "AniStairCaseLightV2_Defs.h"
//#include "Stair.h"

class AniStairCaseLightV2 : public Usermod {
    private:
        //Stair stair;

        struct AniStairSensor {
            bool lastSteadyState = LOW;         // the previous steady state from the input pin
            bool lastFlickerableState = LOW;    // the previous flickerable state from the input pin
            bool currentState = LOW;            // the current reading from the input pin
            bool triggered = false;             // true if the sensor is triggered 
            unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
        };
        
        // Private class members to store the usermod settings in
        bool enabled = true;
        int8_t topSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN;
        int8_t bottomSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN;
        int turnStepsOfAfterMs = USERMOD_ANIMATED_STAIRCASE_V2_TURN_STEPS_OF_AFTER_MS;
        int timeBetweenEachStepMs = USERMOD_ANIMATED_STAIRCASE_V2_TIME_BETWEEN_EACH_STEP_MS;       
        int transitionTimeMs = USERMOD_ANIMATED_STAIRCASE_V2_TRANSITION_TIME_MS;
        String ledsPerStepCfg = USERMOD_ANIMATED_STAIRCASE_V2_LEDS_PER_STEP;
        
        // Private class members that are used inside this usermod class        
        bool initDone = false;        
        unsigned long currentLoopTime;
        AniStairSensor AniStairSensorTop;
        AniStairSensor AniStairSensorBottom;
        //std::vector<int> ledsPerStepVector;

        //void stepsSetup();
        //void SyncSegmentsWithSteps();
        void debugPrint(String val);
        void debugPrintLn(String val);
        void debugPrint(int val);
        void debugPrintLn(int val);

        // OLD Animated_Staircase vars and functions
        bool checkSensors();
        void autoPowerOff();
        void updateSwipe();
        void updateSegments();
        unsigned long lastScanTime      = 0;        // Time of the last sensor check
        const unsigned int scanDelay    = 100;      // Time between checking of the sensors
        unsigned long lastSwitchTime    = 0;        // Last time the lights were switched on or off
        bool on                         = false;     // Lights on or off. (Flipping this will start a transition.)

        // These values are used by the API to read the last sensor state, or trigger a sensor through the API
        bool bottomSensorRead  = false;
        bool topSensorRead     = false;
        bool bottomSensorState = false;
        bool topSensorState    = false;
        bool topSensorWrite    = false;
        bool bottomSensorWrite = false;

        // Indicates which Sensor was seen last (to determine
        // the direction when swiping off)
        #define LOWER false
        #define UPPER true
        bool lastSensor = LOWER;

        // Swipe direction for current transition
        #define SWIPE_UP true
        #define SWIPE_DOWN false
        bool swipe = SWIPE_UP;

        // segment id between onIndex and offIndex are on.
        // controll the swipe by setting/moving these indices around.
        // onIndex must be less than or equal to offIndex
        byte onIndex = 0;
        byte offIndex = 0;

        // The maximum number of configured segments.
        // Dynamically updated based on user configuration.
        byte maxSegmentId = -1;
        byte minSegmentId = -1;
        
        // Strings to reduce flash memory usage (used more than twice)
        static const char _name[];
        static const char _enabled[];
        static const char _topSensor[];
        static const char _topSensorPin[];
        static const char _bottomSensor[];
        static const char _bottomSensorPin[];
        static const char _turnStepsOfAfterMs[];
        static const char _timeBetweenEachStepMs[];
        static const char _transitionTimeMs[];
        static const char _ledsPerStep[];
        static const int8_t _debounceTime;        
        
    public:
        inline void enable(bool enable) { enabled = enable; }
        inline bool isEnabled() { return enabled; }
        uint16_t getId() { return 00; } // return value wass USERMOD_ID_ANIMATED_STAIRCASE_V2
        void setup();
        void loop();
        void addToJsonInfo(JsonObject& root);
        void addToJsonState(JsonObject& root);
        void readFromJsonState(JsonObject& root);
        void addToConfig(JsonObject &root);
        bool readFromConfig(JsonObject &root);
        void appendConfigData();
        void CheckTriggers();
        void DoTheAnimation();
};

long tempTriggerStartTime = 0;
long tempResetTriggerAfter = 30000; // 30 seconds

enum TriggeredSensor {
    TriggeredSensor_None = 0,
    TriggeredSensor_Top = 1,
    TriggeredSensor_Bottom = 2
};
TriggeredSensor FirstSensorTriggered = TriggeredSensor_None;
TriggeredSensor SecondSensorTriggered = TriggeredSensor_None;

enum AniDirection {
    AniDirection_None = 0,
    AniDirection_Up = 1,
    AniDirection_Down = 2
};
AniDirection AnimationDirection = AniDirection_None;

bool lastCheckSensors = false;

// Strings to reduce flash memory usage (used more than twice)
const char AniStairCaseLightV2::_name[]                     PROGMEM = "AniStaircase";
const char AniStairCaseLightV2::_enabled[]                  PROGMEM = "enabled";
const char AniStairCaseLightV2::_topSensor[]                PROGMEM = "topSensor";
const char AniStairCaseLightV2::_topSensorPin[]             PROGMEM = "topSensorpin";
const char AniStairCaseLightV2::_bottomSensor[]             PROGMEM = "bottomSensor";
const char AniStairCaseLightV2::_bottomSensorPin[]          PROGMEM = "bottomSensorpin";
const char AniStairCaseLightV2::_turnStepsOfAfterMs[]       PROGMEM = "turnStepsOfAfterMs";
const char AniStairCaseLightV2::_timeBetweenEachStepMs[]    PROGMEM = "timeBetweenEachStepMs";
const char AniStairCaseLightV2::_transitionTimeMs[]         PROGMEM = "transitionTimeMs";
const char AniStairCaseLightV2::_ledsPerStep[]              PROGMEM = "ledsPerStep";
const int8_t AniStairCaseLightV2::_debounceTime             PROGMEM = 50;