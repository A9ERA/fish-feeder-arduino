#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Pin definitions
#define DHTPIN1 48  // System DHT22
#define DHTPIN2 46  // Feeder DHT22
#define DHTTYPE DHT22

// Sensor names
#define DHT22_SYSTEM "DHT22_SYSTEM"
#define DHT22_FEEDER "DHT22_FEEDER"

// Function declarations
void initDHT();
StaticJsonDocument<256> readDHTSystem();
StaticJsonDocument<256> readDHTFeeder();

#endif // DHT_SENSOR_H 