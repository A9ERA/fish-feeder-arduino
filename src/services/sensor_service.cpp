#include "sensor_service.h"

SensorService::SensorService() : lastSensorRead(0), lastOutput(0) {
}

void SensorService::begin() {
    Serial.println("🔧 Initializing sensor service...");
    
    // Initialize all sensors
    dhtFeed.begin();
    dhtControl.begin();
    waterTempSensor.begin();
    weightSensor.begin();
    soilSensor.begin();
    voltageSensor.begin();
    currentSensor.begin();
    
    Serial.println("✅ All sensors initialized");
}

void SensorService::readAllSensors() {
    sensors.last_update = millis();
    
    // Read DHT sensors
    bool feedValid = dhtFeed.readBoth(sensors.feed_temp, sensors.feed_humidity);
    bool controlValid = dhtControl.readBoth(sensors.control_temp, sensors.control_humidity);
    
    // Read other sensors
    bool waterValid = waterTempSensor.readTemperature(sensors.water_temp);
    bool weightValid = weightSensor.readWeight(sensors.weight);
    bool soilValid = soilSensor.readMoisture(sensors.soil_moisture);
    bool voltageValid = voltageSensor.readVoltage(sensors.battery_voltage);
    bool currentValid = currentSensor.readCurrent(sensors.solar_current);
    
    // Update error flags
    sensors.errors[0] = !feedValid;      // DHT Feed Tank
    sensors.errors[1] = !controlValid;   // DHT Control Box
    sensors.errors[2] = !waterValid;     // DS18B20 Water
    sensors.errors[3] = !weightValid;    // HX711 Weight
    sensors.errors[4] = !soilValid;      // Soil Moisture
    sensors.errors[5] = !voltageValid;   // Battery Voltage
    sensors.errors[6] = !currentValid;   // Solar Current
}

void SensorService::outputSensorData() {
    Serial.println("📊 ===== SENSOR DATA =====");
    
    // Feed tank DHT22
    Serial.print("🍚 ");
    Serial.print(sensors.feed_temp);
    Serial.print("°C, ");
    Serial.print(sensors.feed_humidity);
    Serial.println("%");
    
    // Control box DHT22
    Serial.print("📦 ");
    Serial.print(sensors.control_temp);
    Serial.print("°C, ");
    Serial.print(sensors.control_humidity);
    Serial.println("%");
    
    // Water temperature
    Serial.print("🌊 ");
    Serial.print(sensors.water_temp);
    Serial.println("°C");
    
    // Weight
    Serial.print("⚖️ ");
    Serial.print(sensors.weight);
    Serial.println("kg");
    
    // Soil moisture
    Serial.print("🌱 ");
    Serial.print(sensors.soil_moisture);
    Serial.println("%");
    
    // Battery voltage
    Serial.print("🔋 ");
    Serial.print(sensors.battery_voltage);
    Serial.println("V");
    
    // Solar current
    Serial.print("☀️ ");
    Serial.print(sensors.solar_current);
    Serial.println("A");
    
    // Print any errors
    printSensorErrors();
    
    Serial.println("========================");
}

void SensorService::outputSystemStatus() {
    Serial.println("🔧 ===== SYSTEM STATUS =====");
    Serial.print("LED Relay: ");
    Serial.println(status.relay_led ? "ON" : "OFF");
    Serial.print("Fan Relay: ");
    Serial.println(status.relay_fan ? "ON" : "OFF");
    Serial.print("Auger: ");
    Serial.println(status.auger_state);
    Serial.print("Blower: ");
    Serial.println(status.blower_state ? "ON" : "OFF");
    Serial.print("Actuator: ");
    Serial.println(status.actuator_state);
    Serial.print("Auto Fan: ");
    Serial.println(status.auto_fan_active ? "ACTIVE" : "INACTIVE");
    
    if (status.is_feeding) {
        Serial.print("Feeding Progress: ");
        Serial.print(sensors.weight);
        Serial.print("/");
        Serial.print(status.feed_target);
        Serial.println("kg");
    }
    
    Serial.println("============================");
}

bool SensorService::shouldReadSensors() {
    return (millis() - lastSensorRead) >= config.sensor_interval;
}

bool SensorService::shouldOutputData() {
    return (millis() - lastOutput) >= config.output_interval;
}

void SensorService::updateTimings() {
    lastSensorRead = millis();
    lastOutput = millis();
}

void SensorService::printSensorErrors() {
    bool hasErrors = false;
    const char* sensorNames[] = {
        "DHT Feed", "DHT Control", "Water Temp", "Weight", 
        "Soil", "Battery", "Solar"
    };
    
    for (int i = 0; i < 7; i++) {
        if (sensors.errors[i]) {
            if (!hasErrors) {
                Serial.println("❌ SENSOR ERRORS:");
                hasErrors = true;
            }
            Serial.print("  - ");
            Serial.print(sensorNames[i]);
            Serial.println(" sensor error");
        }
    }
    
    if (!hasErrors) {
        Serial.println("✅ All sensors OK");
    }
}

// ===== 🎛️ GLOBAL INSTANCE =====
SensorService sensorService; 