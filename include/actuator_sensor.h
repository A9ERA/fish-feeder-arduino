#ifndef ACTUATOR_SENSOR_H
#define ACTUATOR_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Pin definitions for Actuator Motor
#define ACTUATOR_UP_PIN 5     // Actuator up direction
#define ACTUATOR_DOWN_PIN 6   // Actuator down direction  
#define ACTUATOR_ENABLE_PIN 7 // Actuator enable pin

// Actuator motor states
enum ActuatorState {
    ACTUATOR_STOPPED = 0,
    ACTUATOR_MOVING_UP = 1,
    ACTUATOR_MOVING_DOWN = -1
};

// Actuator sensor name
#define ACTUATOR_SENSOR "ACTUATOR_MOTOR"

// Function declarations
void initActuator();
void startActuatorUp();
void startActuatorDown();
void stopActuator();
void setActuatorDirection(int direction);
StaticJsonDocument<256> readActuatorStatus();
void updateActuator();
bool getActuatorStatus();
int getActuatorDirection();

// Command processing
void processActuatorCommand(const String& command);

#endif 