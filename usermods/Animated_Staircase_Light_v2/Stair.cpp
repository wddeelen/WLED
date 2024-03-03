#include "Stair.h"

Stair::Stair() {
     // none 
}

Stair::Stair(
    int8_t _sensorTopTriggerPin, int8_t _sensorTopEchoPin,          // Top sensor pins
    int8_t _sensorBottomTriggerPin, int8_t _sensorBottomEchoPin,    // Bottom sensor pins
    String _ledsPerStep                                             // The leds per steps string from the config
) { 
    SensorTop = AJSRO4M(_sensorTopTriggerPin, _sensorTopEchoPin);
    SensorBottom = AJSRO4M(_sensorBottomTriggerPin, _sensorBottomEchoPin);
    SetupSteps(_ledsPerStep);
}

void Stair::ClearSteps() {
    steps.clear();
    totalLedsInStair = 0;
}

byte Stair::GetStepsSize() { 
    return steps.size(); 
}

String Stair::SetupSteps(String _ledsPerStep) {
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
    step newStep;
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

    // Return the _ledsPerStep.
    return _ledsPerStep;
}

void Stair::SyncStairStepsWithStripSegments() {
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
        Segment &segment = strip.getSegment(i);
        segment.start = ledsInSegments;
        segment.stop = (ledsInSegments + steps[i].leds);
        segment.refreshLightCapabilities();
        ledsInSegments += steps[i].leds;
    }
}