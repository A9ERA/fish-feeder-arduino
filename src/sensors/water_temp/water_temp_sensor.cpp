#include "../../../include/water_temp_sensor.h"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void initWaterTemp() {
  sensors.begin();
  Serial.println("📌 เริ่มต้นระบบวัดอุณหภูมิ DS18B20...");
}

void readWaterTemp() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == -127.00) {
    Serial.println("❌ ไม่พบเซ็นเซอร์ หรือเชื่อมต่อผิด");
  } else {
    Serial.print("🌊 DS18B20 - 🌡️ อุณหภูมิ: ");
    Serial.print(tempC);
    Serial.println(" °C");
  }
}