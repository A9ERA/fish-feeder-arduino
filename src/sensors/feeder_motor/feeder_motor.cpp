#include "../../../include/feeder_motor.h"

// Fixed full speed for feeder motor driving
static const int FEEDER_MOTOR_SPEED = 255;

// Safe reverse delay to protect driver
static const uint16_t FEEDER_MOTOR_REVERSE_DELAY_MS = 150;

// Track last direction
enum FeederMotorDir { FM_STOP_DIR = 0, FM_CW_DIR, FM_CCW_DIR };
static FeederMotorDir feederMotorLastDir = FM_STOP_DIR;

static void feederMotorStopInternal() {
    analogWrite(FM_RPWM, 0);
    analogWrite(FM_LPWM, 0);
    feederMotorLastDir = FM_STOP_DIR;
}

static void feederMotorCW() {
    analogWrite(FM_RPWM, FEEDER_MOTOR_SPEED);
    analogWrite(FM_LPWM, 0);
    feederMotorLastDir = FM_CW_DIR;
}

static void feederMotorCCW() {
    analogWrite(FM_RPWM, 0);
    analogWrite(FM_LPWM, FEEDER_MOTOR_SPEED);
    feederMotorLastDir = FM_CCW_DIR;
}

static void feederMotorSafeSet(FeederMotorDir target) {
    if ((feederMotorLastDir == FM_CW_DIR && target == FM_CCW_DIR) ||
        (feederMotorLastDir == FM_CCW_DIR && target == FM_CW_DIR)) {
        feederMotorStopInternal();
        delay(FEEDER_MOTOR_REVERSE_DELAY_MS);
    }
    switch (target) {
        case FM_CW_DIR: feederMotorCW(); break;
        case FM_CCW_DIR: feederMotorCCW(); break;
        default: feederMotorStopInternal(); break;
    }
}

void initFeederMotor() {
    pinMode(FM_RPWM, OUTPUT);
    pinMode(FM_LPWM, OUTPUT);
    feederMotorStopInternal();
}

// Open: rotate CW at full speed
void feederMotorOpen() {
    feederMotorSafeSet(FM_CW_DIR);
    delay(300);
    feederMotorStop();
}

// Close: rotate CCW at full speed
void feederMotorClose() {
    feederMotorSafeSet(FM_CCW_DIR);
    delay(300);
    feederMotorStop();
}

// Public stop function (exposed via header)
void feederMotorStop() {
    feederMotorStopInternal();
}


