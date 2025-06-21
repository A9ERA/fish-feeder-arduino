#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions
#define SOLAR_VOLTAGE_PIN A6
#define SOLAR_CURRENT_PIN A7
#define LOAD_VOLTAGE_PIN A1
#define LOAD_CURRENT_PIN A0

// Sensor name
#define POWER_MONITOR "POWER_MONITOR"

// Constants for calculations
#define V_REF 5.0
#define V_FACTOR 4.50
#define SENSITIVITY 0.066
#define ZERO_CURRENT_VOLTAGE 2.500

// Function declarations
void initPowerMonitor();
StaticJsonDocument<1024> readPowerMonitor();
float estimateBatteryPercentage(float voltage);
void readSensors(float& solarV, float& solarI, float& loadV, float& loadI);
bool isCharging(float solarV, float solarI);

#endif 