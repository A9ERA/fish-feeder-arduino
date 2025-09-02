#include "../../../include/soil_sensor.h"

void initSoil() {
  // ไม่ต้องตั้งค่า pinMode สำหรับ analogRead
  Serial.println("🌱 เริ่มระบบอ่านค่าความชื้นในดิน...");
}

StaticJsonDocument<256> readSoil() {
  StaticJsonDocument<256> doc;
  doc["name"] = SOIL_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  int soilRaw = analogRead(SOIL_PIN);
  
  // ใช้ค่าที่วัดจริง
  int DRY_ADC = 1023;  // แห้งสนิท
  int WET_ADC = 950;   // จุ่มน้ำเต็มที่

  int soilMoisture = map(soilRaw, DRY_ADC, WET_ADC, 0, 100);
  if (soilMoisture < 0) soilMoisture = 0;
  if (soilMoisture > 100) soilMoisture = 100;

  JsonObject moistureValue = values.createNestedObject();
  moistureValue["type"] = "soil_moisture";
  moistureValue["unit"] = "%";
  moistureValue["value"] = soilMoisture;

  return doc;
}
