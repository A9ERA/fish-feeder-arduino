#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

#include <Arduino.h>

#define SOIL_PIN A2  // AO ต่อกับ A2 ของ Arduino Mega

void initSoil();
void readSoil();

#endif