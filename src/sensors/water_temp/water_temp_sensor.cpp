#include "water_temp_sensor.h"

WaterTempSensor::WaterTempSensor(uint8_t pin) : pin(pin) {
    oneWire = new OneWire(pin);
    sensors = new DallasTemperature(oneWire);
}

WaterTempSensor::~WaterTempSensor() {
    delete sensors;
    delete oneWire;
}

void WaterTempSensor::begin() {
    sensors->begin();
    delay(100);
    Serial.println("🌊 Water DS18B20 temperature sensor initialized");
}

bool WaterTempSensor::readTemperature(float& temperature) {
    sensors->requestTemperatures();
    delay(10);
    temperature = sensors->getTempCByIndex(0);
    return isValidReading(temperature);
}

bool WaterTempSensor::isValidReading(float value) {
    return value != DEVICE_DISCONNECTED_C && value != 85.0 && value > -50.0 && value < 100.0;
}

void WaterTempSensor::printStatus() {
    float temp;
    bool valid = readTemperature(temp);
    
    Serial.print("🌊 Water Temperature: ");
    Serial.print(valid ? temp : -999);
    Serial.print("°C [");
    Serial.print(valid ? "OK" : "ERROR");
    Serial.println("]");
}

// ===== 🎛️ GLOBAL INSTANCE =====
WaterTempSensor waterTempSensor(DS18B20_PIN); 