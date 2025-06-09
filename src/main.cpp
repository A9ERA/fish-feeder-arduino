/*
  🐟 Fish Feeder IoT System - Arduino Mega 2560
  ===============================================
  ✨ Complete IoT fish feeding system with web interface
  
  🎯 Features:
  ✅ Modular sensor system with clean architecture
  ✅ Real HX711 Load Cell with EEPROM calibration
  ✅ Web-configurable motor speeds and thresholds
  ✅ Auto temperature-controlled fan system
  ✅ Precise feeding by weight with safety limits
  ✅ Multi-sensor monitoring with error detection
  ✅ Beautiful emoji-based status output
  ✅ EEPROM settings persistence
  ✅ Comprehensive safety features

  📡 Communication Protocol:
  Sensor Output: 🍚 Temp: XX °C, Humidity: XX %
  Control Commands: R:1/2/0, G:1/2/0/3, B:1/0, A:1/2/0
  Configuration: CFG:auger_speed:200, CFG:temp_threshold:30
  Calibration: CAL:weight:1.5, CAL:reset, CAL:tare
  Feeding: FEED:small/medium/large

  🎨 Created by: Fish Feeder Team
  📅 Version: 2.1 Modular Architecture
  🚀 Ready for Raspberry Pi deployment
*/

// ===== 📚 INCLUDES =====
#include <Arduino.h>
#include <EEPROM.h>
#include "sensor_data.h"
#include "sensor_service.h"
#include "weight_sensor.h"

// ===== 🎛️ GLOBAL VARIABLES =====
Config config;
SensorData sensors;
SystemStatus status;

// ===== ⏰ TIMING =====
unsigned long lastFanCheck = 0;

// ===== 📋 FUNCTION DECLARATIONS =====
void printStartupBanner();
void printHelp();
void initializeHardware();
void loadConfiguration();
void saveConfiguration();
void handleSerialInput();
void parseCommand(String cmd);
void handleRelayCommand(char cmd);
void handleAugerCommand(char cmd);
void speedTestAuger();
void handleBlowerCommand(char cmd);
void handleActuatorCommand(char cmd);
void handleCalibrationCommand(String cmd);
void handleConfigCommand(String cmd);
void handleFeedCommand(String cmd);
void startFeeding(float amount);
void checkFeedingProgress();
void stopFeeding(String reason);
void checkAutoFan();
void stopAllMotors();
void stopAuger();
void stopActuator();

// ===== 🚀 SETUP =====
void setup() {
  Serial.begin(9600);
  
  printStartupBanner();
  initializeHardware();
  loadConfiguration();
  sensorService.begin();
  
  Serial.println(F("✅ Fish Feeder System Ready"));
  printHelp();
  delay(2000);
}

// ===== 🔄 MAIN LOOP =====
void loop() {
  unsigned long now = millis();
  
  // Read sensors
  if (sensorService.shouldReadSensors()) {
    sensorService.readAllSensors();
    sensorService.updateTimings();
  }
  
  // Output data
  if (sensorService.shouldOutputData()) {
    sensorService.outputSensorData();
    sensorService.updateTimings();
  }
  
  // Auto fan control
  if (config.auto_fan_enabled && now - lastFanCheck >= 5000) {
    checkAutoFan();
    lastFanCheck = now;
  }
  
  // Feeding progress
  if (status.is_feeding) {
    checkFeedingProgress();
  }
  
  // Handle commands
  handleSerialInput();
  
  delay(100);
}

// ===== 🎨 STARTUP BANNER =====
void printStartupBanner() {
  Serial.println(F(""));
  Serial.println(F("🐟 =========================================="));
  Serial.println(F("🐟   Fish Feeder IoT System v2.1"));
  Serial.println(F("🐟   Arduino Mega 2560 - Modular"));
  Serial.println(F("🐟 =========================================="));
  Serial.println(F(""));
  Serial.println(F("🔧 Initializing hardware..."));
}

void printHelp() {
  Serial.println(F(""));
  Serial.println(F("📋 Available Commands:"));
  Serial.println(F("🔌 Relay: R:1(LED) R:2(Fan) R:0(All Off)"));
  Serial.println(F("⚙️  Auger: G:1(Forward) G:2(Back) G:0(Stop) G:3(Test)"));
  Serial.println(F("💨 Blower: B:1(On) B:0(Off)"));
  Serial.println(F("🔧 Actuator: A:1(Open) A:2(Close) A:0(Stop)"));
  Serial.println(F("🍽️  Feed: FEED:small/medium/large"));
  Serial.println(F("⚖️  Calibrate: CAL:weight:1.5, CAL:tare, CAL:reset"));
  Serial.println(F("⚙️  Config: CFG:auger_speed:200, CFG:temp_threshold:30"));
  Serial.println(F(""));
}

// ===== 🔧 HARDWARE INITIALIZATION =====
void initializeHardware() {
  // Control pins as OUTPUT
  pinMode(RELAY_LED, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(AUGER_ENA, OUTPUT);
  pinMode(AUGER_IN1, OUTPUT);
  pinMode(AUGER_IN2, OUTPUT);
  pinMode(BLOWER_PIN, OUTPUT);
  pinMode(ACTUATOR_ENA, OUTPUT);
  pinMode(ACTUATOR_IN1, OUTPUT);
  pinMode(ACTUATOR_IN2, OUTPUT);
  
  // Initialize all to OFF/STOP
  digitalWrite(RELAY_LED, HIGH);
  digitalWrite(RELAY_FAN, HIGH);
  stopAllMotors();
  
  Serial.println(F("🔧 Hardware initialized"));
}

// ===== 💾 CONFIGURATION MANAGEMENT =====
void loadConfiguration() {
  EEPROM.get(EEPROM_CONFIG_ADDR, config);
  
  // Check for valid configuration
  if (config.version != 1) {
    Serial.println(F("📝 Loading default configuration"));
    config = Config(); // Reset to default
    saveConfiguration();
  } else {
    Serial.println(F("📝 Configuration loaded from EEPROM"));
  }
}

void saveConfiguration() {
  EEPROM.put(EEPROM_CONFIG_ADDR, config);
  Serial.println(F("💾 Configuration saved"));
}

// ===== 📟 COMMAND HANDLING =====
void handleSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();
    
    if (input.length() > 0) {
      parseCommand(input);
    }
  }
}

void parseCommand(String cmd) {
  if (cmd.startsWith("R:")) {
    handleRelayCommand(cmd.charAt(2));
  } else if (cmd.startsWith("G:")) {
    handleAugerCommand(cmd.charAt(2));
  } else if (cmd.startsWith("B:")) {
    handleBlowerCommand(cmd.charAt(2));
  } else if (cmd.startsWith("A:")) {
    handleActuatorCommand(cmd.charAt(2));
  } else if (cmd.startsWith("CAL:")) {
    handleCalibrationCommand(cmd.substring(4));
  } else if (cmd.startsWith("CFG:")) {
    handleConfigCommand(cmd.substring(4));
  } else if (cmd.startsWith("FEED:")) {
    handleFeedCommand(cmd.substring(5));
  } else {
    Serial.print(F("❌ Unknown: "));
    Serial.println(cmd);
  }
}

// ===== 🔌 RELAY CONTROL =====
void handleRelayCommand(char cmd) {
  switch (cmd) {
    case '1':
      digitalWrite(RELAY_LED, LOW);
      digitalWrite(RELAY_FAN, HIGH);
      status.relay_led = true;
      status.relay_fan = false;
      Serial.println(F("✅ LED ON"));
      break;
    case '2':
      digitalWrite(RELAY_LED, HIGH);
      digitalWrite(RELAY_FAN, LOW);
      status.relay_led = false;
      status.relay_fan = true;
      Serial.println(F("✅ Fan ON"));
      break;
    case '0':
      digitalWrite(RELAY_LED, HIGH);
      digitalWrite(RELAY_FAN, HIGH);
      status.relay_led = false;
      status.relay_fan = false;
      Serial.println(F("✅ All Relays OFF"));
      break;
  }
}

// ===== ⚙️ AUGER CONTROL =====
void handleAugerCommand(char cmd) {
  switch (cmd) {
    case '1':
      digitalWrite(AUGER_IN1, HIGH);
      digitalWrite(AUGER_IN2, LOW);
      analogWrite(AUGER_ENA, config.auger_speed_forward);
      status.auger_state = "forward";
      Serial.print(F("✅ Auger Forward ("));
      Serial.print(config.auger_speed_forward * 100 / 255);
      Serial.println(F("%)"));
      break;
    case '2':
      digitalWrite(AUGER_IN1, LOW);
      digitalWrite(AUGER_IN2, HIGH);
      analogWrite(AUGER_ENA, config.auger_speed_backward);
      status.auger_state = "backward";
      Serial.print(F("✅ Auger Backward ("));
      Serial.print(config.auger_speed_backward * 100 / 255);
      Serial.println(F("%)"));
      break;
    case '0':
      stopAuger();
      Serial.println(F("✅ Auger STOP"));
      break;
    case '3':
      speedTestAuger();
      break;
  }
}

void speedTestAuger() {
  Serial.println(F("🔧 Auger Speed Test..."));
  status.auger_state = "testing";
  
  int speeds[] = {64, 128, 192, 255};
  for (int i = 0; i < 4; i++) {
    digitalWrite(AUGER_IN1, HIGH);
    digitalWrite(AUGER_IN2, LOW);
    analogWrite(AUGER_ENA, speeds[i]);
    Serial.print(F("   Speed: "));
    Serial.print(speeds[i] * 100 / 255);
    Serial.println(F("%"));
    delay(2000);
  }
  stopAuger();
  Serial.println(F("✅ Speed Test Complete"));
}

// ===== 💨 BLOWER CONTROL =====
void handleBlowerCommand(char cmd) {
  switch (cmd) {
    case '1':
      analogWrite(BLOWER_PIN, config.blower_speed);
      status.blower_state = true;
      Serial.print(F("✅ Blower ON ("));
      Serial.print(config.blower_speed * 100 / 255);
      Serial.println(F("%)"));
      break;
    case '0':
      analogWrite(BLOWER_PIN, 0);
      status.blower_state = false;
      Serial.println(F("✅ Blower OFF"));
      break;
  }
}

// ===== 🔧 ACTUATOR CONTROL =====
void handleActuatorCommand(char cmd) {
  switch (cmd) {
    case '1':
      digitalWrite(ACTUATOR_IN1, HIGH);
      digitalWrite(ACTUATOR_IN2, LOW);
      analogWrite(ACTUATOR_ENA, config.actuator_speed);
      status.actuator_state = "opening";
      Serial.println(F("✅ Actuator Opening"));
      break;
    case '2':
      digitalWrite(ACTUATOR_IN1, LOW);
      digitalWrite(ACTUATOR_IN2, HIGH);
      analogWrite(ACTUATOR_ENA, config.actuator_speed);
      status.actuator_state = "closing";
      Serial.println(F("✅ Actuator Closing"));
      break;
    case '0':
      stopActuator();
      Serial.println(F("✅ Actuator STOP"));
      break;
  }
}

// ===== ⚖️ CALIBRATION =====
void handleCalibrationCommand(String cmd) {
  if (cmd.startsWith("WEIGHT:")) {
    float weight = cmd.substring(7).toFloat();
    if (weight > 0) {
      weightSensor.calibrate(weight);
    }
  } else if (cmd == "RESET") {
    weightSensor.resetCalibration();
  } else if (cmd == "TARE") {
    weightSensor.tare();
  }
}

// ===== ⚙️ CONFIGURATION =====
void handleConfigCommand(String cmd) {
  int colon = cmd.indexOf(':');
  if (colon == -1) return;
  
  String param = cmd.substring(0, colon);
  float value = cmd.substring(colon + 1).toFloat();
  
  if (param == "AUGER_SPEED" && value >= 0 && value <= 255) {
    config.auger_speed_forward = (uint8_t)value;
    Serial.print(F("✅ Auger speed: "));
    Serial.println(value);
    saveConfiguration();
  } else if (param == "TEMP_THRESHOLD" && value > 0 && value < 100) {
    config.temp_threshold = value;
    Serial.print(F("✅ Temp threshold: "));
    Serial.print(value, 1);
    Serial.println(F("°C"));
    saveConfiguration();
  } else if (param == "AUTO_FAN") {
    config.auto_fan_enabled = (value != 0);
    Serial.print(F("✅ Auto fan: "));
    Serial.println(config.auto_fan_enabled ? F("ON") : F("OFF"));
    saveConfiguration();
  }
}

// ===== 🍽️ FEEDING CONTROL =====
void handleFeedCommand(String cmd) {
  float amount = 0;
  
  if (cmd == "SMALL") amount = config.feed_small;
  else if (cmd == "MEDIUM") amount = config.feed_medium;
  else if (cmd == "LARGE") amount = config.feed_large;
  else amount = cmd.toFloat();
  
  if (amount > 0 && amount <= 1.0) {
    startFeeding(amount);
  }
}

void startFeeding(float amount) {
  if (status.is_feeding) {
    Serial.println(F("⚠️ Already feeding"));
    return;
  }
  
  status.is_feeding = true;
  status.feed_target = sensors.weight - amount;
  status.feed_start = millis();
  
  digitalWrite(AUGER_IN1, HIGH);
  digitalWrite(AUGER_IN2, LOW);
  analogWrite(AUGER_ENA, config.auger_speed_forward);
  status.auger_state = "feeding";
  
  Serial.print(F("🍽️ Feeding "));
  Serial.print(amount, 3);
  Serial.println(F(" kg"));
  Serial.print(F("   Target: "));
  Serial.print(status.feed_target, 3);
  Serial.println(F(" kg"));
}

void checkFeedingProgress() {
  // Timeout check (30 seconds)
  if (millis() - status.feed_start > 30000) {
    stopFeeding("timeout");
    return;
  }
  
  // Weight target reached
  if (sensors.weight <= status.feed_target) {
    stopFeeding("target_reached");
    return;
  }
}

void stopFeeding(String reason) {
  stopAuger();
  status.is_feeding = false;
  
  float fed = sensors.weight - status.feed_target;
  Serial.print(F("🍽️ Feeding stopped ("));
  Serial.print(reason);
  Serial.println(F(")"));
  Serial.print(F("   Fed: "));
  Serial.print(fed, 3);
  Serial.println(F(" kg"));
  Serial.print(F("   Time: "));
  Serial.print((millis() - status.feed_start) / 1000);
  Serial.println(F("s"));
}

// ===== 🌡️ AUTO FAN CONTROL =====
void checkAutoFan() {
  if (!config.auto_fan_enabled) return;
  
  float avgTemp = (sensors.feed_temp + sensors.control_temp) / 2.0;
  
  // Turn fan ON if temperature exceeds threshold
  if (!status.auto_fan_active && avgTemp > config.temp_threshold) {
    digitalWrite(RELAY_FAN, LOW);
    status.relay_fan = true;
    status.auto_fan_active = true;
    Serial.print(F("🌡️ Auto Fan ON ("));
    Serial.print(avgTemp, 1);
    Serial.println(F("°C)"));
  }
  // Turn fan OFF if temperature drops below threshold - hysteresis
  else if (status.auto_fan_active && avgTemp < (config.temp_threshold - config.temp_hysteresis)) {
    digitalWrite(RELAY_FAN, HIGH);
    status.relay_fan = false;
    status.auto_fan_active = false;
    Serial.print(F("🌡️ Auto Fan OFF ("));
    Serial.print(avgTemp, 1);
    Serial.println(F("°C)"));
  }
}

// ===== 🛑 MOTOR CONTROL =====
void stopAllMotors() {
  stopAuger();
  stopActuator();
  analogWrite(BLOWER_PIN, 0);
  status.blower_state = false;
}

void stopAuger() {
  digitalWrite(AUGER_IN1, LOW);
  digitalWrite(AUGER_IN2, LOW);
  analogWrite(AUGER_ENA, 0);
  status.auger_state = "stopped";
}

void stopActuator() {
  digitalWrite(ACTUATOR_IN1, LOW);
  digitalWrite(ACTUATOR_IN2, LOW);
  analogWrite(ACTUATOR_ENA, 0);
  status.actuator_state = "stopped";
} 