#include "mqtt_service.h"

MQTTService mqttService;

MQTTService::MQTTService() : mqttClient(wifiClient) {
}

void MQTTService::init() {
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    if (!connectToMQTT()) {
        Serial.println("Failed to connect to MQTT broker");
        return;
    }
    
    Serial.println("MQTT Service initialized successfully");
}

bool MQTTService::connectToMQTT() {
    if (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT broker...");
        if (mqttClient.connect("FishFeederClient")) {
            Serial.println("connected");
            return true;
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
            return false;
        }
    }
    return true;
}

bool MQTTService::publishSensorData(const JsonDocument& payload) {
    if (!mqttClient.connected()) {
        if (!connectToMQTT()) {
            return false;
        }
    }

    char buffer[256];
    serializeJson(payload, buffer);
    
    if (mqttClient.publish(MQTT_TOPIC, buffer)) {
        Serial.print("Data published for sensor: ");
        Serial.println(payload["name"].as<const char*>());
        return true;
    } else {
        Serial.print("Failed to publish data for sensor: ");
        Serial.println(payload["name"].as<const char*>());
        return false;
    }
}

void MQTTService::loop() {
    if (!mqttClient.connected()) {
        connectToMQTT();
    }
    mqttClient.loop();
} 