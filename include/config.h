#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// MQTT Broker settings
#define MQTT_BROKER "YOUR_RASPBERRY_PI_IP"
#define MQTT_PORT 1883
#define MQTT_TOPIC "fish_feeder/sensors"

// MQTT Publishing Control
#define MQTT_PUBLISH_SENSOR_DATA true  // Set to false to disable sensor data publishing

#endif
