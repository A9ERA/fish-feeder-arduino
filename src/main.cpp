#include <Arduino.h>
#include "relay_sensor.h"
#include "sensor_service.h"

// Create relay sensor instance
RelaySensor relaySensor;

void setup() {
    // Initialize Serial communication
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for Serial port to connect
    }
    
    // Initialize all sensors
    initAllSensors();
    
    // Initialize relay sensor
    relaySensor.begin();
}

void loop() {
    // Update relay sensor (process any incoming commands)
    relaySensor.update();
    
    // Process other sensor controls
    controlSensor();
    
    // Read and print sensor data every 5 seconds
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead >= 5000) {
        readAndPrintAllSensors();
        lastSensorRead = millis();
    }
    
    delay(10); // Small delay to prevent CPU hogging
}
