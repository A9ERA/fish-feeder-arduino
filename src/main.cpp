#include <Arduino.h>
#include <ArduinoJson.h>
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "acs712_sensor.h"
#include "voltage_sensor.h"

void setup() {
  Serial.begin(9600);
  
  initDHT();
  initSoil();
  initWaterTemp();
  initWeight();
  initACS712();
  initVoltageSensor();
}

void loop() {
  // Read and send data from each sensor
  
  String jsonString;
  
  // DHT System 48
  StaticJsonDocument<256> dhtSystem = readDHTSystem();
  serializeJson(dhtSystem, jsonString);
  Serial.println("[SEND] - " + jsonString);

  // // DHT Feeder 46
  // StaticJsonDocument<256> dhtFeeder = readDHTFeeder();
  // jsonString = "";
  // serializeJson(dhtFeeder, jsonString);
  // Serial.println("[SEND] - " + jsonString);

  // // Soil Sensor
  // StaticJsonDocument<256> soil = readSoil();
  // jsonString = "";
  // serializeJson(soil, jsonString);
  // Serial.println(jsonString);

  // // Water Temperature
  // StaticJsonDocument<256> waterTemp = readWaterTemp();
  // jsonString = "";
  // serializeJson(waterTemp, jsonString);
  // Serial.println(jsonString);

  // // Weight Sensor
  // StaticJsonDocument<256> weight = readWeight();
  // jsonString = "";
  // serializeJson(weight, jsonString);
  // Serial.println(jsonString);

  // // Current Sensor
  // StaticJsonDocument<256> current = readACS712();
  // jsonString = "";
  // serializeJson(current, jsonString);
  // Serial.println(jsonString);

  // // Voltage Sensor
  // StaticJsonDocument<256> voltage = readVoltageSensor();
  // jsonString = "";
  // serializeJson(voltage, jsonString);
  // Serial.println(jsonString);

  // Wait before next reading
  delay(5000);  // 5 seconds delay
}


