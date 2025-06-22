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
    DEVICE_RELAY
};

DeviceType parseDeviceType(const String& device) {
    if (device == "blower") return DEVICE_BLOWER;
    if (device == "actuator") return DEVICE_ACTUATORMOTOR;
    if (device == "auger") return DEVICE_AUGER;
    if (device == "relay") return DEVICE_RELAY;
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

        // Debug message เมื่อรับคำสั่ง
        Serial.println("[INFO] - Received command: " + command);
        Serial.flush(); // Ensure the debug message is sent immediately

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
                Serial.println("[INFO] - Weight calibration command received");
                calibrateWeight();
            }
            return;
        }
        
        DeviceType deviceType = parseDeviceType(device);
        switch (deviceType) {
            case DEVICE_BLOWER:
                if (rest == "start") {
                    startBlower();
                    Serial.println("[INFO] - Blower started");
                } else if (rest == "stop") {
                    stopBlower();
                    Serial.println("[INFO] - Blower stopped");
                } else if (rest.startsWith("speed:")) {
                    int speed = rest.substring(6).toInt();
                    setBlowerSpeed(speed);
                    Serial.println("[INFO] - Blower speed set to " + String(speed));
                } else if (rest.startsWith("direction:")) {
                    String dir = rest.substring(10);
                    if (dir == "reverse") {
                        setBlowerDirection(true);
                        Serial.println("[INFO] - Blower direction set to reverse");
                    } else if (dir == "normal") {
                        setBlowerDirection(false);
                        Serial.println("[INFO] - Blower direction set to normal");
                    }
                }
                break;
            case DEVICE_ACTUATORMOTOR:
                if (rest == "up") {
                    actuatorMotorUp();
                    Serial.println("[INFO] - Actuator moving up");
                } else if (rest == "down") {
                    actuatorMotorDown();
                    Serial.println("[INFO] - Actuator moving down");
                } else if (rest == "stop") {
                    actuatorMotorStop();
                    Serial.println("[INFO] - Actuator stopped");
                }
                break;
            case DEVICE_AUGER:
                if (rest == "forward") {
                    augerMotorForward();
                    Serial.println("[INFO] - Auger moving forward");
                } else if (rest == "backward") {
                    augerMotorBackward();
                    Serial.println("[INFO] - Auger moving backward");
                } else if (rest == "stop") {
                    augerMotorStop();
                    Serial.println("[INFO] - Auger stopped");
                } else if (rest == "speedtest") {
                    augerMotorSpeedTest();
                    Serial.println("[INFO] - Auger speed test started");
                } else if (rest.startsWith("setspeed:")) {
                    int speed = rest.substring(9).toInt();
                    augerMotorSetSpeed(speed);
                    Serial.println("[INFO] - Auger speed set to " + String(speed));
                }
                break;
            case DEVICE_RELAY:
                if (rest.startsWith("led:")) {
                    String ledCmd = rest.substring(4);
                    if (ledCmd == "on") {
                        relayLedOn();
                        Serial.println("[INFO] - LED relay turned on");
                    } else if (ledCmd == "off") {
                        relayLedOff();
                        Serial.println("[INFO] - LED relay turned off");
                    }
                } else if (rest.startsWith("fan:")) {
                    String fanCmd = rest.substring(4);
                    if (fanCmd == "on") {
                        relayFanOn();
                        Serial.println("[INFO] - Fan relay turned on");
                    } else if (fanCmd == "off") {
                        relayFanOff();
                        Serial.println("[INFO] - Fan relay turned off");
                    }
                } else if (rest == "all:off") {
                    relayAllOff();
                    Serial.println("[INFO] - All relays turned off");
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

