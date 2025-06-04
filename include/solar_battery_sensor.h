#ifndef SOLAR_BATTERY_SENSOR_H
#define SOLAR_BATTERY_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions for Solar and Battery monitoring
#define SOLAR_VOLTAGE_PIN A2    // Solar panel voltage monitoring
#define BATTERY_VOLTAGE_PIN A3  // Battery voltage monitoring
#define LOAD_CURRENT_PIN A4     // Load current monitoring (using ACS712)
#define SOLAR_CURRENT_PIN A5    // Solar current monitoring (using ACS712)

// Sensor names
#define SOLAR_SENSOR "SOLAR_MONITOR"
#define BATTERY_SENSOR "BATTERY_MONITOR" 
#define LOAD_SENSOR "LOAD_MONITOR"

// Voltage divider constants
#define VOLTAGE_DIVIDER_RATIO 11.0  // For voltage divider (100k + 10k)/10k
#define ADC_REFERENCE_VOLTAGE 5.0   // Arduino reference voltage
#define ADC_RESOLUTION 1024.0       // 10-bit ADC resolution

// ACS712 current sensor constants (for 20A module)
#define ACS712_SENSITIVITY 0.1      // 100mV/A for ACS712-20A
#define ACS712_ZERO_CURRENT 2.5     // Zero current voltage (Vcc/2)

// Battery monitoring constants
#define BATTERY_MIN_VOLTAGE 10.5    // Minimum safe battery voltage (12V system)
#define BATTERY_MAX_VOLTAGE 14.4    // Maximum battery voltage (fully charged)
#define BATTERY_LOW_THRESHOLD 11.8  // Low battery warning threshold

// Function declarations
void initSolarBattery();
StaticJsonDocument<256> readSolarStatus();
StaticJsonDocument<256> readBatteryStatus();
StaticJsonDocument<256> readLoadStatus();

// Utility functions
float readVoltage(int pin);
float readCurrent(int pin);
float calculateBatteryPercentage(float voltage);
String getBatteryStatus(float voltage);
float calculatePower(float voltage, float current);

#endif 