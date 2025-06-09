#include "acs712_sensor.h"

ACS712Sensor::ACS712Sensor(uint8_t pin, float sensitivity) 
    : pin(pin), sensitivity(sensitivity), offset(2.5) {
}

void ACS712Sensor::begin() {
    pinMode(pin, INPUT);
    calibrateOffset();
    delay(50);
    Serial.println("☀️ ACS712 solar current sensor initialized");
}

bool ACS712Sensor::readCurrent(float& current) {
    int rawValue = analogRead(pin);
    current = convertToCurrent(rawValue);
    return isValidReading(current);
}

float ACS712Sensor::convertToCurrent(int rawValue) {
    // Convert ADC reading to voltage
    float voltage = (rawValue * 5.0) / 1023.0;
    
    // Calculate current based on ACS712 specifications
    // Current = (Voltage - Offset) / (Sensitivity/1000)
    float current = (voltage - offset) / (sensitivity / 1000.0);
    
    return abs(current); // Return absolute value
}

void ACS712Sensor::calibrateOffset() {
    long sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += analogRead(pin);
        delay(10);
    }
    
    // Calculate average voltage when no current flows
    offset = (sum / 100.0) * 5.0 / 1023.0;
    
    Serial.print("🎯 ACS712 offset calibrated: ");
    Serial.print(offset);
    Serial.println("V");
}

bool ACS712Sensor::isValidReading(float value) {
    return value >= 0.0 && value <= 30.0; // Reasonable range for solar current
}

void ACS712Sensor::printStatus() {
    float current;
    bool valid = readCurrent(current);
    
    Serial.print("☀️ Solar Current: ");
    Serial.print(valid ? current : -999);
    Serial.print("A [");
    Serial.print(valid ? "OK" : "ERROR");
    Serial.println("]");
}

// ===== 🎛️ GLOBAL INSTANCE =====
ACS712Sensor currentSensor(CURRENT_PIN, 185.0); // 5A module 