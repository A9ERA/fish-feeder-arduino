#include <Arduino.h>
#include "sensor_service.h"

void setup() {
  Serial.begin(9600);
  initAllSensors();
}

void loop() {
  // readAndPrintAllSensors();
  controlSensor();
  // delay(1000);
}
