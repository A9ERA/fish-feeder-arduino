#ifndef ACS712_SENSOR_H
#define ACS712_SENSOR_H

#include <Arduino.h>
#include "sensor_data.h"

class ACS712Sensor {
private:
    uint8_t pin;
    float sensitivity; // mV/A
    float offset;      // Voltage offset at 0A

public:
    ACS712Sensor(uint8_t pin, float sensitivity = 185.0); // 5A module default
    
    void begin();
    bool readCurrent(float& current);
    float convertToCurrent(int rawValue);
    void calibrateOffset();
    bool isValidReading(float value);
    void printStatus();
};

// ===== 🎛️ GLOBAL ACS712 SENSOR INSTANCE =====
extern ACS712Sensor currentSensor;

#endif // ACS712_SENSOR_H 