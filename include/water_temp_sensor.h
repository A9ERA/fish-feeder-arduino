#ifndef WATER_TEMP_SENSOR_H
#define WATER_TEMP_SENSOR_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 44  // ขาที่ใช้กับ DS18B20

void initWaterTemp();
void readWaterTemp();

#endif