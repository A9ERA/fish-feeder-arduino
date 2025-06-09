#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>
#include "sensor_data.h"

class VoltageSensor {
private:
    uint8_t pin;
    float voltageMultiplier;

public:
    VoltageSensor(uint8_t pin, float multiplier = 1.0);
    
    void begin();
    bool readVoltage(float& voltage);
    float convertToVoltage(int rawValue);
    bool isValidReading(float value);
    void printStatus();
};

// ===== 🎛️ GLOBAL VOLTAGE SENSOR INSTANCE =====
extern VoltageSensor voltageSensor;

#endif // VOLTAGE_SENSOR_H 