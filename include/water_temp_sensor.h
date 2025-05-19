#ifndef WATER_TEMP_SENSOR_H
#define WATER_TEMP_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#define ONE_WIRE_BUS 44  // ขาที่ใช้กับ DS18B20

// Sensor name
#define WATER_TEMP_SENSOR "DS18B20_WATER_TEMP"

void initWaterTemp();
StaticJsonDocument<256> readWaterTemp();

#endif