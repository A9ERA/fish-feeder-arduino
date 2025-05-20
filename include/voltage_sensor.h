#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

#define VOLTAGE_PIN A1  // Analog pin for voltage sensor

void initVoltageSensor();
void readVoltageSensor();

#endif 