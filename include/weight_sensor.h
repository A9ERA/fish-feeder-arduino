#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include <Arduino.h>
#include <HX711.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

const int LOADCELL_DOUT_PIN = 28;
const int LOADCELL_SCK_PIN = 26;
const float FIXED_SCALE_FACTOR = 40101.f;

// EEPROM addresses
const int EEPROM_OFFSET_ADDR = 0;  // Address for storing offset value

// Sensor name
#define WEIGHT_SENSOR "HX711_FEEDER"

extern HX711 scale;

void initWeight();
StaticJsonDocument<256> readWeight();

// Weight calibration function
void calibrateWeight();

#endif