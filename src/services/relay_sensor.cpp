#include <Arduino.h>
#include "services/relay_sensor.h"

// ฟังก์ชันสร้างออบเจ็กต์ RelaySensor
RelaySensor::RelaySensor() {
    // ตั้งค่าเริ่มต้นพินสำหรับรีเลย์
    pinMode(RELAY_1_PIN, OUTPUT);
    pinMode(RELAY_2_PIN, OUTPUT);
    
    // ตั้งค่ารีเลย์ให้อยู่ในสถานะ OFF (HIGH = OFF สำหรับโมดูลรีเลย์ส่วนใหญ่)
    digitalWrite(RELAY_1_PIN, HIGH);
    digitalWrite(RELAY_2_PIN, HIGH);
}

// ฟังก์ชันเริ่มต้นการทำงานของรีเลย์
void RelaySensor::begin() {
    // แสดงข้อความพร้อมใช้งาน
    Serial.println("📌 Relay Control Ready");
    Serial.println("Commands:");
    Serial.println("  [control]:relay:on    - Turn ON");
    Serial.println("  [control]:relay:off   - Turn OFF");
    Serial.println("  [control]:status      - Status");
}

// ฟังก์ชันตรวจสอบหมายเลขรีเลย์ว่าถูกต้องหรือไม่
bool RelaySensor::isValidRelayNumber(int relayNum) {
    return (relayNum == 1 || relayNum == 2);  // มีเฉพาะรีเลย์ 1 และ 2
}

// ฟังก์ชันดึงหมายเลขพินของรีเลย์
int RelaySensor::getRelayPin(int relayNum) {
    return (relayNum == 1) ? RELAY_1_PIN : RELAY_2_PIN;  // มีเฉพาะรีเลย์ 1 และ 2
}

// ฟังก์ชันดึงตัวแปรสถานะของรีเลย์
bool* RelaySensor::getRelayStatePtr(int relayNum) {
    return (relayNum == 1) ? &relay1State : &relay2State;  // มีเฉพาะรีเลย์ 1 และ 2
}

// ฟังก์ชันอัพเดทข้อมูลจาก Serial
void RelaySensor::update() {
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

// ฟังก์ชันประมวลผลคำสั่ง
void RelaySensor::processCommand(String command) {
    command.trim();  // ลบช่องว่างที่ไม่จำเป็น
    
    // ตรวจสอบรูปแบบคำสั่ง
    if (!command.startsWith(CMD_PREFIX)) {
        Serial.println("[ERROR] INVALID_COMMAND");
        return;
    }
    
    // แยกส่วนคำสั่งออกจาก prefix
    String cmd = command.substring(strlen(CMD_PREFIX));
    
    // ประมวลผลคำสั่ง status
    if (cmd == CMD_STATUS) {
        sendStatus();
        return;
    }
    
    // ประมวลผลคำสั่ง relay
    if (cmd.startsWith(CMD_RELAY)) {
        String relayCmd = cmd.substring(strlen(CMD_RELAY));
        
        if (relayCmd == CMD_ON) {
            // เปิดรีเลย์ทั้งหมด
            toggleRelay(1, true);
            toggleRelay(2, true);
            Serial.println("[OK] RELAY_ON");
            return;
        }
        
        if (relayCmd == CMD_OFF) {
            // ปิดรีเลย์ทั้งหมด
            toggleRelay(1, false);
            toggleRelay(2, false);
            Serial.println("[OK] RELAY_OFF");
            return;
        }
    }
    
    // กรณีไม่รู้จักคำสั่ง
    Serial.println("[ERROR] INVALID_COMMAND");
}

// ฟังก์ชันเปลี่ยนสถานะรีเลย์
void RelaySensor::toggleRelay(int relayNum, bool turnOn) {
    if (!isValidRelayNumber(relayNum)) {
        Serial.println("❌ หมายเลขรีเลย์ไม่ถูกต้อง");
        return;
    }

    int pin = getRelayPin(relayNum);
    bool* state = getRelayStatePtr(relayNum);
    
    // อัพเดทสถานะรีเลย์ (LOW = ON สำหรับโมดูลรีเลย์ส่วนใหญ่)
    digitalWrite(pin, turnOn ? LOW : HIGH);
    *state = turnOn;
}

// ฟังก์ชันส่งสถานะรีเลย์
void RelaySensor::sendStatus() {
    Serial.println("[STATUS] Relay Status:");
    Serial.print("  Relay 1: ");
    Serial.println(relay1State ? "ON" : "OFF");
    Serial.print("  Relay 2: ");
    Serial.println(relay2State ? "ON" : "OFF");
}

// ฟังก์ชันดึงสถานะของรีเลย์ที่ระบุ
bool RelaySensor::getRelayState(int relayNum) {
    if (!isValidRelayNumber(relayNum)) {
        return false;
    }
    return (relayNum == 1) ? relay1State : relay2State;
} 