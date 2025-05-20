#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions
#define VOLTAGE_PIN A1

// Sensor name
#define VOLTAGE_SENSOR "VOLTAGE_SYSTEM"

// Function declarations
void initVoltageSensor();
StaticJsonDocument<256> readVoltageSensor();

#endif 