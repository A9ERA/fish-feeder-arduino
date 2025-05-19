#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions
#define SOIL_PIN A0

// Sensor name
#define SOIL_SENSOR "SOIL_MOISTURE"

// Function declarations
void initSoil();
StaticJsonDocument<256> readSoil();

#endif // SOIL_SENSOR_H