#include "../../../include/acs712_sensor.h"

const int sensitivity = 66;     // mV per A
const int voltageOffset = 2490; // ปรับชดเชยใหม่

void initACS712() {
  Serial.println("⚡ เริ่มวัดกระแสไฟฟ้า...");
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
  int raw = analogRead(A0);
  double voltage_mV = (raw / 1024.0) * 5000.0;
  return (voltage_mV - voltageOffset) / sensitivity;
}

StaticJsonDocument<256> readACS712() {
  StaticJsonDocument<256> doc;
  doc["name"] = ACS712_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  double current = readAverageCurrent();

  Serial.print("⚡ Current: ");
  Serial.print(current, 3);
  Serial.println(" A");

  JsonObject currentValue = values.createNestedObject();
  currentValue["type"] = "current";
  currentValue["unit"] = "A";
  currentValue["value"] = current;

  return doc;
}