#include <Arduino.h>
#include "services/relay_sensor.h"
#include "sensor_service.h"

// Create relay sensor instance
RelaySensor relaySensor;

// Timing variables for better performance
unsigned long lastSensorRead = 0;
unsigned long lastHeartbeat = 0;
const unsigned long SENSOR_INTERVAL = 5000;  // 5 seconds
const unsigned long HEARTBEAT_INTERVAL = 30000;  // 30 seconds

// System status
bool systemReady = false;
int errorCount = 0;

void setup() {
    // Initialize Serial communication with higher baud rate for better performance
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for Serial port to connect
    }
    
    Serial.println("=== Fish Feeder Arduino System Starting ===");
    Serial.println("🚀 Initializing sensors and actuators...");
    
    // Initialize all sensors with error handling
    try {
        initAllSensors();
        Serial.println("✅ All sensors initialized successfully");
    } catch (...) {
        Serial.println("❌ Error initializing sensors");
        errorCount++;
    }
    
    // Initialize relay sensor
    try {
        relaySensor.begin();
        Serial.println("✅ Relay sensor initialized successfully");
    } catch (...) {
        Serial.println("❌ Error initializing relay sensor");
        errorCount++;
    }
    
    systemReady = (errorCount == 0);
    
    if (systemReady) {
        Serial.println("🎉 System ready for operation!");
        Serial.println("📡 Waiting for commands from Raspberry Pi...");
    } else {
        Serial.println("⚠️  System started with errors. Some features may not work properly.");
    }
    
    // Send initial system status
    sendSystemStatus();
}

void loop() {
    // Update relay sensor (process any incoming commands)
    relaySensor.update();
    
    // Process other sensor controls
    controlSensor();
    
    // Read and send sensor data at regular intervals
    if (millis() - lastSensorRead >= SENSOR_INTERVAL) {
        if (systemReady) {
            readAndPrintAllSensors();
        } else {
            Serial.println("[ERROR] - System not ready, skipping sensor read");
        }
        lastSensorRead = millis();
    }
    
    // Send heartbeat signal
    if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    // Small delay to prevent CPU hogging
    delay(10);
}

void sendSystemStatus() {
    Serial.print("[STATUS] - {\"system_ready\":");
    Serial.print(systemReady ? "true" : "false");
    Serial.print(",\"error_count\":");
    Serial.print(errorCount);
    Serial.print(",\"uptime\":");
    Serial.print(millis());
    Serial.println("}");
}

void sendHeartbeat() {
    Serial.print("[HEARTBEAT] - {\"uptime\":");
    Serial.print(millis());
    Serial.print(",\"free_memory\":");
    Serial.print(freeMemory());
    Serial.print(",\"system_ready\":");
    Serial.print(systemReady ? "true" : "false");
    Serial.println("}");
}

// Function to get free memory (useful for debugging)
int freeMemory() {
    char top;
    extern char *__brkval;
    extern char __bss_end;
    return __brkval ? &top - __brkval : &top - &__bss_end;
}
