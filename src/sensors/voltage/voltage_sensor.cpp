#include "voltage_sensor.h"

VoltageSensor::VoltageSensor(uint8_t pin, float multiplier) 
    : pin(pin), voltageMultiplier(multiplier) {
}

void VoltageSensor::begin() {
    pinMode(pin, INPUT);
    delay(50);
    Serial.println("🔋 Battery voltage sensor initialized");
}

bool VoltageSensor::readVoltage(float& voltage) {
    int rawValue = analogRead(pin);
    voltage = convertToVoltage(rawValue);
    return isValidReading(voltage);
}

float VoltageSensor::convertToVoltage(int rawValue) {
    // Convert 0-1023 to 0-5V, then apply multiplier for voltage divider
    float voltage = (rawValue * 5.0 / 1023.0) * voltageMultiplier;
    return voltage;
}

bool VoltageSensor::isValidReading(float value) {
    return value >= 0.0 && value <= 25.0; // Reasonable range for battery systems
}

void VoltageSensor::printStatus() {
    float voltage;
    bool valid = readVoltage(voltage);
    
    Serial.print("🔋 Battery Voltage: ");
    Serial.print(valid ? voltage : -999);
    Serial.print("V [");
    Serial.print(valid ? "OK" : "ERROR");
    Serial.println("]");
}

// ===== 🎛️ GLOBAL INSTANCE =====
VoltageSensor voltageSensor(VOLTAGE_PIN, 3.0); // Assuming 3:1 voltage divider 