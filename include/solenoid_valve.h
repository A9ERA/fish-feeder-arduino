#ifndef SOLENOID_VALVE_H
#define SOLENOID_VALVE_H

#include <Arduino.h>

// Pin definitions for Solenoid Valve
#define SOL_ENA 11  // PWM pin for speed control
#define SOL_IN1 12  // Direction pin for open
#define SOL_IN2 13  // Direction pin for close

void initSolenoidValve();
void solenoidValveOpen();
void solenoidValveClose();

#endif // SOLENOID_VALVE_H 