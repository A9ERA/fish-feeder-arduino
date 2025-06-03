#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <Arduino.h>

enum DeviceType {
    DEVICE_UNKNOWN,
    DEVICE_BLOWER,
    DEVICE_ACTUATORMOTOR,
    DEVICE_RELAY
};

void initAllSensors();
void controlSensor();
void readAndPrintAllSensors();

#endif 