#include "../usermods/AniStairs/AniStairs_Stair.h"

void Stair::ClearSteps() {
    //debugPrintLn("void Stair::ClearSteps() : 01");
    steps.clear();
    totalLedsInStair = 0;
}

String Stair::SetupSteps(String _ledsPerStep) {
    //debugPrintLn("String Stair::SetupSteps(String _ledsPerStep) : 01");
    // Clear the steps of the stair.
    ClearSteps();

    // Check if _ledsPerStep is not NUL or Empty
    if (_ledsPerStep == NULL || _ledsPerStep.isEmpty()) return _ledsPerStep;

    // Check if the delimiter char is in _ledsPerStep 
    const char delimiter[] = ";";
    int lastIndexOfDelimiter = -1;
    int indexOfDelimiter = _ledsPerStep.indexOf(delimiter, lastIndexOfDelimiter + 1);
    
    // Stop direct if indexOfDelimiter is below 0.
    // Then the delimiter char is not found in _ledsPerStep.
    if (indexOfDelimiter < 0) return _ledsPerStep;

    // Add delimiter character to the end of _ledsPerStep if 
    // the last character is not equal to the delimiter.
    if (_ledsPerStep.endsWith(delimiter) == false) _ledsPerStep.concat(delimiter);
    
    // Process the values seperated by the delimiter in _ledsPerStep
    String strValueNrLeds;
    String newLedsPerStep = "";
    int intValueNrLeds = -1;
    while (indexOfDelimiter >= 0) {
        // Get the value from _ledsPerStep
        strValueNrLeds = _ledsPerStep.substring(lastIndexOfDelimiter + 1, indexOfDelimiter);

        // Trim the strValueNrLeds value and store it in intValueNrLeds converted as inter.
        strValueNrLeds.trim();
        intValueNrLeds = strValueNrLeds.toInt();

        // Check if the intValueNrLeds is greater then 0
        if (intValueNrLeds > 0) {                    
            // Check if the total number of leds in the stair less than or 
            // equal to the total leds of the strip
            if ((totalLedsInStair + intValueNrLeds) <= strip.getLengthTotal()) {
                totalLedsInStair = totalLedsInStair + intValueNrLeds;
                
                // Build the new step
                step newStep;
                newStep.number = steps.size() + 1;
                newStep.leds = intValueNrLeds;

                // Add the intValue to intVector
                steps.push_back(newStep);
                
                // Add the intValue to the newLedsPerStep string and add a delimiter value afther it.
                newLedsPerStep.concat(String(intValueNrLeds));
                newLedsPerStep.concat(delimiter);

            }
        }

        // Update lastIndexOfDelimiter and find the new indexOfDelimiter
        lastIndexOfDelimiter = indexOfDelimiter;
        indexOfDelimiter = _ledsPerStep.indexOf(delimiter, lastIndexOfDelimiter + 1);
    }
    strValueNrLeds.clear();

    // Check if ledsPerStep is diferent from newLedsPerStep
    if (_ledsPerStep != newLedsPerStep) {
        // Update _ledsPerStep with the clean newLedsPerStep
        _ledsPerStep = newLedsPerStep;
        
    }
    newLedsPerStep.clear();

    maxStepId = (steps.size() - 1);

    // Return the _ledsPerStep.
    return _ledsPerStep;
}

void Stair::SyncStairStepsWithStripSegments() {
    //debugPrintLn("void Stair::SyncStairStepsWithStripSegments() : 01");
    // Check if there are more segments in the strip then steps in the stair.
    if (strip._segments.size() > GetStepsSize()) {
        // Remove all inactive segments from the strip.
        strip.purgeSegments(true);

        // As long as there are more segments in the strip than steps in the stair.
        while ((strip._segments.size() > GetStepsSize()) && GetStepsSize() > 0) {
            // Set the stop led to -1 of the last active segment in the strip.
            strip.getSegment(strip.getLastActiveSegmentId()).stop = -1;
            
            // Remove all inactive segments from the strip.
            strip.purgeSegments(true);
        }
    }

    // Check if there are less segments in the strip than steps in the stair.
    if (GetStepsSize() > strip._segments.size()) {
        // As long as there are fewer segments in the strip than steps in the stair.
        while (GetStepsSize() > strip._segments.size()) {
            // Add a segment to the strip.
            strip.appendSegment(Segment(0, 1));
        }
    }

    // Sync the start stop leds of each segment in 
    // the strip with the step of the stair.
    int ledsInSegments = 0;
    for(int i = 0; i < GetStepsSize(); i++) {
        //steps[i].segmentId = i;
        Segment &seg = strip.getSegment(i);
        seg.start = ledsInSegments;
        seg.stop = (ledsInSegments + steps[i].leds);
        seg.setOption(SEG_OPTION_ON, false);
        seg.refreshLightCapabilities();
        ledsInSegments += steps[i].leds;
    }
}

byte Stair::GetStepsSize() { 
    //debugPrintLn("byte Stair::GetStepsSize() : 01");
    return steps.size(); 
}

String Stair::GetLedsPerStep() {
    //debugPrintLn("String Stair::GetLedsPerStep() : 01");
    return ledsPerStep;
}

void Stair::SetLedsPerStep(String val) {
    //debugPrintLn("void Stair::SetLedsPerStep(String val) : 01");
    ledsPerStep = SetupSteps(val);
}

bool isSettingUp = true;
void Stair::Setup() {
    //debugPrintLn("void Stair::Setup() : 01");
    isSettingUp = true;
    ledsPerStep = SetupSteps(ledsPerStep);
    SyncStairStepsWithStripSegments();
    isSettingUp = false;
}

void Stair::Loop() {
    // Stop directly if the stair is setting up.
    if (isSettingUp == true) return;
    CheckSensors();  
    UpdateAnimation();
}

int Stair::GetTimeBetweenEachStepMs() {
    return timeBetweenEachStepMs;
}

const int minTimeBetweenEachStepMs = 0;
const int maxTimeBetweenEachStepMs = 10000;
void Stair::SetTimeBetweenEachStepMs(int miliseconds) {
    if (miliseconds < minTimeBetweenEachStepMs) miliseconds = minTimeBetweenEachStepMs;
    if (miliseconds > maxTimeBetweenEachStepMs) miliseconds = maxTimeBetweenEachStepMs;
    timeBetweenEachStepMs = miliseconds;
}

int Stair::GetTimeThatEachStepStaysOnMs() {
    return timeThatEachStepStaysOnMs;
}

const int minTimeThatEachStepStaysOnMs = 0;
const int maxTimeThatEachStepStaysOnMs = 1800000;
void Stair::SetTimeThatEachStepStaysOnMs(int miliseconds) {
    if (miliseconds < minTimeThatEachStepStaysOnMs) miliseconds = minTimeThatEachStepStaysOnMs;
    if (miliseconds > maxTimeThatEachStepStaysOnMs) miliseconds = maxTimeThatEachStepStaysOnMs;
    timeThatEachStepStaysOnMs = miliseconds;

    String str = "timeThatEachStepStaysOnMs = ";
    str += timeThatEachStepStaysOnMs;
    debugPrintLn(str);
}

const float OneMinuteIsHowManyMicroseconds = 60000;
float Stair::GetTimeThatEachStepStaysOnMin() {
    return ("%.3f", static_cast<float>(GetTimeThatEachStepStaysOnMs()) / OneMinuteIsHowManyMicroseconds);    
}

void Stair::SetTimeThatEachStepStaysOnMin(float minutes) {
    SetTimeThatEachStepStaysOnMs(round(minutes * OneMinuteIsHowManyMicroseconds));
}

int Stair::GetTransitionTimeMs() {
    return transitionTimeMs;
}

const int minTransitionTimeMs = 0;
const int maxTransitionTimeMs = 10000;
void Stair::SetTransitionTimeMs(int miliseconds) {
    if (miliseconds < minTransitionTimeMs) miliseconds = minTransitionTimeMs;
    if (miliseconds > maxTransitionTimeMs) miliseconds = maxTransitionTimeMs;
    transitionTimeMs = miliseconds;
}






void Stair::CheckSensors() {
    // Check if a sensor is trigered
    if (SensorTop.IsObjectInTriggerRange() == false && SensorBottom.IsObjectInTriggerRange() == true) {
        // The bottom sensor is triggered! So start the animation from bottom to top!
        //debugPrintLn("void Stair::CheckSensors() : 01 > Bottom sensor triggered!");        
        for(int i = 0; i <= maxStepId; i++) {
            if (steps[i].onAfter == -1){
                steps[i].onAfter = (GetTimeBetweenEachStepMs() * i);
                steps[i].offAfter = -1;
            }
        }
        
    } else if (SensorTop.IsObjectInTriggerRange() == true && SensorBottom.IsObjectInTriggerRange() == false) {
        // The top sensor is triggered! So start the animation from bottom to top!
        //debugPrintLn("void Stair::CheckSensors() : 01 > Top sensor triggered!");
        int j = 0;
        for(int i = maxStepId; i >= 0; i--) {
            if (steps[i].onAfter == -1){
                steps[i].onAfter = (GetTimeBetweenEachStepMs() * j);
                steps[i].offAfter = -1;
            }
            j++;
        }
    }     
}

unsigned long lastUpdateAnimationMillis = -1;
void Stair::UpdateAnimation() {
    
    if (lastUpdateAnimationMillis == -1) lastUpdateAnimationMillis = millis();
    bool doCount = false;
    if ((millis() - lastUpdateAnimationMillis) >= 2) {
        lastUpdateAnimationMillis = millis();
        doCount = true;
    }
    
    bool updateDone = false;
    for(int i = 0; i <= maxStepId; i++) {        
        step &stp = steps[i];
        
        if (stp.onAfter == 0 || stp.offAfter == 0) {
            
            Segment &seg = strip.getSegment(i);
            
            if (stp.onAfter == 0) {
                
                if (seg.isSelected() == false) seg.selected = true;
                
                seg.startTransition(GetTransitionTimeMs());
                seg.setOption(SEG_OPTION_ON, true);
                seg.setMode(FX_MODE_GLITTER, true);
                
                stp.offAfter = GetTimeThatEachStepStaysOnMs(); //GetTimeThatEachStepStaysOnMs() + GetTimeBetweenEachStepMs();
                updateDone = true;
            }

            if (stp.offAfter == 0) {
                seg.startTransition(GetTransitionTimeMs());
                seg.setOption(SEG_OPTION_ON, false);
                stp.offAfter = -1;
                updateDone = true;
            }
        }
        
        
        
        
        
        
        if (doCount) {            
            if (stp.onAfter >= 0) stp.onAfter--;
            if (stp.offAfter >= 0) stp.offAfter--;            
        }
    }
    
    if (updateDone) {
        strip.trigger();  // force strip refresh
        //stateChanged = true;  // inform external devices/UI of change
        //colorUpdated(CALL_MODE_DIRECT_CHANGE);
    }
}









// Temp develop debug voids
#define debugPrintDelay 0
void Stair::debugPrint(String val) {
    Serial.print(val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void Stair::debugPrint(int val) {
    debugPrint(String(val));
}

void Stair::debugPrintLn(String val) {
    debugPrint(millis());
    Serial.println(" : " + val); 
    if (debugPrintDelay > 0) delay(debugPrintDelay); 
}

void Stair::debugPrintLn(int val) {
    debugPrintLn(String(val));
}