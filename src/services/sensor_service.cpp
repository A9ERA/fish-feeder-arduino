#include <Arduino.h>
#include <ArduinoJson.h>
#include "blower.h"
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "acs712_sensor.h"
#include "voltage_sensor.h"
#include "actuator_sensor.h"
#include "solar_battery_sensor.h"
#include "services/relay_sensor.h"
#include "sensor_service.h"

// Forward declaration of printJson function
static void printJson(String jsonString);
static void printSensorWithTimestamp(const char* sensorType, StaticJsonDocument<256>& doc);

// Error tracking
static int sensorErrors[11] = {0}; // Track errors for each sensor type
static const char* sensorNames[] = {
    "DHT_SYSTEM", "DHT_FEEDER", "SOIL", "WATER_TEMP", "WEIGHT", 
    "CURRENT", "VOLTAGE", "ACTUATOR", "SOLAR", "BATTERY", "LOAD"
};

DeviceType parseDeviceType(const String& device) {
    if (device == "blower") return DEVICE_BLOWER;
    if (device == "actuatormotor") return DEVICE_ACTUATORMOTOR;
    if (device == "relay") return DEVICE_RELAY;
    return DEVICE_UNKNOWN;
}

void initAllSensors() {
    Serial.println("🔧 Initializing all sensors...");
    
    // Initialize each sensor with error handling
    try {
        initDHT();
        Serial.println("✅ DHT sensors initialized");
    } catch (...) {
        Serial.println("❌ DHT sensor initialization failed");
        sensorErrors[0]++;
    }
    
    try {
        initSoil();
        Serial.println("✅ Soil sensor initialized");
    } catch (...) {
        Serial.println("❌ Soil sensor initialization failed");
        sensorErrors[2]++;
    }
    
    try {
        initWaterTemp();
        Serial.println("✅ Water temperature sensor initialized");
    } catch (...) {
        Serial.println("❌ Water temperature sensor initialization failed");
        sensorErrors[3]++;
    }
    
    try {
        initWeight();
        Serial.println("✅ Weight sensor initialized");
    } catch (...) {
        Serial.println("❌ Weight sensor initialization failed");
        sensorErrors[4]++;
    }
    
    try {
        initACS712();
        Serial.println("✅ Current sensor initialized");
    } catch (...) {
        Serial.println("❌ Current sensor initialization failed");
        sensorErrors[5]++;
    }
    
    try {
        initVoltageSensor();
        Serial.println("✅ Voltage sensor initialized");
    } catch (...) {
        Serial.println("❌ Voltage sensor initialization failed");
        sensorErrors[6]++;
    }
    
    try {
        initActuator();
        Serial.println("✅ Actuator motor initialized");
    } catch (...) {
        Serial.println("❌ Actuator motor initialization failed");
        sensorErrors[7]++;
    }
    
    try {
        initSolarBattery();
        Serial.println("✅ Solar battery system initialized");
    } catch (...) {
        Serial.println("❌ Solar battery system initialization failed");
        sensorErrors[8]++;
    }
    
    try {
        initBlower();
        Serial.println("✅ Blower system initialized");
    } catch (...) {
        Serial.println("❌ Blower system initialization failed");
    }
    
    Serial.println("🎯 Sensor initialization complete");
}

void controlSensor() {
    // Enhanced control command processing
    // Commands format: [control]:device:action:parameter\n
    // Examples:
    // [control]:blower:start
    // [control]:blower:stop
    // [control]:blower:speed:100
    // [control]:blower:direction:reverse
    // [control]:relay:1:on
    // [control]:actuatormotor:up
    
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (!command.startsWith("[control]:")) return;

        command = command.substring(10); // remove "[control]:"
        
        // Parse command parts
        int firstColon = command.indexOf(':');
        if (firstColon == -1) {
            Serial.println("[ERROR] - Invalid command format");
            return;
        }

        String device = command.substring(0, firstColon);
        String rest = command.substring(firstColon + 1);

        // Log received command
        Serial.println("[CMD_RECV] - Device: " + device + ", Action: " + rest);

        DeviceType deviceType = parseDeviceType(device);
        bool commandExecuted = false;
        
        switch (deviceType) {
            case DEVICE_BLOWER:
                commandExecuted = handleBlowerCommand(rest);
                break;
            case DEVICE_ACTUATORMOTOR:
                commandExecuted = handleActuatorCommand(rest);
                break;
            case DEVICE_RELAY:
                commandExecuted = handleRelayCommand(rest);
                break;
            default:
                Serial.println("[ERROR] - Unknown device: " + device);
                return;
        }
        
        // Send command acknowledgment
        Serial.print("[CMD_ACK] - {\"device\":\"");
        Serial.print(device);
        Serial.print("\",\"action\":\"");
        Serial.print(rest);
        Serial.print("\",\"status\":\"");
        Serial.print(commandExecuted ? "success" : "failed");
        Serial.println("\"}");
    }
}

bool handleBlowerCommand(const String& action) {
    if (action == "start") {
        startBlower();
        return true;
    } else if (action == "stop") {
        stopBlower();
        return true;
    } else if (action.startsWith("speed:")) {
        int speed = action.substring(6).toInt();
        if (speed >= 0 && speed <= 255) {
            setBlowerSpeed(speed);
            return true;
        }
    } else if (action.startsWith("direction:")) {
        String dir = action.substring(10);
        if (dir == "reverse") {
            setBlowerDirection(true);
            return true;
        } else if (dir == "normal") {
            setBlowerDirection(false);
            return true;
        }
    }
    return false;
}

bool handleActuatorCommand(const String& action) {
    if (action == "up") {
        startActuatorUp();
        return true;
    } else if (action == "down") {
        startActuatorDown();
        return true;
    } else if (action == "stop") {
        stopActuator();
        return true;
    }
    return false;
}

bool handleRelayCommand(const String& action) {
    // Relay commands are handled by RelaySensor class
    Serial.println("[INFO] - Relay command received: " + action);
    return true;
}

static void printDHTSystem() {
    try {
        StaticJsonDocument<256> dhtSystem = readDHTSystem();
        printSensorWithTimestamp("DHT_SYSTEM", dhtSystem);
        sensorErrors[0] = 0; // Reset error count on success
    } catch (...) {
        sensorErrors[0]++;
        Serial.println("[ERROR] - DHT System sensor read failed (Error count: " + String(sensorErrors[0]) + ")");
    }
}

static void printDHTFeeder() {
    try {
        StaticJsonDocument<256> dhtFeeder = readDHTFeeder();
        printSensorWithTimestamp("DHT_FEEDER", dhtFeeder);
        sensorErrors[1] = 0; // Reset error count on success
    } catch (...) {
        sensorErrors[1]++;
        Serial.println("[ERROR] - DHT Feeder sensor read failed (Error count: " + String(sensorErrors[1]) + ")");
    }
}

static void printSoil() {
    try {
        StaticJsonDocument<256> soil = readSoil();
        printSensorWithTimestamp("SOIL", soil);
        sensorErrors[2] = 0;
    } catch (...) {
        sensorErrors[2]++;
        Serial.println("[ERROR] - Soil sensor read failed (Error count: " + String(sensorErrors[2]) + ")");
    }
}

static void printWaterTemp() {
    try {
        StaticJsonDocument<256> waterTemp = readWaterTemp();
        printSensorWithTimestamp("WATER_TEMP", waterTemp);
        sensorErrors[3] = 0;
    } catch (...) {
        sensorErrors[3]++;
        Serial.println("[ERROR] - Water temperature sensor read failed (Error count: " + String(sensorErrors[3]) + ")");
    }
}

static void printWeight() {
    try {
        StaticJsonDocument<256> weight = readWeight();
        printSensorWithTimestamp("WEIGHT", weight);
        sensorErrors[4] = 0;
    } catch (...) {
        sensorErrors[4]++;
        Serial.println("[ERROR] - Weight sensor read failed (Error count: " + String(sensorErrors[4]) + ")");
    }
}

static void printCurrent() {
    try {
        StaticJsonDocument<256> current = readACS712();
        printSensorWithTimestamp("CURRENT", current);
        sensorErrors[5] = 0;
    } catch (...) {
        sensorErrors[5]++;
        Serial.println("[ERROR] - Current sensor read failed (Error count: " + String(sensorErrors[5]) + ")");
    }
}

static void printVoltage() {
    try {
        StaticJsonDocument<256> voltage = readVoltageSensor();
        printSensorWithTimestamp("VOLTAGE", voltage);
        sensorErrors[6] = 0;
    } catch (...) {
        sensorErrors[6]++;
        Serial.println("[ERROR] - Voltage sensor read failed (Error count: " + String(sensorErrors[6]) + ")");
    }
}

static void printActuator() {
    try {
        StaticJsonDocument<256> actuator = readActuatorStatus();
        printSensorWithTimestamp("ACTUATOR", actuator);
        sensorErrors[7] = 0;
    } catch (...) {
        sensorErrors[7]++;
        Serial.println("[ERROR] - Actuator sensor read failed (Error count: " + String(sensorErrors[7]) + ")");
    }
}

static void printSolar() {
    try {
        StaticJsonDocument<256> solar = readSolarStatus();
        printSensorWithTimestamp("SOLAR", solar);
        sensorErrors[8] = 0;
    } catch (...) {
        sensorErrors[8]++;
        Serial.println("[ERROR] - Solar sensor read failed (Error count: " + String(sensorErrors[8]) + ")");
    }
}

static void printBattery() {
    try {
        StaticJsonDocument<256> battery = readBatteryStatus();
        printSensorWithTimestamp("BATTERY", battery);
        sensorErrors[9] = 0;
    } catch (...) {
        sensorErrors[9]++;
        Serial.println("[ERROR] - Battery sensor read failed (Error count: " + String(sensorErrors[9]) + ")");
    }
}

static void printLoad() {
    try {
        StaticJsonDocument<256> load = readLoadStatus();
        printSensorWithTimestamp("LOAD", load);
        sensorErrors[10] = 0;
    } catch (...) {
        sensorErrors[10]++;
        Serial.println("[ERROR] - Load sensor read failed (Error count: " + String(sensorErrors[10]) + ")");
    }
}

static void printSensorWithTimestamp(const char* sensorType, StaticJsonDocument<256>& doc) {
    // Add timestamp to sensor data
    doc["timestamp"] = millis();
    doc["sensor_type"] = sensorType;
    
    String jsonString;
    serializeJson(doc, jsonString);
    printJson(jsonString);
}

static void printJson(String jsonString) {
    Serial.println("[SEND] - " + jsonString);
}

void readAndPrintAllSensors() {
    Serial.println("📊 Reading all sensors...");
    
    printDHTSystem();
    delay(100); // Small delay between sensor reads
    
    printDHTFeeder();
    delay(100);
    
    printSoil();
    delay(100);
    
    printWaterTemp();
    delay(100);
    
    printWeight();
    delay(100);
    
    printCurrent();
    delay(100);
    
    printVoltage();
    delay(100);
    
    printActuator();
    delay(100);
    
    printSolar();
    delay(100);
    
    printBattery();
    delay(100);
    
    printLoad();
    
    // Print sensor error summary if there are any errors
    bool hasErrors = false;
    for (int i = 0; i < 11; i++) {
        if (sensorErrors[i] > 0) {
            hasErrors = true;
            break;
        }
    }
    
    if (hasErrors) {
        Serial.print("[SENSOR_ERRORS] - {");
        for (int i = 0; i < 11; i++) {
            if (i > 0) Serial.print(",");
            Serial.print("\"");
            Serial.print(sensorNames[i]);
            Serial.print("\":");
            Serial.print(sensorErrors[i]);
        }
        Serial.println("}");
    }
    
    Serial.println("✅ Sensor reading cycle complete");
}

