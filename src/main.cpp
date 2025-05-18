#include <Arduino.h>
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"



void setup() {
  Serial.begin(115200);
  initDHT();
  initSoil();
  initWaterTemp();
  initWeight();
}

void loop() {
  readDHT();
  readSoil();
  readWaterTemp();
  readWeight();
  delay(1000);
}


