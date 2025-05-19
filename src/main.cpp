#include <Arduino.h>
#include <WiFi.h>
#include "dht_sensor.h"
#include "soil_sensor.h"
#include "water_temp_sensor.h"
#include "weight_sensor.h"
#include "mqtt_service.h"
#include "config.h"

bool connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    Serial.println("\nFailed to connect to WiFi");
    return false;
}

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi first
  if (!connectToWiFi()) {
    Serial.println("Failed to initialize WiFi. System may not work properly.");
  }
  
  initDHT();
  initSoil();
  initWaterTemp();
  initWeight();
  mqttService.init();
}

void loop() {
  // Read and publish data for each sensor
  mqttService.publishSensorData(readDHTSystem());
  mqttService.publishSensorData(readDHTFeeder());
  mqttService.publishSensorData(readSoil());
  mqttService.publishSensorData(readWaterTemp());
  mqttService.publishSensorData(readWeight());

  mqttService.loop();
  delay(1000);
}


