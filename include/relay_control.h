#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>

// Pin definitions for Relay Control
#define RELAY_IN1 50  // Relay channel 1 (pond LED light)
#define RELAY_IN2 52  // Relay channel 2 (control box fan)

// Function declarations
void initRelayControl();
void relayLedOn();
void relayLedOff();
void relayFanOn();
void relayFanOff();
void relayAllOff();

#endif // RELAY_CONTROL_H 