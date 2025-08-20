#ifndef FEEDER_MOTOR_H
#define FEEDER_MOTOR_H

#include <Arduino.h>

// Pin definitions for Feeder Motor
// Dual-PWM pins similar to blower control
#define FM_RPWM 8  // PWM pin for CW direction
#define FM_LPWM 9  // PWM pin for CCW direction

void initFeederMotor();
void feederMotorOpen();
void feederMotorClose();
void feederMotorStop();

#endif // FEEDER_MOTOR_H

