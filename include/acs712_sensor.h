#ifndef ACS712_SENSOR_H
#define ACS712_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions
#define ACS712_PIN A2

// Sensor name
#define ACS712_SENSOR "CURRENT_SYSTEM"

// Function declarations
void initACS712();
StaticJsonDocument<256> readACS712();

#endif 