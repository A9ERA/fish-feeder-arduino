#include "../../../include/solar_battery_sensor.h"

void initSolarBattery() {
  // No special initialization needed for analog pins
  Serial.println("☀️🔋 เริ่มต้นระบบตรวจสอบโซลาร์เซลล์และแบตเตอรี่...");
}

float readVoltage(int pin) {
  int adcValue = analogRead(pin);
  // Convert ADC reading to voltage considering voltage divider
  float voltage = (adcValue / ADC_RESOLUTION) * ADC_REFERENCE_VOLTAGE * VOLTAGE_DIVIDER_RATIO;
  return voltage;
}

float readCurrent(int pin) {
  int adcValue = analogRead(pin);
  float sensorVoltage = (adcValue / ADC_RESOLUTION) * ADC_REFERENCE_VOLTAGE;
  
  // Calculate current using ACS712 formula
  float current = (sensorVoltage - ACS712_ZERO_CURRENT) / ACS712_SENSITIVITY;
  
  // Return absolute value (assuming we want magnitude)
  return abs(current);
}

float calculateBatteryPercentage(float voltage) {
  if (voltage >= BATTERY_MAX_VOLTAGE) return 100.0;
  if (voltage <= BATTERY_MIN_VOLTAGE) return 0.0;
  
  // Linear interpolation between min and max voltage
  float percentage = ((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100.0;
  return constrain(percentage, 0.0, 100.0);
}

String getBatteryStatus(float voltage) {
  if (voltage >= 13.0) return "charging";
  if (voltage >= BATTERY_LOW_THRESHOLD) return "normal";
  if (voltage >= BATTERY_MIN_VOLTAGE) return "low";
  return "critical";
}

float calculatePower(float voltage, float current) {
  return voltage * current;
}

StaticJsonDocument<256> readSolarStatus() {
  StaticJsonDocument<256> doc;
  doc["name"] = SOLAR_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  // อ่านแรงดันโซลาร์เซลล์
  float solarVoltage = readVoltage(SOLAR_VOLTAGE_PIN);
  
  // อ่านกระแสโซลาร์เซลล์
  float solarCurrent = readCurrent(SOLAR_CURRENT_PIN);
  
  // คำนวณกำลังไฟฟ้า
  float solarPower = calculatePower(solarVoltage, solarCurrent);

  Serial.print("☀️ Solar Panel - Voltage: ");
  Serial.print(solarVoltage, 2);
  Serial.print(" V, Current: ");
  Serial.print(solarCurrent, 3);
  Serial.print(" A, Power: ");
  Serial.print(solarPower, 2);
  Serial.println(" W");

  // เพิ่มข้อมูลแรงดัน
  JsonObject voltageValue = values.createNestedObject();
  voltageValue["type"] = "voltage";
  voltageValue["unit"] = "V";
  voltageValue["value"] = solarVoltage;

  // เพิ่มข้อมูลกระแส
  JsonObject currentValue = values.createNestedObject();
  currentValue["type"] = "current";
  currentValue["unit"] = "A";
  currentValue["value"] = solarCurrent;

  // เพิ่มข้อมูลกำลังไฟฟ้า
  JsonObject powerValue = values.createNestedObject();
  powerValue["type"] = "power";
  powerValue["unit"] = "W";
  powerValue["value"] = solarPower;

  return doc;
}

StaticJsonDocument<256> readBatteryStatus() {
  StaticJsonDocument<256> doc;
  doc["name"] = BATTERY_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  // อ่านแรงดันแบตเตอรี่
  float batteryVoltage = readVoltage(BATTERY_VOLTAGE_PIN);
  
  // คำนวณเปอร์เซ็นต์แบตเตอรี่
  float batteryPercentage = calculateBatteryPercentage(batteryVoltage);
  
  // ตรวจสอบสถานะแบตเตอรี่
  String batteryStatus = getBatteryStatus(batteryVoltage);

  Serial.print("🔋 Battery - Voltage: ");
  Serial.print(batteryVoltage, 2);
  Serial.print(" V, Percentage: ");
  Serial.print(batteryPercentage, 1);
  Serial.print(" %, Status: ");
  Serial.println(batteryStatus);

  // เพิ่มข้อมูลแรงดัน
  JsonObject voltageValue = values.createNestedObject();
  voltageValue["type"] = "voltage";
  voltageValue["unit"] = "V";
  voltageValue["value"] = batteryVoltage;

  // เพิ่มข้อมูลเปอร์เซ็นต์
  JsonObject percentageValue = values.createNestedObject();
  percentageValue["type"] = "percentage";
  percentageValue["unit"] = "%";
  percentageValue["value"] = batteryPercentage;

  // เพิ่มข้อมูลสถานะ
  JsonObject statusValue = values.createNestedObject();
  statusValue["type"] = "status";
  statusValue["unit"] = "state";
  statusValue["value"] = batteryStatus;

  return doc;
}

StaticJsonDocument<256> readLoadStatus() {
  StaticJsonDocument<256> doc;
  doc["name"] = LOAD_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  // อ่านแรงดันแบตเตอรี่ (สำหรับคำนวณกำลังโหลด)
  float loadVoltage = readVoltage(BATTERY_VOLTAGE_PIN);
  
  // อ่านกระแสโหลด
  float loadCurrent = readCurrent(LOAD_CURRENT_PIN);
  
  // คำนวณกำลังโหลด
  float loadPower = calculatePower(loadVoltage, loadCurrent);

  Serial.print("⚡ Load - Voltage: ");
  Serial.print(loadVoltage, 2);
  Serial.print(" V, Current: ");
  Serial.print(loadCurrent, 3);
  Serial.print(" A, Power: ");
  Serial.print(loadPower, 2);
  Serial.println(" W");

  // เพิ่มข้อมูลแรงดัน
  JsonObject voltageValue = values.createNestedObject();
  voltageValue["type"] = "voltage";
  voltageValue["unit"] = "V";
  voltageValue["value"] = loadVoltage;

  // เพิ่มข้อมูลกระแส
  JsonObject currentValue = values.createNestedObject();
  currentValue["type"] = "current";
  currentValue["unit"] = "A";
  currentValue["value"] = loadCurrent;

  // เพิ่มข้อมูลกำลังไฟฟ้า
  JsonObject powerValue = values.createNestedObject();
  powerValue["type"] = "power";
  powerValue["unit"] = "W";
  powerValue["value"] = loadPower;

  return doc;
} 