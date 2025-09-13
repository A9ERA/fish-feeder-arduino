#include <Arduino.h>
#include "sensor_service.h"
#include "feeder_service.h"

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);
  
  // Initialize all sensors and devices
  initAllSensors();
  
  // Initialize timer-based sensor service (runs in background)
  initSensorService();
  
  // Initialize feeder service (independent from sensor service)
  initFeederService();
  
  Serial.println("[INFO] - System ready. Sensor service running in background, Feeder service ready for commands.");
}

void loop() {
  // Handle control commands first for immediate responsiveness
  if (Serial.available()) {
    controlSensor();
  } else {
    // Update sensor service (non-blocking, time-sliced). Skips if commands are waiting
    updateSensorService();
  }
}
