#include <Arduino.h>
#include "actuator_motor.h"
#include "auger_motor.h"
#include "blower.h"
#include "feeder_service.h"
#include "sensor_service.h"

// Feeder sequence status
static bool feederSequenceActive = false;
static bool feederStopRequested = false;

void initFeederService() {
    feederSequenceActive = false;
    feederStopRequested = false;
    Serial.println("[FEEDER SERVICE] Initialized - ready to handle feeding sequences");
}

// Interruptible delay function
bool interruptibleDelay(unsigned long delayMs) {
    unsigned long startTime = millis();
    while (millis() - startTime < delayMs) {
        if (feederStopRequested) {
            return false; // Interrupted
        }
        
        // Process incoming serial commands during delay
        controlSensor();
        
        delay(100); // Small delay, check every 100ms
    }
    return true; // Completed normally
}

void startFeederSequence(int actuatorUp, int actuatorDown, int augerDuration, int blowerDuration) {
    if (feederSequenceActive) {
        Serial.println("[FEEDER] Warning: Feeder sequence already active, please wait");
        return;
    }
    
    feederSequenceActive = true;
    feederStopRequested = false;
    
    Serial.println("[FEEDER] Starting automated feeder sequence");
    
    // Step 1: Actuator up
    Serial.println("[FEEDER] Step 1: Moving actuator up for " + String(actuatorUp) + " seconds");
    actuatorMotorUp();
    if (!interruptibleDelay(actuatorUp * 1000)) {
        actuatorMotorStop();
        goto emergency_stop;
    }
    actuatorMotorStop();
    Serial.println("[FEEDER] Step 1 completed: Actuator up movement finished");
    
    // Small delay between steps
    if (!interruptibleDelay(1000)) {
        goto emergency_stop;
    }
    
    // Step 2: Actuator down
    Serial.println("[FEEDER] Step 2: Moving actuator down for " + String(actuatorDown) + " seconds");
    actuatorMotorDown();
    if (!interruptibleDelay(actuatorDown * 1000)) {
        actuatorMotorStop();
        goto emergency_stop;
    }
    actuatorMotorStop();
    Serial.println("[FEEDER] Step 2 completed: Actuator down movement finished");
    
    // Small delay before starting auger and blower
    if (!interruptibleDelay(500)) {
        goto emergency_stop;
    }
    
    // Step 3: Start auger and blower simultaneously
    Serial.println("[FEEDER] Step 3: Starting auger (forward) and blower simultaneously");
    Serial.println("[FEEDER] Auger duration: " + String(augerDuration) + "s, Blower duration: " + String(blowerDuration) + "s");
    
    augerMotorForward();
    startBlower();
    
    // Wait for the shorter duration, then stop that device
    int minDuration = min(augerDuration, blowerDuration);
    
    // Wait for the shorter duration
    if (!interruptibleDelay(minDuration * 1000)) {
        goto emergency_stop;
    }
    
    // Stop the device with shorter duration
    if (augerDuration <= blowerDuration) {
        augerMotorStop();
        // Wait for remaining blower time
        if (blowerDuration > augerDuration) {
            int remainingTime = blowerDuration - augerDuration;
            if (!interruptibleDelay(remainingTime * 1000)) {
                goto emergency_stop;
            }
        }
        stopBlower();
    } else {
        stopBlower();
        // Wait for remaining auger time
        int remainingTime = augerDuration - blowerDuration;
        if (!interruptibleDelay(remainingTime * 1000)) {
            goto emergency_stop;
        }
        augerMotorStop();
    }
    
    feederSequenceActive = false;
    feederStopRequested = false;
    Serial.println("[FEEDER] Automated feeder sequence completed successfully!");
    return;

emergency_stop:
    actuatorMotorStop();
    augerMotorStop();
    stopBlower();
    feederSequenceActive = false;
    feederStopRequested = false;
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
