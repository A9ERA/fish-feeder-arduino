#include <Arduino.h>
#include "relay_control.h"

// Global variables for relay state
static bool relayUsedLoad = false;
static bool freezeBattery = false;
static unsigned long relayStopTime = 0;

void initRelayControl() {
    pinMode(RELAY_IN1, OUTPUT);
    pinMode(RELAY_IN2, OUTPUT);
    digitalWrite(RELAY_IN1, HIGH); // Turn off initially (relay is active LOW)
    digitalWrite(RELAY_IN2, HIGH); // Turn off initially (relay is active LOW)
    relayUsedLoad = false;
    freezeBattery = false;
    Serial.println("[RELAY] Relay control initialized");
}

void relayLedOn() {
    digitalWrite(RELAY_IN1, LOW);  // Turn ON pond LED light (relay active LOW)
    freezeBattery = true;
    Serial.println("[RELAY] LED light ON (pond lighting)");
}

void relayLedOff() {
    digitalWrite(RELAY_IN1, HIGH); // Turn OFF pond LED light
    // Check if fan is also off to update states
    if (digitalRead(RELAY_IN2) == HIGH) {
        relayUsedLoad = false;
        freezeBattery = false;
    }
    relayStopTime = millis();
    Serial.println("[RELAY] LED light OFF");
}

void relayFanOn() {
    digitalWrite(RELAY_IN2, LOW);  // Turn ON control box fan (relay active LOW)
    relayUsedLoad = true;
    relayStopTime = millis();
    freezeBattery = true;
    Serial.println("[RELAY] Control box fan ON");
}

void relayFanOff() {
    digitalWrite(RELAY_IN2, HIGH); // Turn OFF control box fan
    // Check if LED is also off to update states
    if (digitalRead(RELAY_IN1) == HIGH) {
        relayUsedLoad = false;
        freezeBattery = false;
    }
    relayStopTime = millis();
    Serial.println("[RELAY] Control box fan OFF");
}

void relayAllOff() {
    digitalWrite(RELAY_IN1, HIGH); // Turn OFF both relays
    digitalWrite(RELAY_IN2, HIGH);
    relayUsedLoad = false;
    relayStopTime = millis();
    freezeBattery = false;
    Serial.println("[RELAY] All relays OFF");
} 