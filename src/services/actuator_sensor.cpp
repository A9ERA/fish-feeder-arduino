#include <Arduino.h>
#include <ArduinoJson.h>
#include "actuator_sensor.h"

// ฟังก์ชันสร้างออบเจ็กต์ ActuatorSensor
ActuatorSensor::ActuatorSensor() {
    // ตั้งค่าเริ่มต้นพินสำหรับ L298N
    pinMode(ENA_PIN, OUTPUT);
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    
    // ตั้งค่าเริ่มต้นให้หยุดนิ่ง
    stop();
}

// ฟังก์ชันเริ่มต้นการทำงานของ actuator
void ActuatorSensor::begin() {
    // แสดงข้อความพร้อมใช้งาน
    Serial.println("📌 Actuator Control Ready");
    Serial.println("คำสั่งที่รองรับ:");
    Serial.println("[control]:actuator:extend  - ดัน actuator ออก");
    Serial.println("[control]:actuator:retract - ดึง actuator กลับ");
    Serial.println("[control]:actuator:stop    - หยุด actuator");
    Serial.println("[control]:status           - แสดงสถานะ actuator");
}

// ฟังก์ชันอัพเดทข้อมูลจาก Serial
void ActuatorSensor::update() {
    // ตรวจสอบ timeout ถ้ากำลังเคลื่อนที่
    if (isMoving) {
        checkTimeout();
    }
    
    // ตรวจสอบไฟเลี้ยง
    if (!checkPower()) {
        stop();
        Serial.println("ERROR:NO_POWER");
        return;
    }
    
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        // เพิ่มตัวอักษรลงในบัฟเฟอร์ถ้าไม่ใช่ newline
        if (c != '\n') {
            commandBuffer += c;
        } else {
            // ประมวลผลคำสั่งเมื่อได้รับ newline
            if (commandBuffer.length() > 0) {
                processCommand(commandBuffer);
                commandBuffer = "";  // ล้างบัฟเฟอร์
            }
        }
    }
}

// ฟังก์ชันตรวจสอบ timeout
void ActuatorSensor::checkTimeout() {
    if (millis() - moveStartTime >= TIMEOUT_MS) {
        stop();
        Serial.println("ERROR:TIMEOUT");
    }
}

// ฟังก์ชันตรวจสอบไฟเลี้ยง
bool ActuatorSensor::checkPower() {
    // TODO: เพิ่มการตรวจสอบไฟเลี้ยงจริง
    // ตอนนี้จำลองว่ามีไฟเลี้ยงตลอด
    return true;
}

// ฟังก์ชันประมวลผลคำสั่ง
void ActuatorSensor::processCommand(String command) {
    command.trim();  // ลบช่องว่างที่ไม่จำเป็น
    
    // ตรวจสอบรูปแบบคำสั่ง
    if (!command.startsWith(CMD_PREFIX)) {
        Serial.println("ERROR:INVALID_COMMAND");
        return;
    }
    
    // แยกส่วนคำสั่งออกจาก prefix
    String cmd = command.substring(strlen(CMD_PREFIX));
    
    // ประมวลผลคำสั่ง status
    if (cmd == CMD_STATUS) {
        sendStatus();
        return;
    }
    
    // ประมวลผลคำสั่ง actuator
    if (cmd.startsWith(CMD_ACTUATOR)) {
        String actuatorCmd = cmd.substring(strlen(CMD_ACTUATOR));
        
        if (actuatorCmd == CMD_EXTEND) {
            extend();
            Serial.println("OK:MOVING");
            return;
        }
        
        if (actuatorCmd == CMD_RETRACT) {
            retract();
            Serial.println("OK:MOVING");
            return;
        }
        
        if (actuatorCmd == CMD_STOP) {
            stop();
            Serial.println("OK:STOPPED");
            return;
        }
    }
    
    // กรณีไม่รู้จักคำสั่ง
    Serial.println("ERROR:INVALID_COMMAND");
}

// ฟังก์ชันดัน actuator ออก
void ActuatorSensor::extend() {
    if (!checkPower()) {
        Serial.println("ERROR:NO_POWER");
        return;
    }
    
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(ENA_PIN, 255);  // ความเร็วสูงสุด
    isMoving = true;
    isExtending = true;
    moveStartTime = millis();
}

// ฟังก์ชันดึง actuator กลับ
void ActuatorSensor::retract() {
    if (!checkPower()) {
        Serial.println("ERROR:NO_POWER");
        return;
    }
    
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(ENA_PIN, 255);  // ความเร็วสูงสุด
    isMoving = true;
    isExtending = false;
    moveStartTime = millis();
}

// ฟังก์ชันหยุด actuator
void ActuatorSensor::stop() {
    analogWrite(ENA_PIN, 0);  // หยุดมอเตอร์
    isMoving = false;
}

// ฟังก์ชันส่งสถานะ
void ActuatorSensor::sendStatus() {
    if (!isMoving) {
        Serial.println(isExtending ? "OK:EXTENDED" : "OK:RETRACTED");
    } else {
        Serial.println("OK:MOVING");
    }
}

// ฟังก์ชันดึงสถานะปัจจุบันในรูปแบบ JSON
String ActuatorSensor::getStatusJson() {
    StaticJsonDocument<256> doc;
    
    if (!isMoving) {
        doc["status"] = "OK";
        doc["state"] = isExtending ? "EXTENDED" : "RETRACTED";
    } else {
        doc["status"] = "OK";
        doc["state"] = isExtending ? "EXTENDING" : "RETRACTING";
    }
    
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
} 