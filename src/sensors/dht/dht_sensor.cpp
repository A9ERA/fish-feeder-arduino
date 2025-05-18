#include "../../../include/dht_sensor.h"

// Create DHT sensor objects
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

void initDHT() {
  dht1.begin();
  dht2.begin();
  Serial.println("📡 เริ่มอ่านค่า DHT22 ที่ขา 48 และ 46...");
}

void readDHT() {
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();

  Serial.print("📍 DHT1 (ขา 48) - 🌡️ Temp: ");
  if (isnan(temp1)) {
    Serial.print("NaN");
  } else {
    Serial.print(temp1);
  }
  Serial.print(" °C\t💧 Humidity: ");
  if (isnan(hum1)) {
    Serial.print("NaN");
  } else {
    Serial.print(hum1);
  }
  Serial.println(" %");

  Serial.print("📍 DHT2 (ขา 46) - 🌡️ Temp: ");
  if (isnan(temp2)) {
    Serial.print("NaN");
  } else {
    Serial.print(temp2);
  }
  Serial.print(" °C\t💧 Humidity: ");
  if (isnan(hum2)) {
    Serial.print("NaN");
  } else {
    Serial.print(hum2);
  }
  Serial.println(" %");
} 