#include <Arduino.h>
#include <ArduinoJson.h>
#include "blower.h"
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "acs712_sensor.h"
#include "voltage_sensor.h"
#include "actuator_motor.h"
#include "auger_motor.h"
#include "relay_control.h"
#include "sensor_service.h"

// Forward declaration of printJson function
static void printJson(String jsonString);

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

void initAllSensors() {
  // Read sensors
  initDHT();
  initSoil();
  initWaterTemp();
  initWeight();
  initACS712();
  initVoltageSensor();

  // Control devices
  initBlower();
  initActuatorMotor();
  initAugerMotor();
  initRelayControl();
}

void controlSensor() {
    // control command will be:
    
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
    // [control]:relay:led:on\n
    // [control]:relay:led:off\n
    // [control]:relay:fan:on\n
    // [control]:relay:fan:off\n
    // [control]:relay:all:off\n
    
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
  // Get current time in seconds since boot
  unsigned long currentMillis = millis();
  unsigned long currentSeconds = (currentMillis / 1000) % 60;
  
  // Only print when seconds value is divisible by 5 (0, 5, 10, 15, 20, ...)
  if (currentSeconds % 5 == 0) {
    // Add a small delay to prevent multiple prints within the same second
    static unsigned long lastPrintTime = 0;
    if (currentMillis - lastPrintTime >= 5000) { // At least 5 seconds between prints
      printDHTSystem();
      printDHTFeeder();
      printSoil();
      printWaterTemp();
      printWeight();
      printCurrent();
      printVoltage();
      
      lastPrintTime = currentMillis;
    }
  }
}

