#include "../usermods/AniStairs/AniStairs_Stair_Sensor.h"

Sensor::Sensor(int8_t TriggerPinNumber, int8_t EchoPinNumber, int TriggerMinDistance, int TriggerMaxDistance) {
    SetTriggerPin(TriggerPinNumber);
    SetEchoPin(EchoPinNumber);
}

void Sensor::Ping() {
    // Direct stop if one of the pins are lower then 0
    if (pinNrTrigger < 0 || pinNrEcho < 0) return;

    // Update a ping only after 125 miliseconds
    if ((millis() - lastPing) > 125) {
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

        // Set the value of objectInTriggerRange
        objectInTriggerRange = (distance >= GetMinTriggerDistance() && distance <= GetMaxTriggerDistance());

        // Update the last ping time
        lastPing = millis();
    }    
}

int Sensor::GetDistance() {
    Ping();
    return distance;
}

uint8_t Sensor::GetTriggerPin() {
    return pinNrTrigger;
}

void Sensor::SetTriggerPin(uint8_t pinNumber) {
    if (pinNumber >= 0) pinMode(pinNumber, OUTPUT);
    pinNrTrigger = pinNumber;
}

uint8_t Sensor::GetEchoPin() {
    return pinNrEcho;
}

void Sensor::SetEchoPin(uint8_t pinNumber) {
    if (pinNumber >= 0) pinMode(pinNumber, INPUT);
    pinNrEcho = pinNumber;
}

int Sensor::GetMinTriggerDistance() {
    return minTriggerDistance;
}

void Sensor::SetMinTriggerDistance(int CmDistance) {
    if (CmDistance < 20) CmDistance = 20;
    if (CmDistance > 500) CmDistance = 500;
    if (CmDistance >= GetMaxTriggerDistance()) {
        SetMinTriggerDistance(CmDistance - 5);
        return;
    }
    minTriggerDistance = CmDistance;
}

int Sensor::GetMaxTriggerDistance() {
    return maxTriggerDistance;
}

void Sensor::SetMaxTriggerDistance(int CmDistance) {
    if (CmDistance < 20) CmDistance = 20;
    if (CmDistance > 500) CmDistance = 500;
    if (CmDistance < GetMinTriggerDistance()) {
        SetMaxTriggerDistance(CmDistance + 5);
        return;
    }
    maxTriggerDistance = CmDistance;
}

bool Sensor::IsObjectInTriggerRange() {
    Ping();
    return objectInTriggerRange;
}