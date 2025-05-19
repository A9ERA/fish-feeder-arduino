#include "../../../include/dht_sensor.h"

// Create DHT sensor objects
DHT dht1(DHTPIN1, DHTTYPE);  // System DHT22
DHT dht2(DHTPIN2, DHTTYPE);  // Feeder DHT22


void initDHT() {
  dht1.begin();
  dht2.begin();
  Serial.println("📡 เริ่มอ่านค่า DHT22 ที่ขา 48 และ 46...");
}

StaticJsonDocument<256> readDHTSystem() {
  StaticJsonDocument<256> doc;
  doc["name"] = DHT22_SYSTEM;
  JsonArray values = doc.createNestedArray("value");

  float temp = dht1.readTemperature();
  float hum = dht1.readHumidity();

  Serial.print("📍 DHT1 (ขา 48) - 🌡️ Temp: ");
  if (isnan(temp)) {
    Serial.print("NaN");
    temp = 0;
  } else {
    Serial.print(temp);
  }
  Serial.print(" °C\t💧 Humidity: ");
  if (isnan(hum)) {
    Serial.print("NaN");
    hum = 0;
  } else {
    Serial.print(hum);
  }
  Serial.println(" %");

  JsonObject tempValue = values.createNestedObject();
  tempValue["type"] = "temperature";
  tempValue["unit"] = "C";
  tempValue["value"] = temp;

  JsonObject humValue = values.createNestedObject();
  humValue["type"] = "humidity";
  humValue["unit"] = "%";
  humValue["value"] = hum;

  return doc;
}

StaticJsonDocument<256> readDHTFeeder() {
  StaticJsonDocument<256> doc;
  doc["name"] = DHT22_FEEDER;
  JsonArray values = doc.createNestedArray("value");

  float temp = dht2.readTemperature();
  float hum = dht2.readHumidity();

  Serial.print("📍 DHT2 (ขา 46) - 🌡️ Temp: ");
  if (isnan(temp)) {
    Serial.print("NaN");
    temp = 0;
  } else {
    Serial.print(temp);
  }
  Serial.print(" °C\t💧 Humidity: ");
  if (isnan(hum)) {
    Serial.print("NaN");
    hum = 0;
  } else {
    Serial.print(hum);
  }
  Serial.println(" %");

  JsonObject tempValue = values.createNestedObject();
  tempValue["type"] = "temperature";
  tempValue["unit"] = "C";
  tempValue["value"] = temp;

  JsonObject humValue = values.createNestedObject();
  humValue["type"] = "humidity";
  humValue["unit"] = "%";
  humValue["value"] = hum;

  return doc;
}
