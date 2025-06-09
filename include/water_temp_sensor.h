#ifndef WATER_TEMP_SENSOR_H
#define WATER_TEMP_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_data.h"

class WaterTempSensor {
private:
    OneWire* oneWire;
    DallasTemperature* sensors;
    uint8_t pin;

public:
    WaterTempSensor(uint8_t pin);
    ~WaterTempSensor();
    
    void begin();
    bool readTemperature(float& temperature);
    bool isValidReading(float value);
    void printStatus();
};

// ===== 🎛️ GLOBAL WATER TEMP INSTANCE =====
extern WaterTempSensor waterTempSensor;

#endif // WATER_TEMP_SENSOR_H 