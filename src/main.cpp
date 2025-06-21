#include <Arduino.h>
#include "sensor_service.h"

void setup() {
  Serial.begin(9600);
  
  // Initialize all sensors and devices
  initAllSensors();
  
  // Initialize timer-based sensor service (runs in background)
  initSensorService();
  
  Serial.println("[INFO] - System ready. Sensor service running in background.");
}

void loop() {
  // Update sensor service (non-blocking, timer-based)
  updateSensorService();
  
  // Handle control commands (non-blocking)
  controlSensor();
  
  // Short delay to prevent overwhelming the system
  delay(10); // Much shorter delay since we're not blocking on sensor reads
}
