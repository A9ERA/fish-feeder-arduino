#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <Arduino.h>

// ===== 💾 EEPROM ADDRESSES =====
#define EEPROM_SCALE_ADDR 0
#define EEPROM_OFFSET_ADDR 4
#define EEPROM_CONFIG_ADDR 8

// ===== ⚙️ SYSTEM CONFIGURATION =====
struct Config {
  uint8_t auger_speed_forward = 200;
  uint8_t auger_speed_backward = 180;
  uint8_t blower_speed = 255;
  uint8_t actuator_speed = 220;
  float temp_threshold = 30.0;
  float temp_hysteresis = 2.0;
  uint16_t sensor_interval = 5000;
  uint16_t output_interval = 10000;
  bool auto_fan_enabled = true;
  float feed_small = 0.05;
  float feed_medium = 0.10;
  float feed_large = 0.20;
  uint8_t version = 1;
};

// ===== 📊 SENSOR DATA =====
struct SensorData {
  float feed_temp = 0;
  float feed_humidity = 0;
  float control_temp = 0;
  float control_humidity = 0;
  float water_temp = 0;
  float soil_moisture = 0;
  float weight = 0;
  float battery_voltage = 0;
  float solar_current = 0;
  bool errors[7] = {false};
  unsigned long last_update = 0;
};

// ===== 🔧 SYSTEM STATUS =====
struct SystemStatus {
  bool relay_led = false;
  bool relay_fan = false;
  String auger_state = "stopped";
  bool blower_state = false;
  String actuator_state = "stopped";
  bool auto_fan_active = false;
  float feed_target = 0;
  unsigned long feed_start = 0;
  bool is_feeding = false;
  bool calibration_mode = false;
};

// ===== 📍 HARDWARE PINS =====
// Sensors
#define DHT_FEED_PIN 46
#define DHT_CONTROL_PIN 48
#define DS18B20_PIN 22
#define SOIL_PIN A0
#define HX711_DOUT_PIN 20
#define HX711_SCK_PIN 21
#define CURRENT_PIN A1
#define VOLTAGE_PIN A2

// Controls
#define RELAY_LED 50
#define RELAY_FAN 52
#define AUGER_ENA 8
#define AUGER_IN1 9
#define AUGER_IN2 10
#define BLOWER_PIN 11
#define ACTUATOR_ENA 6
#define ACTUATOR_IN1 7
#define ACTUATOR_IN2 12

// ===== 🎛️ GLOBAL INSTANCES =====
extern Config config;
extern SensorData sensors;
extern SystemStatus status;

#endif // SENSOR_DATA_H 