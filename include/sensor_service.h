#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <Arduino.h>

enum DeviceType {
    DEVICE_UNKNOWN,
    DEVICE_BLOWER,
    DEVICE_ACTUATORMOTOR,
    DEVICE_RELAY
};

// Main sensor functions
void initAllSensors();
void controlSensor();
void readAndPrintAllSensors();

// Command handling functions
bool handleBlowerCommand(const String& action);
bool handleActuatorCommand(const String& action);
bool handleRelayCommand(const String& action);

// Individual sensor reading functions
static void printDHTSystem();
static void printDHTFeeder();
static void printSoil();
static void printWaterTemp();
static void printWeight();
static void printCurrent();
static void printVoltage();
static void printActuator();
static void printSolar();
static void printBattery();
static void printLoad();

// System status functions
void sendSystemStatus();
void sendHeartbeat();
int freeMemory();

#endif 