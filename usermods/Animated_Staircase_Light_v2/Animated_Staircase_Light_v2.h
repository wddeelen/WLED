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
    unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
};

class AniStairCaseLightV2 : public Usermod {
    private:
        // Private class members.
        bool initDone = false;
        bool enabled = true;
        int8_t topSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_TOP_SENSOR_PIN;
        int8_t bottomSensorPin = USERMOD_ANIMATED_STAIRCASE_V2_BOTTOM_SENSOR_PIN;
        bool topSensorTriggered = false;
        bool bottomSensorTriggered = false;
        String ledsPerStep;
        unsigned long currentLoopTime;
        unsigned long lastLoopTime;
        AniStairSensor AniStairSensorTop;
        AniStairSensor AniStairSensorBottom;
        int totalSteps = 0;

        // Strings to reduce flash memory usage (used more than twice)
        static const char _name[];
        static const char _enabled[];
        static const char _topSensor[];
        static const char _topSensorPin[];
        static const char _bottomSensor[];
        static const char _bottomSensorPin[];
        static const char _ledsPerStep[];
        static const int8_t _debounceTime;
        int stepLeds[0];
        
    public:
        inline void enable(bool enable) { enabled = enable; }
        inline bool isEnabled() { return enabled; }
        uint16_t getId() { return USERMOD_ID_ANIMATED_STAIRCASE_V2; }
        void setup();
        void populateArrStepLeds();
        void loop();
        void testStringToIntArray();
        void addToJsonInfo(JsonObject& root);
        void addToJsonState(JsonObject& root);
        void readFromJsonState(JsonObject& root);
        void addToConfig(JsonObject &root);
        bool readFromConfig(JsonObject &root);
        void appendConfigData();

};

void AniStairCaseLightV2::setup() {
    // Validate pin numbers and set to -1 if they are not valid.
    if (topSensorPin < 0) topSensorPin = -1;
    if (bottomSensorPin < 0) bottomSensorPin = -1;

    // Check if the usermod is enabled
    if (enabled) {
        // Config says we are enabled
        Serial.println(F("AniStairCaseLightV2: Allocating top and bottom sensor pins..."));

        // Allocate Top Sensor pin
        if (topSensorPin >= 0 && pinManager.allocatePin(topSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
            // Set the pin mode for the Top sensor pin.
            pinMode(topSensorPin, INPUT_PULLUP);
            Serial.print(F("AniStairCaseLightV2: Top sensor pin "));
            Serial.print(topSensorPin);
            Serial.println(F(" allocated."));
            
            // Allocate Bottom Sensor pin
            if (bottomSensorPin >= 0 && pinManager.allocatePin(bottomSensorPin, true, PinOwner::UM_ANIMATED_STAIRCASE_V2)) {
                // Set the pin mode for the Bottom sensor pin.
                pinMode(bottomSensorPin, INPUT_PULLUP);
                Serial.print(F("AniStairCaseLightV2: Bottom sensor pin "));
                Serial.print(bottomSensorPin);
                Serial.println(F(" allocated."));

                populateArrStepLeds();

            } else {
                if (bottomSensorPin >= 0) {
                    Serial.print(F("AniStairCaseLightV2: Bottom sensor pin "));
                    Serial.print(bottomSensorPin);
                    Serial.println(F(" allocation failed."));
                }
                bottomSensorPin = -1;
            }
        } else {
            if (topSensorPin >= 0) {
                Serial.print(F("AniStairCaseLightV2: Top sensor pin "));
                Serial.print(topSensorPin);
                Serial.println(F(" allocation failed."));
            }
            topSensorPin = -1;
        }
    }
    enable(enabled);
    initDone = true;
}

void AniStairCaseLightV2::populateArrStepLeds()
{
    // Create an int array of the ledsPerStep string
    Serial.println(F("AniStairCaseLightV2: START CONVERT String ledsPerStep TO int stepLeds[]"));

    // Check if ledsPerStep is not NULL or emty
 




 /*   
    // ledsPerStep
    
    char *ptr;

    //memset(stepLeds, 0, sizeof(stepLeds));

    int index = 0;
    char delimiters[] = "|";

    //String welp = ledsPerStep;

    Serial.print("AniStairCaseLightV2: 01 ledsPerStep=");
    Serial.println(ledsPerStep);
    //Serial.print("AniStairCaseLightV2: 01 welp=");
    //Serial.println(welp);

    char *testString = new char[ledsPerStep.length() + 1];
    strcpy(testString, ledsPerStep.c_str());
    
    ptr = strtok(testString, delimiters);
    while (ptr != NULL)
    {
        stepLeds[index] = atoi(ptr);
        index++;
        ptr = strtok(NULL, delimiters);
    }
    totalSteps = index;
    Serial.print("AniStairCaseLightV2: totalSteps = ");
    Serial.println(totalSteps);

    for (int i = 0; i < totalSteps; i++)
    {
        Serial.print("AniStairCaseLightV2: stepLeds[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(stepLeds[i]);
    }

    Serial.print("AniStairCaseLightV2: 02 ledsPerStep=");
    Serial.println(ledsPerStep);
    //Serial.print("AniStairCaseLightV2: 02 welp=");
    //Serial.println(welp);
*/
    Serial.println(F("AniStairCaseLightV2: END CONVERT String ledsPerStep TO int stepLeds[]"));
}

void AniStairCaseLightV2::loop() {
    // Stop directly if the usermod is disabled or the strip is updating
    if (!enabled || strip.isUpdating()) return;

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
            Serial.println("AniStairCaseLightV2: AniStairSensorTop is pressed");

            Serial.print("AniStairCaseLightV2: ledsPerStep=");
            Serial.println(ledsPerStep);
            
            byte rgbColor[3];
            rgbColor[0]=255; rgbColor[1]=0; rgbColor[2]=0; rgbColor[3]=128;
            int startLedVan = 0;
            
            for (int i = 0; i < totalSteps; i++) {
                Serial.print("AniStairCaseLightV2: stepLeds[");
                Serial.print(i);
                Serial.print("] = ");
                Serial.println(stepLeds[i]);
                
                strip.setRange(startLedVan, (startLedVan + stepLeds[i]), colorFromRgbw(rgbColor));
                startLedVan = startLedVan + stepLeds[i];
            }

            strip.trigger();  // force strip refresh
            stateChanged = true;  // inform external devices/UI of change
            colorUpdated(CALL_MODE_DIRECT_CHANGE);

        } else if(AniStairSensorTop.lastSteadyState == LOW && AniStairSensorTop.currentState == HIGH) {
            Serial.println("AniStairCaseLightV2: AniStairSensorTop is released");
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
            Serial.println("AniStairCaseLightV2: AniStairSensorBottom is pressed");

            testStringToIntArray();

        } else if(AniStairSensorBottom.lastSteadyState == LOW && AniStairSensorBottom.currentState == HIGH) {
            Serial.println("AniStairCaseLightV2: AniStairSensorBottom is released");
        }
        AniStairSensorBottom.lastSteadyState = AniStairSensorBottom.currentState;
    }




    // Update the value of lastLoopTime with currentLoopTime
    lastLoopTime = currentLoopTime;
}

String originalString = "";//"10;20;30;40;50;60;70;80;90;100;110;120;130;140;";
int intArray[0];
std::vector<int> intVector;

void AniStairCaseLightV2::testStringToIntArray()
{
    // Check if originalString is not NUL or Empty
    if (originalString == NULL || originalString.isEmpty()) {
        Serial.println("originalString is NULL or Empty!");
        intVector.clear();
        return;
    }

    // Check if the delimiter char is in originalString 
    char delimiter[] = "|";
    int indexOfDelimiter = originalString.indexOf(delimiter);
    if (indexOfDelimiter >= 0) {
        // delimiter found in originalString
        Serial.print("delimiter ");
        Serial.print(delimiter);
        Serial.println(" found in originalString");


    } else {
        // delimiter NOT found in originalString
        Serial.print("delimiter ");
        Serial.print(delimiter);
        Serial.println(" NOT found in originalString");

        // Check if originalString is not a int value
        bool welp = originalString.toInt();


        //if (isdigit(originalString) == false) {

        //}
        

        // intVector.clear();
        
    }

    Serial.print("indexOfDelimiter = ");
    Serial.println(indexOfDelimiter);







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
        uiDomString += topSensorTriggered ? F("True") : F("False");
        uiDomString += F("<br>Bottom triggered: ");
        uiDomString += bottomSensorTriggered ? F("True") : F("False");
        infoArr.add(uiDomString);
    }

    /*
    // WEET NIET OF DEZE CODE NODIG IS
    JsonObject sensorTop = root[F(_topSensor)];
    if (sensorTop.isNull()) sensorTop = root.createNestedObject(F(_topSensor));
    sensorTop[F("motion")] = topSensorTriggered;

    JsonObject sensorBottom = root[F(_bottomSensor)];
    if (sensorBottom.isNull()) sensorBottom = root.createNestedObject(F(_bottomSensor));
    sensorBottom[F("motion")] = bottomSensorTriggered;
    */
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
    top[FPSTR(_ledsPerStep)] = ledsPerStep;
    Serial.println(F("AniStairCaseLightV2: Config saved."));
}

bool AniStairCaseLightV2::readFromConfig(JsonObject &root) {
    // Look for JSON object: {"Ani-Staircase": {"Enabled": false, "Top-Sensor-pin": 0, "Bottom-Sensor-pin": 0}}
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) {
        Serial.println(F("AniStairCaseLightV2: No config found. (Using defaults.)"));
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
        Serial.println(F("AniStairCaseLightV2: Config loaded from cfg.json file."));
    } else {
        Serial.println(F("AniStairCaseLightV2: Config (re)loaded."));
        
        // Check if there are changes in parameters from the settings page
        if (newTopSensorPin != topSensorPin || newBottomSensorPin != bottomSensorPin || newLedsPerStep != ledsPerStep) {
            // Deallocate top sensor pin
            pinManager.deallocatePin(topSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            Serial.print(F("AniStairCaseLightV2: Top sensor pin "));
            Serial.print(topSensorPin);
            Serial.println(F(" deallocated."));
            topSensorPin = newTopSensorPin;

            // Deallocate bottom sensor pin
            pinManager.deallocatePin(bottomSensorPin, PinOwner::UM_ANIMATED_STAIRCASE_V2);
            Serial.print(F("AniStairCaseLightV2: Bottom sensor pin "));
            Serial.print(bottomSensorPin);
            Serial.println(F(" deallocated."));
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
