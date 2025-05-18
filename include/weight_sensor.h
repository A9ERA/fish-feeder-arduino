#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include <Arduino.h>
#include <HX711.h>
#include <EEPROM.h>

const int LOADCELL_DOUT_PIN = 20;
const int LOADCELL_SCK_PIN = 21;
const int EEPROM_SCALE_ADDR = 0;
const int EEPROM_OFFSET_ADDR = EEPROM_SCALE_ADDR + sizeof(float);

extern HX711 scale;
extern float scaleFactor;
extern long offset;

void initWeight();
void readWeight();

#endif