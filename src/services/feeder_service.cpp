#include <Arduino.h>
#include "solenoid_valve.h"
#include "blower.h"
#include "feeder_service.h"
#include "sensor_service.h"
#include "weight_sensor.h"

// Constants for solenoid valve timings
#define SOLENOID_OPEN_DURATION 5    // 5 seconds for solenoid open

// Weight monitoring constants
#define WEIGHT_CHECK_INTERVAL 100    // Check weight every 100ms
#define WEIGHT_TOLERANCE 5.0         // 5g tolerance for weight measurement
#define MAX_WEIGHT_WAIT_TIME 30000   // Maximum 30 seconds to wait for weight change

// Feeder sequence status
static bool feederSequenceActive = false;
static bool feederStopRequested = false;

void initFeederService() {
    feederSequenceActive = false;
    feederStopRequested = false;
    Serial.println("[FEEDER SERVICE] Initialized - ready to handle feeding sequences");
}

// Function to process serial commands during delays
// Returns true if should continue, false if should stop
bool processSerialCommands(bool limitedMode = false) {
    if (!Serial.available()) {
        return true; // No commands to process, continue
    }
    
    if (limitedMode) {
        // Limited mode: only process feeder:stop commands
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command == "[control]:feeder:stop") {
            Serial.println("[FEEDER] Stop command received");
            feederStopRequested = true;
            return false;
        }
        // Ignore other commands in limited mode
        return true;
    } else {
        // Normal mode: process all sensor commands
        controlSensor();
        return !feederStopRequested;
    }
}

// Interruptible delay function
bool interruptibleDelay(unsigned long delayMs) {
    unsigned long startTime = millis();
    while (millis() - startTime < delayMs) {
        if (feederStopRequested) {
            return false; // Interrupted
        }
        
        // Process incoming serial commands during delay
        if (!processSerialCommands()) {
            return false; // Stop requested
        }
        
        delay(100); // Small delay, check every 100ms
    }
    return true; // Completed normally
}

// Function to wait for weight reduction
bool waitForWeightReduction(float targetReduction) {
    Serial.println("[FEEDER] Waiting for weight reduction of " + String(targetReduction) + "g");
    
    // Stop sensor service printing to reduce noise
    bool wasSensorActive = isSensorServiceActive();
    if (wasSensorActive) {
        stopSensorService();
        Serial.println("[FEEDER] Stopped sensor service printing for weight monitoring");
    }
    
    // Get initial weight
    StaticJsonDocument<256> weightDoc = readWeight();
    float initialWeight = weightDoc["weight"];
    Serial.println("[FEEDER] Initial weight: " + String(initialWeight) + "g");
    
    unsigned long startTime = millis();
    float currentWeight = initialWeight;
    float weightReduction = 0.0;
    
    while (weightReduction < (targetReduction - WEIGHT_TOLERANCE)) {
        if (feederStopRequested) {
            Serial.println("[FEEDER] Weight monitoring stopped by user request");
            if (wasSensorActive) {
                startSensorService();
            }
            return false;
        }
        
        // Check for timeout
        if (millis() - startTime > MAX_WEIGHT_WAIT_TIME) {
            Serial.println("[FEEDER] Warning: Weight monitoring timeout after " + String(MAX_WEIGHT_WAIT_TIME/1000) + " seconds");
            if (wasSensorActive) {
                startSensorService();
            }
            return true; // Continue with sequence even if timeout
        }
        
        // Read current weight
        weightDoc = readWeight();
        currentWeight = weightDoc["weight"];
        weightReduction = initialWeight - currentWeight;
        
        Serial.println("[FEEDER] Current weight: " + String(currentWeight) + "g, Reduction: " + String(weightReduction) + "g");
        
        // Only process feeder stop commands during weight monitoring
        if (!processSerialCommands(true)) { // Limited mode: only feeder:stop commands
            if (wasSensorActive) {
                startSensorService();
            }
            return false;
        }
        
        delay(WEIGHT_CHECK_INTERVAL);
    }
    
    Serial.println("[FEEDER] Target weight reduction achieved: " + String(weightReduction) + "g");
    
    // Restart sensor service if it was active
    if (wasSensorActive) {
        startSensorService();
        Serial.println("[FEEDER] Restarted sensor service printing");
    }
    
    return true;
}

void startFeederSequence(int feedAmount, int blowerDuration) {
    if (feederSequenceActive) {
        Serial.println("[FEEDER] Warning: Feeder sequence already active, please wait");
        return;
    }
    
    feederSequenceActive = true;
    feederStopRequested = false;
    
    // Declare variables at the beginning to avoid goto bypass issues
    int remainingBlowerTime;
    
    Serial.println("[FEEDER] Starting automated feeder sequence");
    Serial.println("[FEEDER] Feed amount: " + String(feedAmount) + "g");
    
    // Step 1: Start blower and open solenoid valve simultaneously
    Serial.println("[FEEDER] Step 1: Starting blower and opening solenoid valve");
    Serial.println("[FEEDER] Blower duration: " + String(blowerDuration) + "s");
    Serial.println("[FEEDER] Waiting for weight reduction of " + String(feedAmount) + "g");
    
    startBlower();
    solenoidValveOpen();
    
    // Wait for weight reduction (no time limit for solenoid open)
    if (!waitForWeightReduction(feedAmount)) {
        solenoidValveClose();
        stopBlower();
        goto emergency_stop;
    }
    Serial.println("[FEEDER] Step 1 completed: Target weight reduction achieved");
    
    // Step 2: Close solenoid valve immediately when weight target is reached
    Serial.println("[FEEDER] Step 2: Closing solenoid valve");
    solenoidValveClose();
    Serial.println("[FEEDER] Step 2 completed: Solenoid valve close finished, blower continues");
    
    // Step 3: Continue blower for remaining duration
    remainingBlowerTime = blowerDuration;
    
    if (remainingBlowerTime > 0) {
        Serial.println("[FEEDER] Step 3: Continuing blower for remaining " + String(remainingBlowerTime) + "s");
        if (!interruptibleDelay(remainingBlowerTime * 1000)) {
            stopBlower();
            goto emergency_stop;
        }
    }
    
    stopBlower();
    
    feederSequenceActive = false;
    feederStopRequested = false;
    Serial.println("[FEEDER] Automated feeder sequence completed successfully!");
    return;

emergency_stop:
    solenoidValveClose();
    stopBlower();
    feederSequenceActive = false;
    feederStopRequested = false;
    
    // Make sure to restart sensor service if it was stopped
    if (!isSensorServiceActive()) {
        startSensorService();
        Serial.println("[FEEDER] Restarted sensor service after emergency stop");
    }
    
    Serial.println("[FEEDER] Feeder sequence stopped by user request");
}

void stopFeederSequence() {
    if (feederSequenceActive) {
        Serial.println("[FEEDER] Stop request received - stopping sequence");
        feederStopRequested = true;
    } else {
        Serial.println("[FEEDER] No active sequence to stop");
    }
}
