#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

// Pin definitions
#define DHTPIN1 48
#define DHTPIN2 46
#define DHTTYPE DHT22

// Function declarations
void initDHT();
void readDHT();

#endif // DHT_SENSOR_H 