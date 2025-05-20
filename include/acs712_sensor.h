#ifndef ACS712_SENSOR_H
#define ACS712_SENSOR_H

#include <Arduino.h>

#define ACS712_PIN A0  // Analog pin for ACS712 sensor

void initACS712();
void readACS712();
double readCurrentOnce();  // Read a single current measurement
double readAverageCurrent();  // Read and average multiple current measurements

#endif 