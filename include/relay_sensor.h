#ifndef RELAY_SENSOR_H
#define RELAY_SENSOR_H

#include <Arduino.h>

class RelaySensor {
private:
    // Pin definitions
    const int RELAY_1_PIN = 50;
    const int RELAY_2_PIN = 52;
    
    // Relay states
    bool relay1State = false;
    bool relay2State = false;
    
    // Command constants
    const char* CMD_PREFIX = "[control]:relay:";
    const char* CMD_START = "start:";
    const char* CMD_STOP = "stop:";
    const char* CMD_STATUS = "status";
    
    // Buffer for incoming commands
    String commandBuffer;
    
    // Helper functions
    bool isValidRelayNumber(int relayNum);
    int getRelayPin(int relayNum);
    bool* getRelayStatePtr(int relayNum);

public:
    // Constructor
    RelaySensor();
    
    // Initialize the sensor
    void begin();
    
    // Process incoming serial data
    void update();
    
    // Process a command
    void processCommand(String command);
    
    // Toggle relay state
    void toggleRelay(int relayNum, bool turnOn);
    
    // Send current status of all relays
    void sendStatus();
    
    // Get current state of a specific relay
    bool getRelayState(int relayNum);
};

#endif // RELAY_SENSOR_H 