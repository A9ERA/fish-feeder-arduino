#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <Arduino.h>
#include "sensor_data.h"
#include "dht_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "soil_sensor.h"
#include "voltage_sensor.h"
#include "acs712_sensor.h"

class SensorService {
private:
    unsigned long lastSensorRead;
    unsigned long lastOutput;

public:
    SensorService();
    
    void begin();
    void readAllSensors();
    void outputSensorData();
    void outputSystemStatus();
    bool shouldReadSensors();
    bool shouldOutputData();
    void updateTimings();
    void printSensorErrors();
};

// ===== 🎛️ GLOBAL SENSOR SERVICE INSTANCE =====
extern SensorService sensorService;

#endif // SENSOR_SERVICE_H 