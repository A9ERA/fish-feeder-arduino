#include "../../../include/acs712_sensor.h"

const float sensitivity = 0.185; // V/A for ACS712-5A
const float vRef = 5.0;         // Reference voltage
const int numSamples = 500;     // Number of samples for averaging

void initACS712() {
  Serial.println("⚡ เริ่มวัดกระแสไฟฟ้า...");
}

StaticJsonDocument<256> readACS712() {
  StaticJsonDocument<256> doc;
  doc["name"] = ACS712_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  // Read multiple samples and average
  float totalCurrent = 0;
  for (int i = 0; i < numSamples; i++) {
    int raw = analogRead(ACS712_PIN);
    float voltage = (raw / 1023.0) * vRef;
    float current = (voltage - (vRef / 2.0)) / sensitivity;
    totalCurrent += current;
    delayMicroseconds(100); // Small delay between readings
  }
  float avgCurrent = totalCurrent / numSamples;

  // Ensure non-negative current readings
  if (avgCurrent < 0) avgCurrent = 0;

  Serial.print("⚡ Current: ");
  Serial.print(avgCurrent, 3);
  Serial.println(" A");

  JsonObject currentValue = values.createNestedObject();
  currentValue["type"] = "current";
  currentValue["unit"] = "A";
  currentValue["value"] = avgCurrent;

  return doc;
} 