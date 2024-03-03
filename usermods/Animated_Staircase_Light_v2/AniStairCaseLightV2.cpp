#include "AniStairCaseLightV2.h"


void AniStairCaseLightV2::setup() {
    /*
    stair = Stair(
        9, 10,          // Top sensor pins
        8, 3,           // Bottom sensor pins
        ledsPerStepCfg  // The leds per steps string from the config
    );
    */
    
    // Validate pin numbers and set to -1 if they are not valid.
    if (topSensorPin < 0) topSensorPin = -1;
    if (bottomSensorPin < 0) bottomSensorPin = -1;

    // Check if the usermod is enabled
    if (enabled) {
        // Config says we are enabled

        // Allocate Top Sensor pin
        if (topSensorPin >= 0 && pinManager.allocatePin(topSensorPin, true, PinOwner::None)) { //PinOwner shoeld be UM_ANIMATED_STAIRCASE_V2
            // Set the pin mode for the Top sensor pin.
            pinMode(topSensorPin, INPUT_PULLUP);
            
            // Allocate Bottom Sensor pin
            if (bottomSensorPin >= 0 && pinManager.allocatePin(bottomSensorPin, true, PinOwner::None)) {//PinOwner shoeld be UM_ANIMATED_STAIRCASE_V2
                // Set the pin mode for the Bottom sensor pin.
                pinMode(bottomSensorPin, INPUT_PULLUP);
                //WELP ledsPerStepCfg = stair.SetupSteps(ledsPerStepCfg);

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
    
    
    minSegmentId = 0;
    maxSegmentId = strip._segments.size(); //WELP stair.GetStepsSize(); // ledsPerStepVector.size();

    bool currentCheckSensors = checkSensors();
    if (lastCheckSensors == false && currentCheckSensors == true) {
        //WELP if (strip._segments.size() != stair.GetStepsSize()) {
        //WELP     stair.SyncStairStepsWithStripSegments();
        //WELP }        
    }
    autoPowerOff(); //if (on) autoPowerOff();    
    updateSwipe();
}

void AniStairCaseLightV2::CheckTriggers() {
    // Get the last states
    TriggeredSensor LastFirstSensorTriggered = FirstSensorTriggered;
    TriggeredSensor LastSecondSensorTriggered = SecondSensorTriggered;
    
    // Get the distance from the top sensor
    int Distance =  0; //WELP stair.SensorTop.GetDistance();
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
    Distance = 0; //WELP stair.SensorBottom.GetDistance();
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

void AniStairCaseLightV2::debugPrint(String val) {
    Serial.print(val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void AniStairCaseLightV2::debugPrintLn(String val) {
    Serial.println(val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void AniStairCaseLightV2::debugPrint(int val) {
    debugPrint(String(val));
}

void AniStairCaseLightV2::debugPrintLn(int val) {
    debugPrintLn(String(val));
}

void AniStairCaseLightV2::DoTheAnimation() {

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
    top[FPSTR(_ledsPerStep)] = ledsPerStepCfg;
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
    String newLedsPerStep = ledsPerStepCfg;    
    
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
        ledsPerStepCfg = newLedsPerStep;
    } else {
        
        // Check if there are changes in parameters from the settings page
        if (
               newTopSensorPin != topSensorPin 
            || newBottomSensorPin != bottomSensorPin 
            || newTurnStepsOfAfterMs != turnStepsOfAfterMs 
            || newTimeBetweenEachStepMs != timeBetweenEachStepMs
            || newTransitionTimeMs != transitionTimeMs
            || newLedsPerStep != ledsPerStepCfg
        ) {
            // Deallocate top sensor pin
            pinManager.deallocatePin(topSensorPin, PinOwner::None);//PinOwner shoeld be UM_ANIMATED_STAIRCASE_V2
            topSensorPin = newTopSensorPin;

            // Deallocate bottom sensor pin
            pinManager.deallocatePin(bottomSensorPin, PinOwner::None);//PinOwner shoeld be UM_ANIMATED_STAIRCASE_V2
            bottomSensorPin = newBottomSensorPin;

            turnStepsOfAfterMs = newTurnStepsOfAfterMs;
            timeBetweenEachStepMs = newTimeBetweenEachStepMs;
            transitionTimeMs = newTransitionTimeMs;
            ledsPerStepCfg = newLedsPerStep;             
            
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