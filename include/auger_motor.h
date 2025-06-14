#ifndef AUGER_MOTOR_H
#define AUGER_MOTOR_H

#include <Arduino.h>

// Pin definitions for Auger Motor
#define AUG_ENA 8    // PWM pin for speed control
#define AUG_IN1 9    // Direction pin for forward
#define AUG_IN2 10   // Direction pin for backward

// Function declarations
void initAugerMotor();
void augerMotorForward();
void augerMotorBackward();
void augerMotorStop();
void augerMotorSpeedTest();

#endif // AUGER_MOTOR_H 