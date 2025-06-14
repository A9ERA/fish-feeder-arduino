#ifndef ACTUATOR_MOTOR_H
#define ACTUATOR_MOTOR_H

#include <Arduino.h>

// Pin definitions for Linear Actuator
#define ACT_ENA 11  // PWM pin for speed control
#define ACT_IN1 12  // Direction pin for extend
#define ACT_IN2 13  // Direction pin for retract

// Function declarations
void initActuatorMotor();
void actuatorMotorUp();
void actuatorMotorDown();
void actuatorMotorStop();

#endif // ACTUATOR_MOTOR_H 