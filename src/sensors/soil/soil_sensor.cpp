#include "soil_sensor.h"

SoilSensor::SoilSensor(uint8_t pin) : pin(pin) {
}

void SoilSensor::begin() {
    pinMode(pin, INPUT);
    delay(50);
    Serial.println("🌱 Soil moisture sensor initialized");
}

bool SoilSensor::readMoisture(float& moisture) {
    int rawValue = analogRead(pin);
    moisture = convertToPercentage(rawValue);
    return isValidReading(moisture);
}

float SoilSensor::convertToPercentage(int rawValue) {
    // Convert 0-1023 to 0-100% (inverted: higher analog value = lower moisture)
    return map(rawValue, 1023, 0, 0, 100);
}

bool SoilSensor::isValidReading(float value) {
    return value >= 0.0 && value <= 100.0;
}

void SoilSensor::printStatus() {
    float moisture;
    bool valid = readMoisture(moisture);
    
    Serial.print("🌱 Soil Moisture: ");
    Serial.print(valid ? moisture : -999);
    Serial.print("% [");
    Serial.print(valid ? "OK" : "ERROR");
    Serial.println("]");
}

// ===== 🎛️ GLOBAL INSTANCE =====
SoilSensor soilSensor(SOIL_PIN); 