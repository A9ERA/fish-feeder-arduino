#include <Arduino.h>
#include "auger_motor.h"

// Global variables for auger state
static bool augerUsedLoad = false;
static bool freezeBattery = false;
static unsigned long augerStopTime = 0;
static int augerSpeed = 255;

void initAugerMotor() {
    pinMode(AUG_ENA, OUTPUT);
    pinMode(AUG_IN1, OUTPUT);
    pinMode(AUG_IN2, OUTPUT);
    analogWrite(AUG_ENA, 0);  // Stop initially
    augerUsedLoad = false;
    freezeBattery = false;
    Serial.println("[AUGER] Auger motor initialized");
}

void augerMotorForward() {
    digitalWrite(AUG_IN1, HIGH);
    digitalWrite(AUG_IN2, LOW);
    analogWrite(AUG_ENA, augerSpeed);
    augerUsedLoad = true;
    freezeBattery = true;
    augerStopTime = millis();
    Serial.println("[AUGER] Auger running forward");
}

void augerMotorBackward() {
    digitalWrite(AUG_IN1, LOW);
    digitalWrite(AUG_IN2, HIGH);
    analogWrite(AUG_ENA, augerSpeed);
    augerUsedLoad = true;
    freezeBattery = true;
    augerStopTime = millis();
    Serial.println("[AUGER] Auger running backward");
}

void augerMotorStop() {
    analogWrite(AUG_ENA, 0);
    augerUsedLoad = false;
    freezeBattery = false;
    augerStopTime = millis();
    Serial.println("[AUGER] Auger stopped");
}

void augerMotorSpeedTest() {
    Serial.println("[AUGER] Starting speed test");
    int speeds[] = {64, 128, 192, 255};
    
    digitalWrite(AUG_IN1, HIGH);
    digitalWrite(AUG_IN2, LOW);
    
    for (int i = 0; i < 4; i++) {
        analogWrite(AUG_ENA, speeds[i]);
        Serial.print("[AUGER] Speed: ");
        Serial.print((speeds[i] * 100) / 255);
        Serial.println("%");
        delay(1000);
    }
    
    analogWrite(AUG_ENA, 0);
    augerUsedLoad = false;
    freezeBattery = false;
    augerStopTime = millis();
    Serial.println("[AUGER] Speed test completed");
}

void augerMotorSetSpeed(int speed) {
    augerSpeed = speed;
    Serial.println("[AUGER] Auger speed set to " + String(speed));
}