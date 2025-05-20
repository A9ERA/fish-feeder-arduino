#include <Arduino.h>
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "acs712_sensor.h"
#include "voltage_sensor.h"



void setup() {
  Serial.begin(115200);
  // initDHT();
  // initSoil();
  // initWaterTemp();
  // initWeight();
  // initACS712();
  initVoltageSensor();
}

void loop() {
  // readDHT();
  // readSoil();
  // readWaterTemp();
  // readWeight();
  // readACS712();
  readVoltageSensor();
  delay(1000);
}


