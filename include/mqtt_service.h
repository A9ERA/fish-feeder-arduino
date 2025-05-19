#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"

class MQTTService {
private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    bool connectToMQTT();

public:
    MQTTService();
    void init();
    bool publishSensorData(const JsonDocument& payload);
    void loop();
};

extern MQTTService mqttService;

#endif 