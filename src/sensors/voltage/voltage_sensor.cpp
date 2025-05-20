#include "../../../include/voltage_sensor.h"

const float correctionFactor = 11.98 / 12.95;  // ประมาณ 0.925

void initVoltageSensor() {
  Serial.println("🔍 เริ่มวัดแรงดันไฟฟ้า...");
}

void readVoltageSensor() {
  int raw = analogRead(VOLTAGE_PIN);
  float voltage = (raw / 1023.0) * 5.0 * 5.0 * correctionFactor;

  Serial.print("📟 Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");
}