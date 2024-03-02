#pragma once
#include "wled.h"

class AJSRO4M {
    private:
        uint8_t pinNrTrigger = -1;
        uint8_t pinNrEcho = -1;
        long lastPing = -1;
        long pingDuration = -1;
        int distance = -1;
        
        void Ping() {
            // Direct stop if one of the pins are lower then 0
            if (pinNrTrigger < 0 || pinNrEcho < 0) return;

            // Update a ping only after 250 miliseconds
            if ((millis() - lastPing) > 250) {
                // Make the trigger pin low for 5 Microseconds.
                digitalWrite(pinNrTrigger, LOW);
                delayMicroseconds(5);

                // Trigger the sensor by setting the trigger pin high for 10 microseconds:
                digitalWrite(pinNrTrigger, HIGH);
                delayMicroseconds(10);
                digitalWrite(pinNrTrigger, LOW);

                // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
                pingDuration = pulseIn(pinNrEcho, HIGH);

                // Calculate the distance:
                distance = rint((pingDuration * 0.034) / 2);

                // Update the last ping time
                lastPing = millis();
            }        
        }; 

    public:
        AJSRO4M(int8_t _pinNrTrigger, int8_t _pinNrEcho) {
            pinNrTrigger = _pinNrTrigger;
            pinNrEcho = _pinNrEcho;
            if (pinNrTrigger > -1 && pinNrEcho > -1) {
                pinMode(pinNrTrigger, OUTPUT);
                pinMode(pinNrEcho, INPUT);
            }
        };

        int GetDistance() {
            Ping();
            return distance;
        };
};

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN
#define USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN 35
#endif

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN
#define USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN 36
#endif

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_TURN_STEPS_OF_AFTER_MS
#define USERMOD_ANIMATED_STAIRCASE_V2_TURN_STEPS_OF_AFTER_MS 5000
#endif

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_TIME_BETWEEN_EACH_STEP_MS
#define USERMOD_ANIMATED_STAIRCASE_V2_TIME_BETWEEN_EACH_STEP_MS 250
#endif

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_TRANSITION_TIME_MS
#define USERMOD_ANIMATED_STAIRCASE_V2_TRANSITION_TIME_MS 250
#endif

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_LEDS_PER_STEP
#define USERMOD_ANIMATED_STAIRCASE_V2_LEDS_PER_STEP "10;10;10"
#endif

struct AniStairSensor {
    bool lastSteadyState = LOW;         // the previous steady state from the input pin
    bool lastFlickerableState = LOW;    // the previous flickerable state from the input pin
    bool currentState = LOW;            // the current reading from the input pin
    bool triggered = false;             // true if the sensor is triggered 
    unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
};

class AniStairCaseLightV2 : public Usermod {
    private:
        // Private class members to store the usermod settings in
        bool enabled = true;
        int8_t topSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN;
        int8_t bottomSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN;
        int turnStepsOfAfterMs = USERMOD_ANIMATED_STAIRCASE_V2_TURN_STEPS_OF_AFTER_MS;
        int timeBetweenEachStepMs = USERMOD_ANIMATED_STAIRCASE_V2_TIME_BETWEEN_EACH_STEP_MS;       
        int transitionTimeMs = USERMOD_ANIMATED_STAIRCASE_V2_TRANSITION_TIME_MS;
        String ledsPerStep = USERMOD_ANIMATED_STAIRCASE_V2_LEDS_PER_STEP;

        AJSRO4M Ajsro4MTop = AJSRO4M(9, 10);
        AJSRO4M Ajsro4MBottom = AJSRO4M(8, 3);       
        
        // Private class members that are used inside this usermod class        
        bool initDone = false;        
        unsigned long currentLoopTime;
        AniStairSensor AniStairSensorTop;
        AniStairSensor AniStairSensorBottom;
        std::vector<int> ledsPerStepVector;

        void stepsSetup();
        void segmentSetup();
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
        uint16_t getId() { return USERMOD_ID_ANIMATED_STAIRCASE_V2; }
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

#define debugPrintDelay 0
void AniStairCaseLightV2::debugPrint(String val) { Serial.print(val); if (debugPrintDelay > 0) delay(debugPrintDelay); }
void AniStairCaseLightV2::debugPrintLn(String val) { Serial.println(val); if (debugPrintDelay > 0) delay(debugPrintDelay); }
void AniStairCaseLightV2::debugPrint(int val) { debugPrint(String(val)); }
void AniStairCaseLightV2::debugPrintLn(int val) { debugPrintLn(String(val));}

void AniStairCaseLightV2::setup() {
    // Validate pin numbers and set to -1 if they are not valid.
    if (topSensorPin < 0) topSensorPin = -1;
    if (bottomSensorPin < 0) bottomSensorPin = -1;

    // Check if the usermod is enabled
    if (enabled) {
        // Config says we are enabled

        // Allocate Top Sensor pin
        if (topSensorPin >= 0 && pinManager.allocatePin(topSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
            // Set the pin mode for the Top sensor pin.
            pinMode(topSensorPin, INPUT_PULLUP);
            
            // Allocate Bottom Sensor pin
            if (bottomSensorPin >= 0 && pinManager.allocatePin(bottomSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
                // Set the pin mode for the Bottom sensor pin.
                pinMode(bottomSensorPin, INPUT_PULLUP);
                stepsSetup();

            } else {
                bottomSensorPin = -1;
            }
        } else {
            
            topSensorPin = -1;
        }
    }
    enable(enabled);
    initDone = true;
}

/*
START EIGEN CODE OM TE ALLES OP MIJN MANIER WERKEND TE KRIJGEN
*/
#define TopMinDistance 20
#define TopMaxDistance 80
#define BottomMinDistance 20
#define BottomMaxDistance 80

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

void AniStairCaseLightV2::CheckTriggers() {
    // Get the last states
    TriggeredSensor LastFirstSensorTriggered = FirstSensorTriggered;
    TriggeredSensor LastSecondSensorTriggered = SecondSensorTriggered;
    
    // Get the distance from the top sensor
    int Distance = Ajsro4MTop.GetDistance();
    if (Distance >= TopMinDistance && Distance <= TopMaxDistance) {
        //debugPrintLn("Top binnen afstand");
        // Set FirstSensorTriggered to Top if there is nobody on the stairs
        // and set AnimationDirection to AniDirection_Down
        if (FirstSensorTriggered == TriggeredSensor_None && SecondSensorTriggered == TriggeredSensor_None) {
            FirstSensorTriggered = TriggeredSensor_Top;
            AnimationDirection = AniDirection_Down;
        }
    }

    // Get the distance from the bottom sensor
    Distance = Ajsro4MBottom.GetDistance();
    if (Distance >= BottomMinDistance && Distance <= BottomMaxDistance) {
        //debugPrintLn("Bottom binnen afstand");
        // Set FirstSensorTriggered to Bottom if there is nobody on the stairs
        // and set AnimationDirection to AniDirection_Up
        if (FirstSensorTriggered == TriggeredSensor_None && SecondSensorTriggered == TriggeredSensor_None) {
            FirstSensorTriggered = TriggeredSensor_Bottom;
            AnimationDirection = AniDirection_Up;
        }

    }

    // Check if there is a sensor triggered
    if (LastFirstSensorTriggered != FirstSensorTriggered || LastSecondSensorTriggered != SecondSensorTriggered) {
        // Set tempTriggerStartTime to the current mills for the reset
        tempTriggerStartTime = millis();

        // Output the readings
        String str = "";
        str += "FirstSensorTriggered : ";
        str += FirstSensorTriggered;
        str += " ";
        
        str += "SecondSensorTriggered : ";
        str += SecondSensorTriggered;
        str += " (0 = None, 1 = Top, 2 = Bottom) ";
        
        str += "AnimationDirection : ";
        str += AnimationDirection;
        str += " (0 = None, 1 = Up, 2 = Down)";
        debugPrintLn(str);


    }


    
    // TEMP check to reset verything
    if ((millis() - tempTriggerStartTime) >= tempResetTriggerAfter) {
        FirstSensorTriggered = TriggeredSensor_None;
        SecondSensorTriggered = TriggeredSensor_None;
        AnimationDirection = AniDirection_None;
        tempTriggerStartTime = 0;
    }
}

void AniStairCaseLightV2::DoTheAnimation() {

}

/*
EIND EIGEN CODE OM TE ALLES OP MIJN MANIER WERKEND TE KRIJGEN
*/

bool lastCheckSensors = false;
void AniStairCaseLightV2::loop() {
    // Stop directly if the usermod is disabled or the strip is updating
    if (!enabled || !initDone || strip.isUpdating()) return;

    // Check Distance sensors
    CheckTriggers();
    DoTheAnimation();

    
    
    
    
    //String str = "Top = ";
    //str += Ajsro4MTop.GetDistance();
    //str += " Bottom = ";
    //str += Ajsro4MBottom.GetDistance();
    //debugPrintLn(str);
    
    
    minSegmentId = 0; // strip.getMainSegmentId();  // it may not be the best idea to start with main segment as it may not be the first one
    maxSegmentId = ledsPerStepVector.size(); // strip.getLastActiveSegmentId() + 1;

    bool currentCheckSensors = checkSensors();
    if (lastCheckSensors == false && currentCheckSensors == true) {
        if (strip._segments.size() != ledsPerStepVector.size()) {
            segmentSetup();
        }        
    }
    autoPowerOff(); //if (on) autoPowerOff();    
    updateSwipe();
}

bool AniStairCaseLightV2::checkSensors() {
    bool sensorChanged = false;
    if ((millis() - lastScanTime) > scanDelay) {
        lastScanTime = millis();
        
        if (bottomSensorPin > 0) {
            bottomSensorRead = digitalRead(bottomSensorPin); // PIR
        } else {
            bottomSensorRead = false; // DUNNO
        }
        
        if (topSensorPin > 0) {
            topSensorRead = digitalRead(topSensorPin); // PIR
        } else {
            topSensorRead = false; // DUNNO
        }

        if (bottomSensorRead != bottomSensorState) {
            bottomSensorState = bottomSensorRead; // change previous state
            sensorChanged = true;
        }

        if (topSensorRead != topSensorState) {
            topSensorState = topSensorRead; // change previous state
            sensorChanged = true;
        }

        // Values read, reset the flags for next API call
        topSensorWrite = false;
        bottomSensorWrite = false;
        
        if (topSensorRead != bottomSensorRead) {
            lastSwitchTime = millis();
            
            if (on) {
                lastSensor = topSensorRead;
            } else {
                // If the bottom sensor triggered, we need to swipe up, ON
                swipe = bottomSensorRead;
                     


                if (onIndex == offIndex) {
                    // Position the indices for a correct on-swipe
                    if (swipe == SWIPE_UP) {
                        onIndex = minSegmentId;
                    } else {
                        onIndex = maxSegmentId;
                    }
                    offIndex = onIndex;
                }
                on = true;
            }
        }
    }
    return sensorChanged;
}

void AniStairCaseLightV2::autoPowerOff() {
    if ((millis() - lastSwitchTime) > turnStepsOfAfterMs) {
        // if sensors are still on, do nothing
        //if (bottomSensorState || topSensorState) return;
        
        // Swipe OFF in the direction of the last sensor detection
        // WLED-MM/TroyHacks: This should follow you up/down the stairs.
        
        if (lastSensor == SWIPE_UP) {
            swipe = SWIPE_DOWN;
        } else {
            swipe = SWIPE_UP;
        }
        on = false;
        
    }
}

void AniStairCaseLightV2::updateSwipe() {
    if ((millis() - lastTime) > timeBetweenEachStepMs) {
        lastTime = millis();
        
        byte oldOn  = onIndex;
        byte oldOff = offIndex;
        
        if (on) {
            // Turn on all segments
            onIndex  = MAX(minSegmentId, onIndex - 1);
            offIndex = MIN(maxSegmentId, offIndex + 1);
        } else {
            if (swipe == SWIPE_UP) {
                onIndex = MIN(offIndex, onIndex + 1);
            } else {
                offIndex = MAX(onIndex, offIndex - 1);
            }
        }
        
        if (oldOn != onIndex || oldOff != offIndex) updateSegments(); // reduce the number of updates to necessary ones
    }
}

void AniStairCaseLightV2::updateSegments() {  
    if (transitionTimeMs > 0) {
        fadeTransition = true;
    } else {
        fadeTransition = false;
    }
    strip.setTransition(transitionTimeMs);

    for (int i = minSegmentId; i < maxSegmentId; i++) {
        Segment &seg = strip.getSegment(i);
        if (!seg.isActive()) continue; // skip gaps
        
       
        if (i >= onIndex && i < offIndex) {
            seg.setOption(SEG_OPTION_ON, true);
            
            // We may need to copy mode and colors from segment 0 to make sure
            // changes are propagated even when the config is changed during a wipe
            // seg.setMode(mainsegment.mode);
            // seg.setColor(0, mainsegment.colors[0]);
        } else {
            seg.setOption(SEG_OPTION_ON, false);
        }
        
        // Always mark segments as "transitional", we are animating the staircase
        //seg.setOption(SEG_OPTION_TRANSITIONAL, true); // not needed anymore as setOption() does it
    }
    strip.trigger();  // force strip refresh
    stateChanged = true;  // inform external devices/UI of change
    colorUpdated(CALL_MODE_DIRECT_CHANGE);
}

/*
===============================================================================================
== END OLD Animated_Staircase CODES
===============================================================================================
*/

void AniStairCaseLightV2::segmentSetup() {
    // Directs stop if ledsPerStepVector.size() == 0
    //if (ledsPerStepVector.size() == 0) return;    

    // Remove segments from the strip is there are more segments then needed
    if (strip._segments.size() > ledsPerStepVector.size()) {
        
        // Remove the last segment from strip until the size of ledsPerStepVector is the same as strip._segments
        strip.purgeSegments(true);

        while ((strip._segments.size() > ledsPerStepVector.size()) && ledsPerStepVector.size() > 0) {            
            
            strip.getSegment(strip.getLastActiveSegmentId()).stop = -1;

            strip.purgeSegments(true);
        }
    }

    // Add new segments to the strip if there are less then needed
    if (ledsPerStepVector.size() > strip._segments.size()) {
        // Add a new segmet to the strip until there are enough
        while (ledsPerStepVector.size() > strip._segments.size()) {
            strip.appendSegment(Segment(0, 1));
        }
    }

    // Update start stop leds of each segment
    int ledsInSegments = 0;
    for(int i = 0; i < ledsPerStepVector.size(); i++) {
        Segment &segment = strip.getSegment(i);
        segment.start = ledsInSegments;
        segment.stop = (ledsInSegments + ledsPerStepVector[i]);
        segment.refreshLightCapabilities();
        //segment.name = "";
        ledsInSegments += ledsPerStepVector[i];
    }
}

void AniStairCaseLightV2::stepsSetup()
{
    ledsPerStepVector.clear();

    int totalConfiguredLeds = strip.getLengthTotal();
    
    // Check if ledsPerStep is not NUL or Empty
    if (ledsPerStep == NULL || ledsPerStep.isEmpty()) {        
        return;
    }

     // Check if the delimiter char is in originalString 
    const char delimiter[] = ";";
    int lastIndexOfDelimiter = -1;
    int indexOfDelimiter = ledsPerStep.indexOf(delimiter, lastIndexOfDelimiter + 1);
    if (indexOfDelimiter < 0) {
        // delimiter found in ledsPerStep
        return;
    }

    // Add delimiter character to the end of ledsPerStep if 
    // the last character is not equal to the delimiter.
    if (ledsPerStep.endsWith(delimiter) == false) {
        ledsPerStep.concat(delimiter);
    }

    // Process the values in ledsPerStep
    String strValue;
    String newLedsPerStep = "";
    int totalLedsConfiguredForSteps = 0;        
    while (indexOfDelimiter >= 0) {
        // Get the value from ledsPerStep
        strValue = ledsPerStep.substring(lastIndexOfDelimiter + 1, indexOfDelimiter);

        // Trim the strValue value and convert it to an inter
        strValue.trim();
        int intValue = strValue.toInt();

        // Check if the intValue is greater then 0
        if (intValue > 0) {
            
            if ((totalLedsConfiguredForSteps + intValue) <= totalConfiguredLeds) {
                totalLedsConfiguredForSteps = totalLedsConfiguredForSteps + intValue;
                
                // Add the intValue to intVector
                ledsPerStepVector.push_back(intValue);
                
                // Add the intValue to the newLedsPerStep string and add a delimiter value afther it.
                newLedsPerStep.concat(String(intValue));
                newLedsPerStep.concat(delimiter);

            }

        }
  
        // Update lastIndexOfDelimiter and find the new indexOfDelimiter
        lastIndexOfDelimiter = indexOfDelimiter;
        indexOfDelimiter = ledsPerStep.indexOf(delimiter, lastIndexOfDelimiter + 1);
    }
    strValue.clear();

    // Check if ledsPerStep is diferent from newLedsPerStep
    if (ledsPerStep != newLedsPerStep) {
        // Update newLedsPerStep with the clean newLedsPerStep
        ledsPerStep = newLedsPerStep;
        
    }
    newLedsPerStep.clear();
}

void AniStairCaseLightV2::addToJsonInfo(JsonObject& root) {
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");    
    
    String uiDomString = F("<button class=\"btn btn-xs\" onclick=\"requestJson({");
    uiDomString += FPSTR(_name);
    uiDomString += F(":{");
    uiDomString += FPSTR(_enabled);
    uiDomString += enabled ? F(":false}});\">") : F(":true}});\">");
    uiDomString += F("<i class=\"icons");
    uiDomString += enabled ? F(" on") : F(" off");
    uiDomString += F("\">&#xe08f;</i>");
    uiDomString += F("</button>");    
    JsonArray infoArr = user.createNestedArray(FPSTR(_name));
    infoArr.add(uiDomString);   
    
    if (enabled) {
        uiDomString  = F("<br>Top triggered: ");
        uiDomString += AniStairSensorTop.triggered ? F("True") : F("False");
        uiDomString += F("<br>Bottom triggered: ");
        uiDomString += AniStairSensorBottom.triggered ? F("True") : F("False");
        infoArr.add(uiDomString);
    }    
}

void AniStairCaseLightV2::addToJsonState(JsonObject& root) {
    JsonObject jsonObj = root[FPSTR(_name)];
    if (jsonObj.isNull()) jsonObj = root.createNestedObject(FPSTR(_name));
}

void AniStairCaseLightV2::readFromJsonState(JsonObject& root) {
    if (!initDone) return;
    bool en = enabled;
    JsonObject jsonObj = root[FPSTR(_name)];
    if (!jsonObj.isNull()) {
        if (jsonObj[FPSTR(_enabled)].is<bool>()) {
            en = jsonObj[FPSTR(_enabled)].as<bool>();
        } else {
            String str = jsonObj[FPSTR(_enabled)]; // checkbox -> off or on
            en = (bool)(str!="off"); // off is guaranteed to be present
        }
        if (en != enabled) enable(en);
    }
}

void AniStairCaseLightV2::addToConfig(JsonObject &root) {
    // Add JSON object: {"Ani-Staircase": {"Enabled": false, "Top-Sensor-pin": 0, "Bottom-Sensor-pin": 0}}
    JsonObject top = root.createNestedObject(FPSTR(_name));
    
    top[FPSTR(_enabled)] = enabled;
    top[FPSTR(_topSensorPin)] = topSensorPin;
    top[FPSTR(_bottomSensorPin)] = bottomSensorPin;
    top[FPSTR(_turnStepsOfAfterMs)] = turnStepsOfAfterMs;
    top[FPSTR(_timeBetweenEachStepMs)] = timeBetweenEachStepMs;
    top[FPSTR(_transitionTimeMs)] = transitionTimeMs;
    top[FPSTR(_ledsPerStep)] = ledsPerStep;


}

bool AniStairCaseLightV2::readFromConfig(JsonObject &root) {
    // Look for JSON object: {"Ani-Staircase": {"Enabled": false, "Top-Sensor-pin": 0, "Bottom-Sensor-pin": 0}}
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        return false;
    }

    int8_t newTopSensorPin = topSensorPin;
    int8_t newBottomSensorPin = bottomSensorPin;
    int newTurnStepsOfAfterMs = turnStepsOfAfterMs;
    int newTimeBetweenEachStepMs = timeBetweenEachStepMs;
    int newTransitionTimeMs = transitionTimeMs;
    String newLedsPerStep = ledsPerStep;    
    
    enabled = top[FPSTR(_enabled)] | enabled;    
    newTopSensorPin = top[FPSTR(_topSensorPin)] | newTopSensorPin;
    newBottomSensorPin = top[FPSTR(_bottomSensorPin)] | newBottomSensorPin;
    newTurnStepsOfAfterMs = top[FPSTR(_turnStepsOfAfterMs)] | newTurnStepsOfAfterMs;
    newTimeBetweenEachStepMs = top[FPSTR(_timeBetweenEachStepMs)] | newTimeBetweenEachStepMs;
    newTransitionTimeMs = top[FPSTR(_transitionTimeMs)] | newTransitionTimeMs;
    newLedsPerStep = top[FPSTR(_ledsPerStep)] | newLedsPerStep;
    
    if (!initDone) {
        // First run: reading from cfg.json
        topSensorPin = newTopSensorPin;
        bottomSensorPin = newBottomSensorPin;
        turnStepsOfAfterMs = newTurnStepsOfAfterMs;
        timeBetweenEachStepMs = newTimeBetweenEachStepMs;
        transitionTimeMs = newTransitionTimeMs;
        ledsPerStep = newLedsPerStep;
    } else {
        
        // Check if there are changes in parameters from the settings page
        if (
               newTopSensorPin != topSensorPin 
            || newBottomSensorPin != bottomSensorPin 
            || newTurnStepsOfAfterMs != turnStepsOfAfterMs 
            || newTimeBetweenEachStepMs != timeBetweenEachStepMs
            || newTransitionTimeMs != transitionTimeMs
            || newLedsPerStep != ledsPerStep
        ) {
            // Deallocate top sensor pin
            pinManager.deallocatePin(topSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            topSensorPin = newTopSensorPin;

            // Deallocate bottom sensor pin
            pinManager.deallocatePin(bottomSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            bottomSensorPin = newBottomSensorPin;

            turnStepsOfAfterMs = newTurnStepsOfAfterMs;
            timeBetweenEachStepMs = newTimeBetweenEachStepMs;
            transitionTimeMs = newTransitionTimeMs;
            ledsPerStep = newLedsPerStep;             
            
            // Initialise
            setup();
        }
    }

    // Use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return !top[FPSTR(_ledsPerStep)].isNull();
}

void AniStairCaseLightV2::appendConfigData() {
    oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":ledsPerStep")); oappend(SET_F("',1,'<i class=\"warn\"><br>Specify the number of LEDs per for each step. Example 10;10;10 for 3 steps with 10 LEDs.</i>');"));
}

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

