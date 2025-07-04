#include <Arduino.h>
#include <ArduinoJson.h>
#include "blower.h"
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "weight_sensor.h"
#include "power_monitor.h"
#include "actuator_motor.h"
#include "auger_motor.h"
#include "relay_control.h"
#include "sensor_service.h"
#include "feeder_service.h"

// Forward declaration of printJson function
static void printJson(String jsonString);

// Timer-based sensor service variables
static unsigned long sensorPrintInterval = 5000; // Default 5 seconds
static unsigned long lastSensorPrintTime = 0;
static bool sensorServiceActive = false;

enum DeviceType {
    DEVICE_UNKNOWN,
    DEVICE_BLOWER,
    DEVICE_ACTUATORMOTOR,
    DEVICE_AUGER,
    DEVICE_RELAY,
    DEVICE_FEEDER
};

DeviceType parseDeviceType(const String& device) {
    if (device == "blower") return DEVICE_BLOWER;
    if (device == "actuator") return DEVICE_ACTUATORMOTOR;
    if (device == "auger") return DEVICE_AUGER;
    if (device == "relay") return DEVICE_RELAY;
    if (device == "feeder") return DEVICE_FEEDER;
    return DEVICE_UNKNOWN;
}

// Helper functions to print individual sensor data
static void printDHTSystem() {
  String jsonString;
  StaticJsonDocument<256> dhtSystem = readDHTSystem();
  serializeJson(dhtSystem, jsonString);
  printJson(jsonString);
}

static void printDHTFeeder() {
  String jsonString;
  StaticJsonDocument<256> dhtFeeder = readDHTFeeder();
  serializeJson(dhtFeeder, jsonString);
  printJson(jsonString);
}

static void printSoil() {
  String jsonString;
  StaticJsonDocument<256> soil = readSoil();
  serializeJson(soil, jsonString);
  printJson(jsonString);
}

static void printWeight() {
  String jsonString;
  StaticJsonDocument<256> weight = readWeight();
  serializeJson(weight, jsonString);
  printJson(jsonString);
}

static void printPowerMonitor() {
  String jsonString;
  StaticJsonDocument<1024> powerMonitor = readPowerMonitor();
  serializeJson(powerMonitor, jsonString);
  printJson(jsonString);
}

static void printJson(String jsonString) {
  Serial.println("[SEND] - " + jsonString);
}

void initAllSensors() {
  // Read sensors
  initDHT();
  initSoil();
  initWeight();
  initPowerMonitor();

  // Control devices
  initBlower();
  initActuatorMotor();
  initAugerMotor();
  initRelayControl();
}

// New timer-based sensor service functions
void initSensorService() {
  lastSensorPrintTime = millis();
  sensorServiceActive = true;
  Serial.println("[INFO] - Sensor service initialized in background mode");
}

void updateSensorService() {
  if (!sensorServiceActive) return;
  
  unsigned long currentMillis = millis();
  
  // Check if it's time to print sensor data
  if (currentMillis - lastSensorPrintTime >= sensorPrintInterval) {
    // Print all sensor data
    printDHTSystem();
    printDHTFeeder();
    printSoil();
    printWeight();
    printPowerMonitor();
    
    lastSensorPrintTime = currentMillis;
  }
}

void setSensorPrintInterval(unsigned long intervalMs) {
  sensorPrintInterval = intervalMs;
  Serial.println("[INFO] - Sensor print interval set to: " + String(intervalMs) + "ms");
}

// Sensor service control functions
void startSensorService() {
  sensorServiceActive = true;
  lastSensorPrintTime = millis();
  Serial.println("[INFO] - Sensor service started");
}

void stopSensorService() {
  sensorServiceActive = false;
  Serial.println("[INFO] - Sensor service stopped");
}

bool isSensorServiceActive() {
  return sensorServiceActive;
}

void controlSensor() {
    // control command will be:
    
    // Device controls:
    // [control]:blower:start\n
    // [control]:blower:stop\n
    // [control]:blower:speed:100\n
    // [control]:blower:direction:reverse\n
    // [control]:blower:direction:normal\n
    // [control]:actuator:up\n
    // [control]:actuator:down\n
    // [control]:actuator:stop\n
    // [control]:auger:forward\n
    // [control]:auger:backward\n
    // [control]:auger:stop\n
    // [control]:auger:speedtest\n
    // [control]:auger:setspeed:100\n
    // [control]:relay:led:on\n
    // [control]:relay:led:off\n
    // [control]:relay:fan:on\n
    // [control]:relay:fan:off\n
    // [control]:relay:all:off\n
    
    // Feeder sequence controls:
    // [control]:feeder:start:feedAmount,augerDuration,blowerDuration\n
    // [control]:feeder:stop\n
    
    // Sensor service controls:
    // [control]:sensors:start\n
    // [control]:sensors:stop\n
    // [control]:sensors:interval:1000\n
    // [control]:sensors:status\n
    
    // Weight calibration controls:
    // [control]:weight:calibrate\n
    
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (!command.startsWith("[control]:")) return;


        command = command.substring(10); // remove "[control]:"
        int firstColon = command.indexOf(':');
        if (firstColon == -1) return;

        String device = command.substring(0, firstColon);
        String rest = command.substring(firstColon + 1);
        
        // Handle sensor service commands
        if (device == "sensors") {
            if (rest == "start") {
                startSensorService();
            } else if (rest == "stop") {
                stopSensorService();
            } else if (rest.startsWith("interval:")) {
                unsigned long interval = rest.substring(9).toInt();
                setSensorPrintInterval(interval);
            } else if (rest == "status") {
                Serial.println("[INFO] - Sensor service status: " + 
                             String(isSensorServiceActive() ? "ACTIVE" : "INACTIVE"));
                Serial.println("[INFO] - Print interval: " + String(sensorPrintInterval) + "ms");
            }
            return;
        }
        
        // Handle weight calibration commands
        if (device == "weight") {
            if (rest == "calibrate") {
                calibrateWeight();
            }
            return;
        }
        
        DeviceType deviceType = parseDeviceType(device);
        switch (deviceType) {
            case DEVICE_FEEDER:
                if (rest.startsWith("start:")) {
                    String params = rest.substring(6);
                    
                    // Parse parameters: feedAmount,augerDuration,blowerDuration
                    int comma1 = params.indexOf(',');
                    int comma2 = params.indexOf(',', comma1 + 1);
                    
                    if (comma1 != -1 && comma2 != -1) {
                        int feedAmount = params.substring(0, comma1).toInt();
                        int augerDuration = params.substring(comma1 + 1, comma2).toInt();
                        int blowerDuration = params.substring(comma2 + 1).toInt();
                        
                        startFeederSequence(feedAmount, augerDuration, blowerDuration);
                    } else {
                        Serial.println("[ERROR] - Invalid feeder start parameters format. Expected: feedAmount,augerDuration,blowerDuration");
                    }
                } else if (rest == "stop") {
                    stopFeederSequence();
                }
                break;
            case DEVICE_BLOWER:
                if (rest == "start") {
                    startBlower();
                } else if (rest == "stop") {
                    stopBlower();
                } else if (rest.startsWith("speed:")) {
                    int speed = rest.substring(6).toInt();
                    setBlowerSpeed(speed);
                } else if (rest.startsWith("direction:")) {
                    String dir = rest.substring(10);
                    if (dir == "reverse") {
                        setBlowerDirection(true);
                    } else if (dir == "normal") {
                        setBlowerDirection(false);
                    }
                }
                break;
            case DEVICE_ACTUATORMOTOR:
                if (rest == "up") {
                    actuatorMotorUp();
                } else if (rest == "down") {
                    actuatorMotorDown();
                } else if (rest == "stop") {
                    actuatorMotorStop();
                }
                break;
            case DEVICE_AUGER:
                if (rest == "forward") {
                    augerMotorForward();
                } else if (rest == "backward") {
                    augerMotorBackward();
                } else if (rest == "stop") {
                    augerMotorStop();
                } else if (rest == "speedtest") {
                    augerMotorSpeedTest();
                } else if (rest.startsWith("setspeed:")) {
                    int speed = rest.substring(9).toInt();
                    augerMotorSetSpeed(speed);
                }
                break;
            case DEVICE_RELAY:
                if (rest.startsWith("led:")) {
                    String ledCmd = rest.substring(4);
                    if (ledCmd == "on") {
                        relayLedOn();
                    } else if (ledCmd == "off") {
                        relayLedOff();
                    }
                } else if (rest.startsWith("fan:")) {
                    String fanCmd = rest.substring(4);
                    if (fanCmd == "on") {
                        relayFanOn();
                    } else if (fanCmd == "off") {
                        relayFanOff();
                    }
                } else if (rest == "all:off") {
                    relayAllOff();
                }
                break;
            default:
                // ไม่รู้จักอุปกรณ์
                Serial.println("[INFO] - Unknown device: " + device);
                break;
        }
    }
}

void readAndPrintAllSensors() {
  // Get current time in seconds since boot
  unsigned long currentMillis = millis();
  unsigned long currentSeconds = (currentMillis / 1000) % 60;
  
  // Only print when seconds value is divisible by 5 (0, 5, 10, 15, 20, ...)
  if (currentSeconds % 2 == 0) {
    // Add a small delay to prevent multiple prints within the same second
    static unsigned long lastPrintTime = 0;
    if (currentMillis - lastPrintTime >= 2000) { // At least 5 seconds between prints
      printDHTSystem();
      printDHTFeeder();
      printSoil();
      printWeight();
      printPowerMonitor();
      
      lastPrintTime = currentMillis;
    }
  }
}

