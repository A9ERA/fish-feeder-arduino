#include <Arduino.h>
#include <ArduinoJson.h>
#include "blower.h"
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "acs712_sensor.h"
#include "voltage_sensor.h"
#include "services/relay_sensor.h"
#include "sensor_service.h"

// Forward declaration of printJson function
static void printJson(String jsonString);

DeviceType parseDeviceType(const String& device) {
    if (device == "blower") return DEVICE_BLOWER;
    if (device == "actuatormotor") return DEVICE_ACTUATORMOTOR;
    if (device == "relay") return DEVICE_RELAY;
    return DEVICE_UNKNOWN;
}

void initAllSensors() {
  initDHT();
  initSoil();
  initWaterTemp();
  initWeight();
  initACS712();
  initVoltageSensor();
  initBlower();
}

void controlSensor() {
    // control command will be like this:
    // [control]:blower:start\n
    // [control]:blower:stop\n
    // [control]:blower:speed:100\n
    // [control]:blower:direction:reverse\n
    // [control]:blower:direction:normal\n
    
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (!command.startsWith("[control]:")) return;

        command = command.substring(10); // remove "[control]:"
        int firstColon = command.indexOf(':');
        if (firstColon == -1) return;

        String device = command.substring(0, firstColon);
        String rest = command.substring(firstColon + 1);

        DeviceType deviceType = parseDeviceType(device);
        switch (deviceType) {
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
                    // actuatorMotorUp();
                } else if (rest == "down") {
                    // actuatorMotorDown();
                } else if (rest == "stop") {
                    // actuatorMotorStop();
                }
                break;
            case DEVICE_RELAY:
                // Relay commands are handled by RelaySensor class
                // This is just a placeholder to acknowledge relay commands
                break;
            default:
                // ไม่รู้จักอุปกรณ์
                break;
        }
    }
}

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

static void printWaterTemp() {
  String jsonString;
  StaticJsonDocument<256> waterTemp = readWaterTemp();
  serializeJson(waterTemp, jsonString);
  printJson(jsonString);
}

static void printWeight() {
  String jsonString;
  StaticJsonDocument<256> weight = readWeight();
  serializeJson(weight, jsonString);
  printJson(jsonString);
}

static void printCurrent() {
  String jsonString;
  StaticJsonDocument<256> current = readACS712();
  serializeJson(current, jsonString);
  printJson(jsonString);
}

static void printVoltage() {
  String jsonString;
  StaticJsonDocument<256> voltage = readVoltageSensor();
  serializeJson(voltage, jsonString);
  printJson(jsonString);
}

static void printJson(String jsonString) {
  Serial.println("[SEND] - " + jsonString);
}

void readAndPrintAllSensors() {
  printDHTSystem();
  printDHTFeeder();
  printSoil();
  printWaterTemp();
  printWeight();
  printCurrent();
  printVoltage();
}

