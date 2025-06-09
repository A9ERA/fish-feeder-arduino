/**
 * ระบบให้อาหารปลาอัตโนมัติ IoT - Arduino Mega 2560 Firmware
 * 
 * ฟีเจอร์หลัก:
 * - อ่านเซ็นเซอร์: DHT22, DS18B20, HX711, Soil Moisture
 * - ควบคุม: Blower, Actuator, Auger, Relay
 * - รับคำสั่งจาก Pi ผ่าน Serial
 * - ส่งข้อมูลเซ็นเซอร์แบบ JSON
 * - ระบบ Auto Feed Sequence
 * 
 * พิน Assignment:
 * - DHT22 Control Box: Pin 48
 * - DHT22 Feed Container: Pin 46  
 * - DS18B20 Water Temp: Pin 44
 * - HX711 Load Cell: DT=20, SCK=21
 * - Soil Moisture: A2
 * - Blower: RPWM=5, LPWM=6
 * - Actuator: ENA=11, IN1=12, IN2=13
 * - Auger: ENA=8, IN1=9, IN2=10
 * - Relay: IN1=50, IN2=52
 * - Solar/Battery Monitor: A0-A4
 */

#include <Arduino.h>
#include <DHT.h>
#include <HX711.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// === พิน Definition ===
// เซ็นเซอร์
#define DHTPIN_BOX 48          // DHT22 ตู้ควบคุม
#define DHTPIN_FEED 46         // DHT22 ถังอาหาร
#define DHTTYPE DHT22
#define ONE_WIRE_BUS 44        // DS18B20 อุณหภูมิน้ำ
#define SOIL_PIN A2            // Soil Moisture Sensor
#define HX_DT 20               // HX711 Load Cell DT
#define HX_SCK 21              // HX711 Load Cell SCK

// พลังงาน Monitor
#define SOLAR_VOLTAGE_PIN A3
#define SOLAR_CURRENT_PIN A4
#define LOAD_VOLTAGE_PIN A1
#define LOAD_CURRENT_PIN A0

// อุปกรณ์ควบคุม
#define RPWM 5                 // Blower PWM ขวา
#define LPWM 6                 // Blower PWM ซ้าย
#define RELAY_IN1 50           // Relay 1
#define RELAY_IN2 52           // Relay 2
#define ACT_ENA 11             // Actuator Enable
#define ACT_IN1 12             // Actuator Direction 1
#define ACT_IN2 13             // Actuator Direction 2
#define AUG_ENA 8              // Auger Enable
#define AUG_IN1 9              // Auger Direction 1
#define AUG_IN2 10             // Auger Direction 2

// === ค่าคงที่ ===
#define SERIAL_BAUD 115200
#define JSON_BUFFER_SIZE 1024
#define SENSOR_SAMPLE_COUNT 50
#define HEARTBEAT_INTERVAL 5000
#define FEED_SEQUENCE_TIMEOUT 30000

// EEPROM Addresses
#define EEPROM_SCALE_ADDR 0
#define EEPROM_OFFSET_ADDR 4
#define EEPROM_SETTINGS_ADDR 8

// === เซ็นเซอร์ Objects ===
DHT dhtBox(DHTPIN_BOX, DHTTYPE);
DHT dhtFeed(DHTPIN_FEED, DHTTYPE);
HX711 scale;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);

// === ตัวแปรระบบ ===
struct SystemStatus {
  bool system_ready = false;
  unsigned long uptime = 0;
  unsigned long free_memory = 0;
  unsigned long last_heartbeat = 0;
  int error_count = 0;
  int command_count = 0;
  float command_success_rate = 100.0;
};

struct SensorData {
  float dht_box_temp = NAN;
  float dht_box_humidity = NAN;
  float dht_feed_temp = NAN;
  float dht_feed_humidity = NAN;
  float water_temperature = NAN;
  float feed_weight = NAN;
  float soil_moisture = NAN;
  float solar_voltage = 0.0;
  float solar_current = 0.0;
  float battery_voltage = 0.0;
  float load_current = 0.0;
  unsigned long last_updated = 0;
};

struct DeviceStatus {
  bool blower_running = false;
  int blower_speed = 0;
  String actuator_position = "stopped";
  String auger_direction = "stopped";
  bool relay1_state = false;
  bool relay2_state = false;
  bool feed_sequence_active = false;
  unsigned long feed_sequence_start = 0;
};

// Global instances
SystemStatus system_status;
SensorData sensor_data;
DeviceStatus device_status;

// Load Cell calibration
float scale_factor = 1.0;
long scale_offset = 0;

// === ฟังก์ชัน Utility ===
unsigned long getFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void sendHeartbeat() {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  doc["type"] = "heartbeat";
  doc["timestamp"] = millis();
  doc["system_ready"] = system_status.system_ready;
  doc["uptime"] = millis();
  doc["free_memory"] = getFreeMemory();
  doc["error_count"] = system_status.error_count;
  doc["command_success_rate"] = system_status.command_success_rate;
  
  Serial.println();
  serializeJson(doc, Serial);
  Serial.println();
}

void sendError(String message) {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  doc["type"] = "error";
  doc["message"] = message;
  doc["timestamp"] = millis();
  
  Serial.println();
  serializeJson(doc, Serial);
  Serial.println();
  
  system_status.error_count++;
}

void sendSuccess(String message, JsonObject data = JsonObject()) {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  doc["type"] = "success";
  doc["message"] = message;
  doc["timestamp"] = millis();
  
  if (!data.isNull()) {
    doc["data"] = data;
  }
  
  Serial.println();
  serializeJson(doc, Serial);
  Serial.println();
}

// === เซ็นเซอร์ Functions ===
void readSensors() {
  // อ่าน DHT22 sensors
  sensor_data.dht_box_temp = dhtBox.readTemperature();
  sensor_data.dht_box_humidity = dhtBox.readHumidity();
  sensor_data.dht_feed_temp = dhtFeed.readTemperature();
  sensor_data.dht_feed_humidity = dhtFeed.readHumidity();
  
  // อ่าน DS18B20 water temperature
  waterTemp.requestTemperatures();
  sensor_data.water_temperature = waterTemp.getTempCByIndex(0);
  if (sensor_data.water_temperature == -127.00) {
    sensor_data.water_temperature = NAN;
  }
  
  // อ่าน HX711 weight
  if (scale.is_ready()) {
    sensor_data.feed_weight = scale.get_units(10);
  }
  
  // อ่าน Soil Moisture
  int soil_raw = analogRead(SOIL_PIN);
  sensor_data.soil_moisture = map(soil_raw, 300, 1023, 100, 0);
  sensor_data.soil_moisture = constrain(sensor_data.soil_moisture, 0, 100);
  
  // อ่าน Power System (simplified)
  sensor_data.solar_voltage = (analogRead(SOLAR_VOLTAGE_PIN) * 5.0 / 1023.0) * 4.5;
  sensor_data.battery_voltage = (analogRead(LOAD_VOLTAGE_PIN) * 5.0 / 1023.0) * 4.5;
  sensor_data.solar_current = abs((analogRead(SOLAR_CURRENT_PIN) * 5.0 / 1023.0) - 2.5) / 0.066;
  sensor_data.load_current = abs((analogRead(LOAD_CURRENT_PIN) * 5.0 / 1023.0) - 2.5) / 0.066;
  
  sensor_data.last_updated = millis();
}

void sendSensorData() {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  
  doc["type"] = "sensor_data";
  doc["timestamp"] = millis();
  
  // Environmental sensors
  JsonObject env = doc.createNestedObject("environmental");
  env["dht22_control_box"]["temperature"] = sensor_data.dht_box_temp;
  env["dht22_control_box"]["humidity"] = sensor_data.dht_box_humidity;
  env["dht22_feed_container"]["temperature"] = sensor_data.dht_feed_temp;
  env["dht22_feed_container"]["humidity"] = sensor_data.dht_feed_humidity;
  env["ds18b20_water_temp"] = sensor_data.water_temperature;
  env["hx711_feed_weight"] = sensor_data.feed_weight;
  env["soil_moisture"] = sensor_data.soil_moisture;
  
  // Power system
  JsonObject power = doc.createNestedObject("power");
  power["solar_voltage"] = sensor_data.solar_voltage;
  power["solar_current"] = sensor_data.solar_current;
  power["battery_voltage"] = sensor_data.battery_voltage;
  power["load_current"] = sensor_data.load_current;
  
  // Device status
  JsonObject devices = doc.createNestedObject("devices");
  devices["blower_running"] = device_status.blower_running;
  devices["blower_speed"] = device_status.blower_speed;
  devices["actuator_position"] = device_status.actuator_position;
  devices["auger_direction"] = device_status.auger_direction;
  devices["relay1_state"] = device_status.relay1_state;
  devices["relay2_state"] = device_status.relay2_state;
  devices["feed_sequence_active"] = device_status.feed_sequence_active;
  
  Serial.println();
  serializeJson(doc, Serial);
  Serial.println();
}

// === Device Control Functions ===
void controlBlower(String action, int speed = 255) {
  if (action == "start") {
    analogWrite(RPWM, speed);
    digitalWrite(LPWM, LOW);
    device_status.blower_running = true;
    device_status.blower_speed = speed;
    sendSuccess("Blower started at speed " + String(speed));
  } else if (action == "stop") {
    analogWrite(RPWM, 0);
    analogWrite(LPWM, 0);
    device_status.blower_running = false;
    device_status.blower_speed = 0;
    sendSuccess("Blower stopped");
  } else {
    sendError("Invalid blower action: " + action);
  }
}

void controlActuator(String action) {
  if (action == "up") {
    digitalWrite(ACT_IN1, HIGH);
    digitalWrite(ACT_IN2, LOW);
    analogWrite(ACT_ENA, 255);
    device_status.actuator_position = "extending";
    sendSuccess("Actuator extending");
  } else if (action == "down") {
    digitalWrite(ACT_IN1, LOW);
    digitalWrite(ACT_IN2, HIGH);
    analogWrite(ACT_ENA, 255);
    device_status.actuator_position = "retracting";
    sendSuccess("Actuator retracting");
  } else if (action == "stop") {
    analogWrite(ACT_ENA, 0);
    device_status.actuator_position = "stopped";
    sendSuccess("Actuator stopped");
  } else {
    sendError("Invalid actuator action: " + action);
  }
}

void controlAuger(String action, int speed = 200) {
  if (action == "forward") {
    digitalWrite(AUG_IN1, HIGH);
    digitalWrite(AUG_IN2, LOW);
    analogWrite(AUG_ENA, speed);
    device_status.auger_direction = "forward";
    sendSuccess("Auger rotating forward");
  } else if (action == "reverse") {
    digitalWrite(AUG_IN1, LOW);
    digitalWrite(AUG_IN2, HIGH);
    analogWrite(AUG_ENA, speed);
    device_status.auger_direction = "reverse";
    sendSuccess("Auger rotating reverse");
  } else if (action == "stop") {
    analogWrite(AUG_ENA, 0);
    device_status.auger_direction = "stopped";
    sendSuccess("Auger stopped");
  } else {
    sendError("Invalid auger action: " + action);
  }
}

void controlRelay(int relay_id, String action) {
  int pin = (relay_id == 1) ? RELAY_IN1 : RELAY_IN2;
  bool* state = (relay_id == 1) ? &device_status.relay1_state : &device_status.relay2_state;
  
  if (action == "on") {
    digitalWrite(pin, LOW); // Active LOW relay
    *state = true;
    sendSuccess("Relay " + String(relay_id) + " ON");
  } else if (action == "off") {
    digitalWrite(pin, HIGH);
    *state = false;
    sendSuccess("Relay " + String(relay_id) + " OFF");
  } else {
    sendError("Invalid relay action: " + action);
  }
}

// === Feed Sequence ===
void executeAutoFeedSequence() {
  device_status.feed_sequence_active = true;
  device_status.feed_sequence_start = millis();
  
  sendSuccess("Starting auto feed sequence");
  
  // Step 1: Actuator up (3 seconds)
  controlActuator("up");
  delay(3000);
  controlActuator("stop");
  
  // Step 2: Auger forward (6 seconds)
  controlAuger("forward");
  delay(6000);
  controlAuger("stop");
  
  // Step 3: Blower start (5 seconds)
  controlBlower("start");
  delay(5000);
  controlBlower("stop");
  
  // Step 4: Actuator down
  controlActuator("down");
  delay(3000);
  controlActuator("stop");
  
  device_status.feed_sequence_active = false;
  sendSuccess("Auto feed sequence completed");
}

// === Load Cell Calibration ===
void calibrateLoadCell(float known_weight) {
  if (known_weight <= 0) {
    sendError("Invalid calibration weight");
    return;
  }
  
  scale.set_scale();
  scale.tare();
  
  delay(1000);
  
  long reading = scale.get_value(20);
  if (reading == 0) {
    sendError("No reading from load cell");
    return;
  }
  
  scale_factor = reading / known_weight;
  scale_offset = scale.get_offset();
  
  // Save to EEPROM
  EEPROM.put(EEPROM_SCALE_ADDR, scale_factor);
  EEPROM.put(EEPROM_OFFSET_ADDR, scale_offset);
  
  scale.set_scale(scale_factor);
  scale.set_offset(scale_offset);
  
  StaticJsonDocument<JSON_BUFFER_SIZE> response;
  response["scale_factor"] = scale_factor;
  response["offset"] = scale_offset;
  
  sendSuccess("Load cell calibrated successfully", response.as<JsonObject>());
}

// === Serial Command Processing ===
void processSerialCommand() {
  if (!Serial.available()) return;
  
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  if (input.length() == 0) return;
  
  system_status.command_count++;
  
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    sendError("Invalid JSON command: " + String(error.c_str()));
    return;
  }
  
  String command = doc["command"];
  
  if (command == "get_sensors") {
    readSensors();
    sendSensorData();
  }
  else if (command == "get_status") {
    sendHeartbeat();
  }
  else if (command == "control_blower") {
    String action = doc["action"];
    int speed = doc["speed"] | 255;
    controlBlower(action, speed);
  }
  else if (command == "control_actuator") {
    String action = doc["action"];
    controlActuator(action);
  }
  else if (command == "control_auger") {
    String action = doc["action"];
    int speed = doc["speed"] | 200;
    controlAuger(action, speed);
  }
  else if (command == "control_relay") {
    int relay_id = doc["relay_id"];
    String action = doc["action"];
    controlRelay(relay_id, action);
  }
  else if (command == "auto_feed") {
    executeAutoFeedSequence();
  }
  else if (command == "calibrate_scale") {
    float weight = doc["weight"];
    calibrateLoadCell(weight);
  }
  else if (command == "emergency_stop") {
    // Stop all devices
    controlBlower("stop");
    controlActuator("stop");
    controlAuger("stop");
    controlRelay(1, "off");
    controlRelay(2, "off");
    device_status.feed_sequence_active = false;
    sendSuccess("Emergency stop executed");
  }
  else {
    sendError("Unknown command: " + command);
  }
}

// === Setup ===
void setup() {
  Serial.begin(SERIAL_BAUD);
  
  // Initialize sensors
  dhtBox.begin();
  dhtFeed.begin();
  waterTemp.begin();
  scale.begin(HX_DT, HX_SCK);
  
  // Load calibration from EEPROM
  EEPROM.get(EEPROM_SCALE_ADDR, scale_factor);
  EEPROM.get(EEPROM_OFFSET_ADDR, scale_offset);
  
  if (isnan(scale_factor) || scale_factor == 0) {
    scale_factor = 1.0;
  }
  
  scale.set_scale(scale_factor);
  scale.set_offset(scale_offset);
  
  // Initialize control pins
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(RELAY_IN1, OUTPUT);
  pinMode(RELAY_IN2, OUTPUT);
  pinMode(ACT_ENA, OUTPUT);
  pinMode(ACT_IN1, OUTPUT);
  pinMode(ACT_IN2, OUTPUT);
  pinMode(AUG_ENA, OUTPUT);
  pinMode(AUG_IN1, OUTPUT);
  pinMode(AUG_IN2, OUTPUT);
  
  // Set initial states
  digitalWrite(RELAY_IN1, HIGH); // Active LOW relay
  digitalWrite(RELAY_IN2, HIGH);
  analogWrite(ACT_ENA, 0);
  analogWrite(AUG_ENA, 0);
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
  
  // System ready
  system_status.system_ready = true;
  system_status.last_heartbeat = millis();
  
  // Send startup message
  sendSuccess("Fish Feeder Arduino Mega 2560 initialized and ready");
}

// === Main Loop ===
void loop() {
  unsigned long current_time = millis();
  
  // Process serial commands
  processSerialCommand();
  
  // Send heartbeat every 5 seconds
  if (current_time - system_status.last_heartbeat >= HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    system_status.last_heartbeat = current_time;
  }
  
  // Read and send sensor data every 10 seconds
  static unsigned long last_sensor_read = 0;
  if (current_time - last_sensor_read >= 10000) {
    readSensors();
    sendSensorData();
    last_sensor_read = current_time;
  }
  
  // Feed sequence timeout check
  if (device_status.feed_sequence_active) {
    if (current_time - device_status.feed_sequence_start >= FEED_SEQUENCE_TIMEOUT) {
      device_status.feed_sequence_active = false;
      sendError("Feed sequence timeout - stopped");
    }
  }
  
  delay(100); // Small delay to prevent overwhelming the serial
} 