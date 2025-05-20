#include "../../../include/acs712_sensor.h"

// === กำหนดค่าความไวของเซ็นเซอร์กระแส (sensitivity) ===
// ใช้สำหรับ ACS712 รุ่น 30A = 66 mV/A
const int sensitivity = 66;

// === ค่าชดเชยแรงดันศูนย์ (offset) ===
// ค่านี้คำนวณจากค่าจริงที่อ่านได้ตอนกระแส 0.073A
const int voltageOffset = 2488;

void initACS712() {
  Serial.println("🔍 เริ่มวัดกระแสด้วย ACS712...");
}

double readAverageCurrent() {
  const int sampleCount = 200;
  double sum = 0;
  for (int i = 0; i < sampleCount; i++) {
    sum += readCurrentOnce();
  }
  return sum / sampleCount;
}

double readCurrentOnce() {
  int raw = analogRead(ACS712_PIN);  // อ่านค่าจากขา ACS712_PIN
  double voltage_mV = (raw / 1024.0) * 5000.0; // แปลงเป็น mV
  double current = (voltage_mV - voltageOffset) / sensitivity;
  return current;
}

void readACS712() {
  double current = readAverageCurrent();
  Serial.print("⚡ Current: ");
  Serial.print(current, 3); // แสดง 3 ตำแหน่งทศนิยม
  Serial.println(" A");
} 