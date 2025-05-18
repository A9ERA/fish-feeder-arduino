#include "../../../include/soil_sensor.h"

void initSoil() {
  // ไม่ต้องตั้งค่า pinMode สำหรับ analogRead
  Serial.println("🌱 เริ่มระบบอ่านค่าความชื้นในดิน...");
}

void readSoil() {
  int soilRaw = analogRead(SOIL_PIN);
  float soilMoisture = map(soilRaw, 300, 1023, 100, 0);  // Raw ต่ำ = ชื้นมาก
  soilMoisture = constrain(soilMoisture, 0, 100);

  Serial.print("📟 Raw Value: ");
  Serial.print(soilRaw);
  Serial.print("  🌱 Soil Moisture: ");
  Serial.print(soilMoisture, 0);
  Serial.println(" %");
}