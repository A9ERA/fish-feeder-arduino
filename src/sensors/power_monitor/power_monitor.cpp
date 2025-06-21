#include "../../../include/power_monitor.h"

// Global variables
float currentBatteryPercent = 0.0;
String batteryStatusText = "";

void initPowerMonitor() {
  Serial.println("⚡ เริ่มต้นระบบมอนิเตอร์พลังงาน...");
}

// === ฟังก์ชันประเมินเปอร์เซ็นต์แบตเตอรี่จากแรงดัน (Lithium-ion 12V 12AH) ===
float estimateBatteryPercentage(float voltage) {
  // ⚡ LITHIUM-ION 12V 12AH BATTERY SPECIFICATIONS:
  // • แรงดันการใช้งาน: 8.4-12.6V
  // • แรงดันในการชาร์จ: 12.6V  
  // • มีบอร์ดบาลานซ์ในตัว
  // • ความจุ: 12AH
  
  const float minV = 8.4;    // 0% - ตามสเปคแรงดันต่ำสุด
  const float maxV = 12.6;   // 100% - ตามสเปคแรงดันชาร์จเต็ม
  
  if (voltage >= maxV) return 100.0;
  if (voltage <= minV) return 0.0;
  
  // ⚡ LITHIUM-ION 12V 12AH DISCHARGE CURVE (ตามสเปคจริง):
  float percent = 0.0;
  
  if (voltage >= 12.4) {
    // 12.4-12.6V = 90-100% (เต็มเกือบสุด)
    percent = 90.0 + ((voltage - 12.4) / 0.2) * 10.0;
  } else if (voltage >= 12.0) {
    // 12.0-12.4V = 70-90% (ช่วงบน)
    percent = 70.0 + ((voltage - 12.0) / 0.4) * 20.0;
  } else if (voltage >= 11.5) {
    // 11.5-12.0V = 40-70% (ช่วงกลาง)
    percent = 40.0 + ((voltage - 11.5) / 0.5) * 30.0;
  } else if (voltage >= 10.5) {
    // 10.5-11.5V = 15-40% (ช่วงล่าง)
    percent = 15.0 + ((voltage - 10.5) / 1.0) * 25.0;
  } else if (voltage >= 9.0) {
    // 9.0-10.5V = 5-15% (ช่วงต่ำ)
    percent = 5.0 + ((voltage - 9.0) / 1.5) * 10.0;
  } else {
    // 8.4-9.0V = 0-5% (ช่วงวิกฤต - ใกล้หมด)
    percent = ((voltage - 8.4) / 0.6) * 5.0;
  }
  
  return constrain(percent, 0.0, 100.0);
}

// === อ่านค่าแรงดัน/กระแสแบบเฉลี่ยจากแต่ละเซ็นเซอร์ ===
void readSensors(float& solarV, float& solarI, float& loadV, float& loadI) {
  const int sampleCount = 150;
  long sumVS = 0, sumIS = 0;
  long sumVL = 0, sumIL = 0;

  for (int i = 0; i < sampleCount; i++) {
    sumVS += analogRead(SOLAR_VOLTAGE_PIN);
    sumIS += analogRead(SOLAR_CURRENT_PIN);
    sumVL += analogRead(LOAD_VOLTAGE_PIN);
    sumIL += analogRead(LOAD_CURRENT_PIN);
  }

  solarV = (sumVS / (float)sampleCount / 1023.0) * V_REF * V_FACTOR;
  loadV  = (sumVL / (float)sampleCount / 1023.0) * V_REF * V_FACTOR;

  solarI = (((sumIS / (float)sampleCount) / 1023.0) * V_REF - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;
  loadI  = (((sumIL / (float)sampleCount) / 1023.0) * V_REF - ZERO_CURRENT_VOLTAGE) / SENSITIVITY;

  if (solarV < 1.0) solarV = 0.0;
  if (abs(solarI) < 0.50 || solarV < 1.0) solarI = 0.0;
  if (loadI < 0.0) loadI = -loadI;
}

// === ตรวจสอบว่าแผงโซลาร์กำลังชาร์จแบตเตอรี่อยู่หรือไม่ ===
bool isCharging(float solarV, float solarI) {
  // ⚡ เงื่อนไขการชาร์จ (ตามความต้องการผู้ใช้):
  // - แสดง "กำลังชาร์จ..." เมื่อ Solar Voltage มีค่า (> 5V)
  // - เพื่อป้องกันการแสดงเปอร์เซ็นต์ที่ผิดพลาดขณะชาร์จ
  
  return (solarV > 5.0);  // มีแรงดันโซลาร์ = กำลังชาร์จ
}

// === ฟังก์ชันหลักสำหรับอ่านค่าและส่งคืน JSON ===
StaticJsonDocument<1024> readPowerMonitor() {
  StaticJsonDocument<1024> doc;
  doc["name"] = POWER_MONITOR;
  JsonArray values = doc.createNestedArray("value");

  // อ่านค่าจากเซ็นเซอร์ทั้งหมด
  float solarV, solarI, loadV, loadI;
  readSensors(solarV, solarI, loadV, loadI);

  // อัปเดตสถานะแบตเตอรี่
  bool charging = isCharging(solarV, solarI);
  float batteryPercent = 0.0;
  String batteryStatus = "";
  
  if (charging) {
    batteryStatus = "charging";
    batteryPercent = -1.0; // ใช้ -1 แทน % เมื่อกำลังชาร์จ
  } else {
    batteryPercent = estimateBatteryPercentage(loadV);
    batteryStatus = "discharging";
  }

  // สร้าง JSON objects สำหรับแต่ละค่า
  
  // Solar Voltage
  JsonObject solarVoltageObj = values.createNestedObject();
  solarVoltageObj["type"] = "solarVoltage";
  solarVoltageObj["unit"] = "V";
  solarVoltageObj["value"] = solarV;

  // Solar Current
  JsonObject solarCurrentObj = values.createNestedObject();
  solarCurrentObj["type"] = "solarCurrent";
  solarCurrentObj["unit"] = "A";
  solarCurrentObj["value"] = solarI;

  // Load Voltage
  JsonObject loadVoltageObj = values.createNestedObject();
  loadVoltageObj["type"] = "loadVoltage";
  loadVoltageObj["unit"] = "V";
  loadVoltageObj["value"] = loadV;

  // Load Current
  JsonObject loadCurrentObj = values.createNestedObject();
  loadCurrentObj["type"] = "loadCurrent";
  loadCurrentObj["unit"] = "A";
  loadCurrentObj["value"] = loadI;

  // Battery Voltage (same as load voltage)
  JsonObject batteryVoltageObj = values.createNestedObject();
  batteryVoltageObj["type"] = "batteryVoltage";
  batteryVoltageObj["unit"] = "V";
  batteryVoltageObj["value"] = loadV;

  // Battery Percentage
  JsonObject batteryPercentageObj = values.createNestedObject();
  batteryPercentageObj["type"] = "batteryPercentage";
  batteryPercentageObj["unit"] = "%";
  batteryPercentageObj["value"] = batteryPercent;

  // Battery Status
  JsonObject batteryStatusObj = values.createNestedObject();
  batteryStatusObj["type"] = "batteryStatus";
  batteryStatusObj["unit"] = "string";
  batteryStatusObj["value"] = batteryStatus;

  // Print debug information
  Serial.print("⚡ Solar: "); Serial.print(solarV, 1); Serial.print("V, "); Serial.print(solarI, 3); Serial.print("A");
  Serial.print(" | Load: "); Serial.print(loadV, 1); Serial.print("V, "); Serial.print(loadI, 3); Serial.print("A");
  Serial.print(" | Battery: "); Serial.print(batteryPercent, 1); Serial.print("% ("); Serial.print(batteryStatus); Serial.println(")");

  return doc;
} 