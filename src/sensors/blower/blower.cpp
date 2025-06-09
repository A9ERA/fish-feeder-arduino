#include "blower.h"

Blower::Blower(uint8_t pin) : pin(pin), speed(0), isRunning(false) {
}

void Blower::begin() {
    pinMode(pin, OUTPUT);
    turnOff();
    Serial.println("💨 Blower motor initialized");
}

void Blower::turnOn(uint8_t newSpeed) {
    speed = newSpeed;
    analogWrite(pin, speed);
    isRunning = true;
    
    Serial.print("✅ Blower ON - Speed: ");
    Serial.print(speed * 100 / 255);
    Serial.println("%");
}

void Blower::turnOff() {
    speed = 0;
    analogWrite(pin, 0);
    isRunning = false;
    Serial.println("✅ Blower OFF");
}

void Blower::setSpeed(uint8_t newSpeed) {
    if (newSpeed == 0) {
        turnOff();
    } else {
        speed = newSpeed;
        analogWrite(pin, speed);
        isRunning = true;
        
        Serial.print("⚙️ Blower speed: ");
        Serial.print(speed * 100 / 255);
        Serial.println("%");
    }
}

void Blower::printStatus() {
    Serial.print("💨 Blower: ");
    if (isRunning) {
        Serial.print("ON (");
        Serial.print(speed * 100 / 255);
        Serial.println("%)");
    } else {
        Serial.println("OFF");
    }
}

// ===== 🎛️ GLOBAL INSTANCE =====
Blower blower(BLOWER_PIN); 