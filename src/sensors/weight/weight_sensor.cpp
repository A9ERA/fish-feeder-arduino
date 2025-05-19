#include "../../../include/weight_sensor.h"

HX711 scale;
float scaleFactor = 1.0;
long offset = 0;

void initWeight() {
  Serial.println("📦 เริ่มต้นระบบชั่งน้ำหนัก...");

  EEPROM.get(EEPROM_SCALE_ADDR, scaleFactor);
  EEPROM.get(EEPROM_OFFSET_ADDR, offset);

  Serial.print("📥 scaleFactor: ");
  Serial.println(scaleFactor, 6);
  Serial.print("📥 offset: ");
  Serial.println(offset);

  if (scaleFactor < 1.0 || scaleFactor > 100000.0) {
    Serial.println("⚠️ ค่า scaleFactor ผิดปกติ กรุณา calibrate ใหม่");
    while (true);
  }

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(scaleFactor);
  scale.set_offset(offset);

  Serial.println("📌 เริ่มวัดน้ำหนัก...");
}

StaticJsonDocument<256> readWeight() {
  StaticJsonDocument<256> doc;
  doc["name"] = WEIGHT_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  float weight = scale.get_units(10);
  Serial.print("⚖️ น้ำหนัก foo: ");
  Serial.print(weight, 3);
  Serial.println(" kg");

  JsonObject weightValue = values.createNestedObject();
  weightValue["type"] = "weight";
  weightValue["unit"] = "kg";
  weightValue["value"] = weight;

  return doc;
}