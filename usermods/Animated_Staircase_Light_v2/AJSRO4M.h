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