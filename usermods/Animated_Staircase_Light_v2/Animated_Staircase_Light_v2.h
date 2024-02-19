#pragma once
#include "wled.h"

#ifndef USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN
#define USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN 35
#endif
#ifndef USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN
#define USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN 36
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
        String ledsPerStep;
        
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

/*
===============================================================================================
== START OLD Animated_Staircase vars and functions
===============================================================================================
*/

        // OLD Animated_Staircase vars and functions
        bool checkSensors();
        void autoPowerOff();
        void updateSwipe();
        void updateSegments();
        unsigned long lastScanTime      = 0;        // Time of the last sensor check
        const unsigned int scanDelay    = 100;      // Time between checking of the sensors
        unsigned long lastSwitchTime    = 0;        // Last time the lights were switched on or off
        bool on                         = false;     // Lights on or off. (Flipping this will start a transition.)
        unsigned long on_time_ms        = 5000;     // The time for the light to stay on -  TroyHacks: 5s for testing
        unsigned long segment_delay_ms  = 150;      // Time between switching each segment

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


/*
===============================================================================================
== END OLD Animated_Staircase vars and functions
===============================================================================================
*/
        
        // Strings to reduce flash memory usage (used more than twice)
        static const char _name[];
        static const char _enabled[];
        static const char _topSensor[];
        static const char _topSensorPin[];
        static const char _bottomSensor[];
        static const char _bottomSensorPin[];
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

};

#define debugPrintDelay 0
void AniStairCaseLightV2::debugPrint(String val) { Serial.print(val); if (debugPrintDelay > 0) delay(debugPrintDelay); }
void AniStairCaseLightV2::debugPrintLn(String val) { Serial.println(val); if (debugPrintDelay > 0) delay(debugPrintDelay); }
void AniStairCaseLightV2::debugPrint(int val) { debugPrint(String(val)); }
void AniStairCaseLightV2::debugPrintLn(int val) { debugPrintLn(String(val));}

void AniStairCaseLightV2::setup() {
    debugPrintLn("DEBUG AniStairCaseLightV2::setup()");
    // Validate pin numbers and set to -1 if they are not valid.
    if (topSensorPin < 0) topSensorPin = -1;
    if (bottomSensorPin < 0) bottomSensorPin = -1;

    // Check if the usermod is enabled
    if (enabled) {
        // Config says we are enabled
        debugPrintLn(F("AniStairCaseLightV2: Allocating top and bottom sensor pins..."));

        // Allocate Top Sensor pin
        if (topSensorPin >= 0 && pinManager.allocatePin(topSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
            // Set the pin mode for the Top sensor pin.
            pinMode(topSensorPin, INPUT_PULLUP);
            debugPrint(F("AniStairCaseLightV2: Top sensor pin "));
            debugPrint(topSensorPin);
            debugPrintLn(F(" allocated."));
            
            // Allocate Bottom Sensor pin
            if (bottomSensorPin >= 0 && pinManager.allocatePin(bottomSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
                // Set the pin mode for the Bottom sensor pin.
                pinMode(bottomSensorPin, INPUT_PULLUP);
                debugPrint(F("AniStairCaseLightV2: Bottom sensor pin "));
                debugPrint(bottomSensorPin);
                debugPrintLn(F(" allocated."));


                stepsSetup();

            } else {
                if (bottomSensorPin >= 0) {
                    debugPrint(F("AniStairCaseLightV2: Bottom sensor pin "));
                    debugPrint(bottomSensorPin);
                    debugPrintLn(F(" allocation failed."));
                }
                bottomSensorPin = -1;
            }
        } else {
            if (topSensorPin >= 0) {
                debugPrint(F("AniStairCaseLightV2: Top sensor pin "));
                debugPrint(topSensorPin);
                debugPrintLn(F(" allocation failed."));
            }
            topSensorPin = -1;
        }
    }
    enable(enabled);
    initDone = true;
}

bool lastCheckSensors = false;
void AniStairCaseLightV2::loop() {
    // Stop directly if the usermod is disabled or the strip is updating
    if (!enabled || !initDone || strip.isUpdating()) return;

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
    
    
    
    
    
 /* 
    // Update the currentLoopTime var with millis()
    currentLoopTime = millis();
    
    // Check AniStairSensorTop
    AniStairSensorTop.currentState = digitalRead(topSensorPin);
    if (AniStairSensorTop.currentState != AniStairSensorTop.lastFlickerableState) {
        AniStairSensorTop.lastDebounceTime = currentLoopTime;
        AniStairSensorTop.lastFlickerableState = AniStairSensorTop.currentState;
    }
    if ((currentLoopTime - AniStairSensorTop.lastDebounceTime) > _debounceTime) {
        if(AniStairSensorTop.lastSteadyState == HIGH && AniStairSensorTop.currentState == LOW) {
            debugPrintLn("AniStairCaseLightV2: AniStairSensorTop is pressed");
            AniStairSensorTop.triggered = true;
            stateChanged = true;  // inform external devices/UI of change
            segmentSetup();
            
        } else if(AniStairSensorTop.lastSteadyState == LOW && AniStairSensorTop.currentState == HIGH) {
            debugPrintLn("AniStairCaseLightV2: AniStairSensorTop is released");
            AniStairSensorTop.triggered = false;
            stateChanged = true;  // inform external devices/UI of change

        }
        AniStairSensorTop.lastSteadyState = AniStairSensorTop.currentState;
    }

    // Check AniStairSensorBottom
    AniStairSensorBottom.currentState = digitalRead(bottomSensorPin);
    if (AniStairSensorBottom.currentState != AniStairSensorBottom.lastFlickerableState) {
        AniStairSensorBottom.lastDebounceTime = currentLoopTime;
        AniStairSensorBottom.lastFlickerableState = AniStairSensorBottom.currentState;
    }
    if ((currentLoopTime - AniStairSensorBottom.lastDebounceTime) > _debounceTime) {
        if(AniStairSensorBottom.lastSteadyState == HIGH && AniStairSensorBottom.currentState == LOW) {
            debugPrintLn("AniStairCaseLightV2: AniStairSensorBottom is pressed");
            AniStairSensorBottom.triggered = true;
            stateChanged = true;  // inform external devices/UI of change
            segmentSetup();

        } else if(AniStairSensorBottom.lastSteadyState == LOW && AniStairSensorBottom.currentState == HIGH) {
            debugPrintLn("AniStairCaseLightV2: AniStairSensorBottom is released");
            AniStairSensorBottom.triggered = false;
            stateChanged = true;  // inform external devices/UI of change


        }
        AniStairSensorBottom.lastSteadyState = AniStairSensorBottom.currentState;
    }
*/
}

/*
===============================================================================================
== START OLD Animated_Staircase CODES
===============================================================================================
*/

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
            debugPrintLn("Bottom sensor changed.");
        }

        if (topSensorRead != topSensorState) {
            topSensorState = topSensorRead; // change previous state
            sensorChanged = true;
            debugPrintLn("Top sensor changed.");
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
                
                debugPrint("ON -> Swipe ");
                debugPrintLn(swipe ? "up." : "down.");         


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


long autoPowerOfflastScanTime=0;
void AniStairCaseLightV2::autoPowerOff() {  

    if (((millis() - autoPowerOfflastScanTime) > 750) && 1==2) {
        autoPowerOfflastScanTime = millis();

        debugPrintLn("DEBUG AniStairCaseLightV2::autoPowerOff()");

        debugPrint("millis() = ");
        debugPrintLn(millis());

        debugPrint("lastSwitchTime = ");
        debugPrintLn(lastSwitchTime);

        debugPrint("on_time_ms = ");
        debugPrintLn(on_time_ms);

        debugPrint("(millis() - lastSwitchTime) = ");
        debugPrintLn((millis() - lastSwitchTime));

        debugPrint("((millis() - lastSwitchTime) > on_time_ms) = ");
        debugPrintLn(((millis() - lastSwitchTime) > on_time_ms));

        debugPrint("bottomSensorState = ");
        debugPrintLn(bottomSensorState);

        debugPrint("topSensorState = ");
        debugPrintLn(topSensorState);

        debugPrintLn("-----------------------------------------------------");

    }
   



    if ((millis() - lastSwitchTime) > on_time_ms) {
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
        
        //debugPrint("OFF -> Swipe ");
        //debugPrintLn(swipe ? "up." : "down.");
    }
}

void AniStairCaseLightV2::updateSwipe() {
    //debugPrintLn("DEBUG AniStairCaseLightV2::updateSwipe()");
    if ((millis() - lastTime) > segment_delay_ms) {
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
    //debugPrintLn("DEBUG AniStairCaseLightV2::updateSegments()");
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
    debugPrintLn("DEBUG AniStairCaseLightV2::segmentSetup()");
    // Remove segments from the strip is there are more segments then needed
    if (strip._segments.size() > ledsPerStepVector.size()) {
        // Remove the last segment from strip until the size of ledsPerStepVector is the same as strip._segments
        strip.purgeSegments(true);
        while (strip._segments.size() > ledsPerStepVector.size()) {
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
        ledsInSegments += ledsPerStepVector[i];

        //segment.setMode(FX_MODE_RANDOM_COLOR, true);
        //segment.speed = 255;
        //segment.intensity = 255;
    }
}

void AniStairCaseLightV2::stepsSetup()
{
    debugPrintLn("DEBUG AniStairCaseLightV2::stepsSetup()");
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
    debugPrintLn("DEBUG AniStairCaseLightV2::addToJsonInfo()");
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

    /*
    // WEET NIET OF DEZE CODE NODIG IS
    JsonObject sensorTop = root[F(_topSensor)];

void AniStairCaseLightV2::debugPrint(int lastIndexOfDelimiter)
{
debugPrintLn(lastIndexOfDelimiter);
}
   if (sensorTop.isNull()) sensorTop = root.createNestedObject(F(_topSensor));
    sensorTop[F("motion")] = topSensorTriggered;

    JsonObject sensorBottom = root[F(_bottomSensor)];
    if (sensorBottom.isNull()) sensorBottom = root.createNestedObject(F(_bottomSensor));
    sensorBottom[F("motion")] = bottomSensorTriggered;
    */
}

void AniStairCaseLightV2::addToJsonState(JsonObject& root) {
    debugPrintLn("DEBUG AniStairCaseLightV2::addToJsonState()");
    JsonObject jsonObj = root[FPSTR(_name)];
    if (jsonObj.isNull()) jsonObj = root.createNestedObject(FPSTR(_name));
}

void AniStairCaseLightV2::readFromJsonState(JsonObject& root) {
    debugPrintLn("DEBUG AniStairCaseLightV2::readFromJsonState()");
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
    debugPrintLn("DEBUG AniStairCaseLightV2::addToConfig()");
    // Add JSON object: {"Ani-Staircase": {"Enabled": false, "Top-Sensor-pin": 0, "Bottom-Sensor-pin": 0}}
    JsonObject top = root.createNestedObject(FPSTR(_name));
    
    top[FPSTR(_enabled)] = enabled;
    top[FPSTR(_topSensorPin)] = topSensorPin;
    top[FPSTR(_bottomSensorPin)] = bottomSensorPin;
    top[FPSTR(_ledsPerStep)] = ledsPerStep;


    debugPrintLn(F("AniStairCaseLightV2: Config saved."));
}

bool AniStairCaseLightV2::readFromConfig(JsonObject &root) {
    debugPrintLn("DEBUG AniStairCaseLightV2::readFromConfig()");
    // Look for JSON object: {"Ani-Staircase": {"Enabled": false, "Top-Sensor-pin": 0, "Bottom-Sensor-pin": 0}}
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        debugPrintLn(F("AniStairCaseLightV2: No config found. (Using defaults.)"));
        return false;
    }

    int8_t newTopSensorPin = topSensorPin;
    int8_t newBottomSensorPin = bottomSensorPin;
    String newLedsPerStep = ledsPerStep;
    
    enabled = top[FPSTR(_enabled)] | enabled;
    newTopSensorPin = top[FPSTR(_topSensorPin)] | newTopSensorPin;
    newBottomSensorPin = top[FPSTR(_bottomSensorPin)] | newBottomSensorPin;
    newLedsPerStep = top[FPSTR(_ledsPerStep)] | newLedsPerStep;
    
    if (!initDone) {
        // First run: reading from cfg.json
        topSensorPin = newTopSensorPin;
        bottomSensorPin = newBottomSensorPin;
        ledsPerStep = newLedsPerStep;
        debugPrintLn(F("AniStairCaseLightV2: Config loaded from cfg.json file."));
    } else {
        debugPrintLn(F("AniStairCaseLightV2: Config (re)loaded."));
        
        // Check if there are changes in parameters from the settings page
        if (newTopSensorPin != topSensorPin || newBottomSensorPin != bottomSensorPin || newLedsPerStep != ledsPerStep) {
            // Deallocate top sensor pin
            pinManager.deallocatePin(topSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            debugPrint(F("AniStairCaseLightV2: Top sensor pin "));
            debugPrint(topSensorPin);
            debugPrintLn(F(" deallocated."));
            topSensorPin = newTopSensorPin;

            // Deallocate bottom sensor pin
            pinManager.deallocatePin(bottomSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            debugPrint(F("AniStairCaseLightV2: Bottom sensor pin "));
            debugPrint(bottomSensorPin);
            debugPrintLn(F(" deallocated."));
            bottomSensorPin = newBottomSensorPin;

            ledsPerStep = newLedsPerStep;             
            
            // Initialise
            setup();
        }
    }

    // Use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return !top[FPSTR(_ledsPerStep)].isNull();
}

void AniStairCaseLightV2::appendConfigData() {
    debugPrintLn("DEBUG AniStairCaseLightV2::appendConfigData()");
    oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":ledsPerStep")); oappend(SET_F("',1,'<i class=\"warn\"><br>Specify the number of LEDs per for each step. Example 30;30;30 for 3 steps with 30 LEDs.</i>');"));
}

// Strings to reduce flash memory usage (used more than twice)
const char AniStairCaseLightV2::_name[]             PROGMEM = "AniStaircase";
const char AniStairCaseLightV2::_enabled[]          PROGMEM = "enabled";
const char AniStairCaseLightV2::_topSensor[]        PROGMEM = "topSensor";
const char AniStairCaseLightV2::_topSensorPin[]     PROGMEM = "topSensorpin";
const char AniStairCaseLightV2::_bottomSensor[]     PROGMEM = "bottomSensor";
const char AniStairCaseLightV2::_bottomSensorPin[]  PROGMEM = "bottomSensorpin";
const char AniStairCaseLightV2::_ledsPerStep[]      PROGMEM = "ledsPerStep";
const int8_t AniStairCaseLightV2::_debounceTime     PROGMEM = 50;
