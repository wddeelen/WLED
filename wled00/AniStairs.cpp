#include "../usermods/AniStairs/AniStairs.h"

// Strings to reduce flash memory usage (used more than twice)
const char AniStairs::_name[]                               PROGMEM = "AniStairs";
const char AniStairs::_enabled[]                            PROGMEM = "Enabled";
const char AniStairs::_sensorTopTriggerPin[]                PROGMEM = "TopSensorTriggerpin";
const char AniStairs::_sensorTopEchoPin[]                   PROGMEM = "TopSensorEchopin";
const char AniStairs::_sensorTopMinTriggerDistanceCm[]      PROGMEM = "TopMinTriggerCm";
const char AniStairs::_sensorTopMaxTriggerDistanceCm[]      PROGMEM = "TopMaxTriggerCm";
const char AniStairs::_sensorBottomTriggerPin[]             PROGMEM = "BottomSensorTriggerpin";
const char AniStairs::_sensorBottomEchoPin[]                PROGMEM = "BottomSensorEchopin";
const char AniStairs::_sensorBottomMinTriggerDistanceCm[]   PROGMEM = "BottomMinTriggerCm";
const char AniStairs::_sensorBottomMaxTriggerDistanceCm[]   PROGMEM = "BottomMaxTriggerCm";
const char AniStairs::_timeBetweenEachStepMs[]              PROGMEM = "TimeBetweenEachStepMs";
const char AniStairs::_timeThatEachStepStaysOnMin[]         PROGMEM = "TimeThatEachStepStaysOnMin";
const char AniStairs::_transitionTimeMs[]                   PROGMEM = "TransitionTimeMs";
const char AniStairs::_ledsPerStep[]                        PROGMEM = "LedsPerStep";

uint8_t AniStairs::ReAllocatePin(uint8_t oldPin, uint8_t newPin) {
    // Check if oldPin is bigger or equal to 0 and the old
    // pin is owned by this usermod (UM_ANISTAIRS)
    if (oldPin >= 0 && pinManager.getPinOwner(oldPin) == PinOwner::UM_ANISTAIRS) {
        pinManager.deallocatePin(oldPin, PinOwner::UM_ANISTAIRS);
    }

    // Check if newPin is bigger or equal to 0 and and cehck
    // if the pin not already in use by somthing else
    if (newPin >= 0 && pinManager.getPinOwner(newPin) == PinOwner::None) {
        // Allocate the newPin
        if (pinManager.allocatePin(newPin, true, PinOwner::UM_ANISTAIRS)) {
            return newPin;
        }
    }
    return -1;
}

void AniStairs::setup() {
    //debugPrintLn("void AniStairs::setup() : 01");
    // Check if the usermod is enabled
    if (enabled) {
        stair.SensorTop.SetTriggerPin(ReAllocatePin(stair.SensorTop.GetTriggerPin(), stair.SensorTop.GetTriggerPin()));
        stair.SensorTop.SetEchoPin(ReAllocatePin(stair.SensorTop.GetEchoPin(), stair.SensorTop.GetEchoPin()));
        stair.SensorBottom.SetTriggerPin(ReAllocatePin(stair.SensorBottom.GetTriggerPin(), stair.SensorBottom.GetTriggerPin()));
        stair.SensorBottom.SetEchoPin(ReAllocatePin(stair.SensorBottom.GetEchoPin(), stair.SensorBottom.GetEchoPin()));
        stair.Setup();
    }
    enable(enabled);
    initDone = true;
}

void AniStairs::loop() {
    if (enabled && initDone) {
        stair.Loop();
    }
}

void AniStairs::addToJsonInfo(JsonObject& root) {
    //debugPrintLn("void AniStairs::addToJsonInfo(JsonObject& root) : 01");
    JsonObject user = root["u"];
    if (user.isNull()) user = root.createNestedObject("u");

    JsonArray infoArr = user.createNestedArray(FPSTR(_name));

    String uiDomString = F("<button class=\"btn btn-xs\" onclick=\"requestJson({");
    uiDomString += FPSTR(_name);
    uiDomString += F(":{");
    uiDomString += FPSTR(_enabled);
    uiDomString += enabled ? F(":false}});\">") : F(":true}});\">");
    uiDomString += F("<i class=\"icons");
    uiDomString += enabled ? F(" on") : F(" off");
    uiDomString += F("\">&#xe08f;</i>");
    uiDomString += F("</button>");
    infoArr.add(uiDomString);

    if (enabled) {
        uiDomString  = F("<br>Top sensor : ");
        uiDomString += "<b";
        if (stair.SensorTop.IsObjectInTriggerRange()) uiDomString += " style=\"color:green;\"";
        uiDomString += ">";        
        uiDomString += stair.SensorTop.GetDistance();
        uiDomString += F("</b> cm");

        uiDomString += F("<br>Bottom sensor : ");
        uiDomString += "<b";
        if (stair.SensorBottom.IsObjectInTriggerRange()) uiDomString += " style=\"color:green;\"";
        uiDomString += ">";        
        uiDomString += stair.SensorBottom.GetDistance();
        uiDomString += F("</b> cm");

        infoArr.add(uiDomString);
    }

}

void AniStairs::addToJsonState(JsonObject& root) {
    //debugPrintLn("void AniStairs::addToJsonState(JsonObject& root) : 01");
    JsonObject jsonObj = root[FPSTR(_name)];
    if (jsonObj.isNull()) jsonObj = root.createNestedObject(FPSTR(_name));
}

void AniStairs::readFromJsonState(JsonObject& root) {
    //debugPrintLn("void AniStairs::readFromJsonState(JsonObject& root) : 01");
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

void AniStairs::addToConfig(JsonObject &root) {
    //debugPrintLn("void AniStairs::addToConfig(JsonObject &root) : 01");
    JsonObject top = root.createNestedObject(FPSTR(_name));
    top[FPSTR(_enabled)]                            = enabled;
    top[FPSTR(_sensorTopTriggerPin)]                = stair.SensorTop.GetTriggerPin();
    top[FPSTR(_sensorTopEchoPin)]                   = stair.SensorTop.GetEchoPin();
    top[FPSTR(_sensorTopMinTriggerDistanceCm)]      = stair.SensorTop.GetMinTriggerDistance();
    top[FPSTR(_sensorTopMaxTriggerDistanceCm)]      = stair.SensorTop.GetMaxTriggerDistance();    
    top[FPSTR(_sensorBottomTriggerPin)]             = stair.SensorBottom.GetTriggerPin();
    top[FPSTR(_sensorBottomEchoPin)]                = stair.SensorBottom.GetEchoPin();
    top[FPSTR(_sensorBottomMinTriggerDistanceCm)]   = stair.SensorBottom.GetMinTriggerDistance();
    top[FPSTR(_sensorBottomMaxTriggerDistanceCm)]   = stair.SensorBottom.GetMaxTriggerDistance(); 
    top[FPSTR(_timeBetweenEachStepMs)]              = stair.GetTimeBetweenEachStepMs();
    top[FPSTR(_timeThatEachStepStaysOnMin)]         = stair.GetTimeThatEachStepStaysOnMin();
    top[FPSTR(_transitionTimeMs)]                   = stair.GetTransitionTimeMs();
    top[FPSTR(_ledsPerStep)]                        = stair.GetLedsPerStep();
}

bool AniStairs::readFromConfig(JsonObject &root) {
    //debugPrintLn("bool AniStairs::readFromConfig(JsonObject &root) : 01");
    JsonObject top = root[FPSTR(_name)];
    if (top.isNull()) return false;
    
    // Load the old config values in temp vars
    bool newEnabled                     = enabled;
    uint8_t newSensorTopTriggerPin      = stair.SensorTop.GetTriggerPin();
    uint8_t newSensorTopEchoPin         = stair.SensorTop.GetEchoPin();    
    int newSensorTopMinTriggerCm        = stair.SensorTop.GetMinTriggerDistance();
    int newSensorTopMaxTriggerCm        = stair.SensorTop.GetMaxTriggerDistance();
    uint8_t newSensorBottomTriggerPin   = stair.SensorBottom.GetTriggerPin();
    uint8_t newSensorBottomEchoPin      = stair.SensorBottom.GetEchoPin();
    int newSensorBottomMinTriggerCm     = stair.SensorBottom.GetMinTriggerDistance();
    int newSensorBottomMaxTriggerCm     = stair.SensorBottom.GetMaxTriggerDistance();
    int newTimeBetweenEachStepMs        = stair.GetTimeBetweenEachStepMs();
    float newTimeThatEachStepStaysOnMin = stair.GetTimeThatEachStepStaysOnMin();
    int newTransitionTimeMs             = stair.GetTransitionTimeMs();
    String newLedsPerStep               = stair.GetLedsPerStep();
    
    newEnabled                      = top[FPSTR(_enabled)]                          | newEnabled;    
    newSensorTopTriggerPin          = top[FPSTR(_sensorTopTriggerPin)]              | newSensorTopTriggerPin;
    newSensorTopEchoPin             = top[FPSTR(_sensorTopEchoPin)]                 | newSensorTopEchoPin;
    newSensorTopMinTriggerCm        = top[FPSTR(_sensorTopMinTriggerDistanceCm)]    | newSensorTopMinTriggerCm;
    newSensorTopMaxTriggerCm        = top[FPSTR(_sensorTopMaxTriggerDistanceCm)]    | newSensorTopMaxTriggerCm;
    newSensorBottomTriggerPin       = top[FPSTR(_sensorBottomTriggerPin)]           | newSensorBottomTriggerPin;
    newSensorBottomEchoPin          = top[FPSTR(_sensorBottomEchoPin)]              | newSensorBottomEchoPin;
    newSensorBottomMinTriggerCm     = top[FPSTR(_sensorBottomMinTriggerDistanceCm)] | newSensorBottomMinTriggerCm;
    newSensorBottomMaxTriggerCm     = top[FPSTR(_sensorBottomMaxTriggerDistanceCm)] | newSensorBottomMaxTriggerCm;
    newTimeBetweenEachStepMs        = top[FPSTR(_timeBetweenEachStepMs)]            | newTimeBetweenEachStepMs;
    newTimeThatEachStepStaysOnMin   = top[FPSTR(_timeThatEachStepStaysOnMin)]       | newTimeThatEachStepStaysOnMin;
    newTransitionTimeMs             = top[FPSTR(_transitionTimeMs)]                 | newTransitionTimeMs;
    newLedsPerStep                  = top[FPSTR(_ledsPerStep)]                      | newLedsPerStep;
    
    // Check init state
    if (!initDone) {
        // Init is not done. Load the settings from cfg.json
        enabled = newEnabled;
        stair.SensorTop.SetTriggerPin(newSensorTopTriggerPin);
        stair.SensorTop.SetEchoPin(newSensorTopEchoPin);
        stair.SensorTop.SetMinTriggerDistance(newSensorTopMinTriggerCm);
        stair.SensorTop.SetMaxTriggerDistance(newSensorTopMaxTriggerCm);
        stair.SensorBottom.SetTriggerPin(newSensorBottomTriggerPin);
        stair.SensorBottom.SetEchoPin(newSensorBottomEchoPin);
        stair.SensorBottom.SetMinTriggerDistance(newSensorBottomMinTriggerCm);
        stair.SensorBottom.SetMaxTriggerDistance(newSensorBottomMaxTriggerCm);
        stair.SetTimeBetweenEachStepMs(newTimeBetweenEachStepMs);
        stair.SetTimeThatEachStepStaysOnMin(newTimeThatEachStepStaysOnMin);
        stair.SetTransitionTimeMs(newTransitionTimeMs);
        stair.SetLedsPerStep(newLedsPerStep);

    } else {
        // Check if there are changes in parameters from the settings page
        if (1 == 2
            || newEnabled                       != enabled
            || newSensorTopTriggerPin           != stair.SensorTop.GetTriggerPin()
            || newSensorTopEchoPin              != stair.SensorTop.GetEchoPin()
            || newSensorTopMinTriggerCm         != stair.SensorTop.GetMinTriggerDistance()
            || newSensorTopMaxTriggerCm         != stair.SensorTop.GetMaxTriggerDistance()
            || newSensorBottomTriggerPin        != stair.SensorBottom.GetTriggerPin()
            || newSensorBottomEchoPin           != stair.SensorBottom.GetEchoPin()
            || newSensorBottomMinTriggerCm      != stair.SensorBottom.GetMinTriggerDistance()
            || newSensorBottomMaxTriggerCm      != stair.SensorBottom.GetMaxTriggerDistance()
            || newTimeBetweenEachStepMs         != stair.GetTimeBetweenEachStepMs()
            || newTimeThatEachStepStaysOnMin    != stair.GetTimeThatEachStepStaysOnMin()
            || newTransitionTimeMs              != stair.GetTransitionTimeMs()
            || newLedsPerStep                   != stair.GetLedsPerStep()
        ) {            
            // Sets the new values to the current values
            enable(newEnabled);
            stair.SensorTop.SetTriggerPin(ReAllocatePin(stair.SensorTop.GetTriggerPin(), newSensorTopTriggerPin));
            stair.SensorTop.SetEchoPin(ReAllocatePin(stair.SensorTop.GetEchoPin(), newSensorTopEchoPin));
            stair.SensorTop.SetMinTriggerDistance(newSensorTopMinTriggerCm);
            stair.SensorTop.SetMaxTriggerDistance(newSensorTopMaxTriggerCm);
            stair.SensorBottom.SetTriggerPin(ReAllocatePin(stair.SensorBottom.GetTriggerPin(), newSensorBottomTriggerPin));
            stair.SensorBottom.SetEchoPin(ReAllocatePin(stair.SensorBottom.GetEchoPin(), newSensorBottomEchoPin));
            stair.SensorBottom.SetMinTriggerDistance(newSensorBottomMinTriggerCm);
            stair.SensorBottom.SetMaxTriggerDistance(newSensorBottomMaxTriggerCm);
            stair.SetTimeBetweenEachStepMs(newTimeBetweenEachStepMs);
            stair.SetTimeThatEachStepStaysOnMin(newTimeThatEachStepStaysOnMin);
            stair.SetTransitionTimeMs(newTransitionTimeMs);
            stair.SetLedsPerStep(newLedsPerStep);
            
            // Do the setup again afther the values are changed
            setup();
        }
    }

    // Use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    return !top[FPSTR(_ledsPerStep)].isNull();
}

void AniStairs::appendConfigData() {
    //debugPrintLn("void AniStairs::appendConfigData() : 01");
    oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":LedsPerStep")); oappend(SET_F("',1,'<i class=\"warn\"><br>Specify the number of LEDs per for each step. Example 10;10;10 for 3 steps with 10 LEDs.</i>');"));
}









// Temp develop debug voids
#define debugPrintDelay 0
void AniStairs::debugPrint(String val) {
    Serial.print(val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void AniStairs::debugPrint(int val) {
    debugPrint(String(val));
}

void AniStairs::debugPrintLn(String val) {
    debugPrint(millis());
    Serial.println(" : " + val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void AniStairs::debugPrintLn(int val) {
    debugPrintLn(String(val));
}