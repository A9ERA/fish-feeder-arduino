#include "dht_sensor.h"

DHTSensor::DHTSensor(uint8_t pin, String name) : pin(pin), name(name) {
    dht = new DHT(pin, DHT22);
}

DHTSensor::~DHTSensor() {
    delete dht;
}

void DHTSensor::begin() {
    dht->begin();
    delay(100);
    Serial.print("🌡️ ");
    Serial.print(name);
    Serial.println(" DHT22 initialized");
}

bool DHTSensor::readTemperature(float& temperature) {
    temperature = dht->readTemperature();
    return isValidReading(temperature);
}

bool DHTSensor::readHumidity(float& humidity) {
    humidity = dht->readHumidity();
    return isValidReading(humidity);
}

bool DHTSensor::readBoth(float& temperature, float& humidity) {
    temperature = dht->readTemperature();
    humidity = dht->readHumidity();
    
    bool tempValid = isValidReading(temperature);
    bool humidValid = isValidReading(humidity);
    
    return tempValid && humidValid;
}

bool DHTSensor::isValidReading(float value) {
    return !isnan(value) && value != 0.0;
}

void DHTSensor::printStatus() {
    float temp, humid;
    bool valid = readBoth(temp, humid);
    
    Serial.print("🌡️ ");
    Serial.print(name);
    Serial.print(" - Temp: ");
    Serial.print(valid ? temp : -999);
    Serial.print("°C, Humidity: ");
    Serial.print(valid ? humid : -999);
    Serial.print("% [");
    Serial.print(valid ? "OK" : "ERROR");
    Serial.println("]");
}

// ===== 🎛️ GLOBAL INSTANCES =====
DHTSensor dhtFeed(DHT_FEED_PIN, "Feed Tank");
DHTSensor dhtControl(DHT_CONTROL_PIN, "Control Box"); 