#include <Arduino.h>
#include <ArduinoJson.h>
#include "actuator_motor.h"

// Global variables for actuator state
static bool actuatorUsedLoad = false;
static bool freezeBattery = false;
static unsigned long actuatorStopTime = 0;
static String currentState = "stopped";

void initActuatorMotor() {
    pinMode(ACT_ENA, OUTPUT);
    pinMode(ACT_IN1, OUTPUT);
    pinMode(ACT_IN2, OUTPUT);
    analogWrite(ACT_ENA, 0);  // Stop initially
    actuatorUsedLoad = false;
    freezeBattery = false;
    currentState = "stopped";
    Serial.println("[ACTUATOR] Actuator motor initialized");
}

void actuatorMotorUp() {
    digitalWrite(ACT_IN1, HIGH);
    digitalWrite(ACT_IN2, LOW);
    analogWrite(ACT_ENA, 255);  // Extend with maximum speed
    actuatorUsedLoad = true;
    freezeBattery = true;
    actuatorStopTime = millis();
    currentState = "extending";
    Serial.println("[ACTUATOR] Actuator extending");
}

void actuatorMotorDown() {
    digitalWrite(ACT_IN1, LOW);
    digitalWrite(ACT_IN2, HIGH);
    analogWrite(ACT_ENA, 255);  // Retract with maximum speed
    actuatorUsedLoad = true;
    freezeBattery = true;
    actuatorStopTime = millis();
    currentState = "retracting";
    Serial.println("[ACTUATOR] Actuator retracting");
}

void actuatorMotorStop() {
    analogWrite(ACT_ENA, 0);  // Stop PWM output
    actuatorUsedLoad = false;
    freezeBattery = false;
    actuatorStopTime = millis();
    currentState = "stopped";
    Serial.println("[ACTUATOR] Actuator stopped");
}
