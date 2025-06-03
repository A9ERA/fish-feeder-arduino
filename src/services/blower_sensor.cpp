#include <Arduino.h>
#include "blower_sensor.h"

// ฟังก์ชันสร้างออบเจ็กต์ BlowerSensor
BlowerSensor::BlowerSensor() {
    // ตั้งค่าเริ่มต้นพินสำหรับ Blower
    pinMode(BLOWER_PIN, OUTPUT);
    
    // ตั้งค่า Blower ให้อยู่ในสถานะ OFF (HIGH = OFF สำหรับโมดูลรีเลย์ส่วนใหญ่)
    digitalWrite(BLOWER_PIN, HIGH);
}

// ฟังก์ชันเริ่มต้นการทำงานของ Blower
void BlowerSensor::begin() {
    // แสดงข้อความพร้อมใช้งาน
    Serial.println("📌 Blower Control Ready");
    Serial.println("คำสั่งที่รองรับ:");
    Serial.println("[control]:blower:on   - เปิด Blower");
    Serial.println("[control]:blower:off  - ปิด Blower");
    Serial.println("[control]:status      - แสดงสถานะ Blower");
}

// ฟังก์ชันอัพเดทข้อมูลจาก Serial
void BlowerSensor::update() {
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
void BlowerSensor::processCommand(String command) {
    command.trim();  // ลบช่องว่างที่ไม่จำเป็น
    
    // ตรวจสอบรูปแบบคำสั่ง
    if (!command.startsWith(CMD_PREFIX)) {
        Serial.println("❌ รูปแบบคำสั่งไม่ถูกต้อง");
        return;
    }
    
    // แยกส่วนคำสั่งออกจาก prefix
    String cmd = command.substring(strlen(CMD_PREFIX));
    
    // ประมวลผลคำสั่ง status
    if (cmd == CMD_STATUS) {
        sendStatus();
        return;
    }
    
    // ประมวลผลคำสั่ง blower
    if (cmd.startsWith(CMD_BLOWER)) {
        String blowerCmd = cmd.substring(strlen(CMD_BLOWER));
        
        if (blowerCmd == CMD_ON) {
            toggleBlower(true);
            Serial.println("✅ Blower ON");
            return;
        }
        
        if (blowerCmd == CMD_OFF) {
            toggleBlower(false);
            Serial.println("✅ Blower OFF");
            return;
        }
    }
    
    // กรณีไม่รู้จักคำสั่ง
    Serial.println("❌ ไม่รู้จักคำสั่ง");
}

// ฟังก์ชันเปลี่ยนสถานะ Blower
void BlowerSensor::toggleBlower(bool turnOn) {
    // อัพเดทสถานะ Blower (LOW = ON สำหรับโมดูลรีเลย์ส่วนใหญ่)
    digitalWrite(BLOWER_PIN, turnOn ? LOW : HIGH);
    blowerState = turnOn;
}

// ฟังก์ชันส่งสถานะ Blower
void BlowerSensor::sendStatus() {
    Serial.println("📊 สถานะ Blower:");
    Serial.print("Blower: ");
    Serial.println(blowerState ? "เปิด" : "ปิด");
}

// ฟังก์ชันดึงสถานะของ Blower
bool BlowerSensor::getState() {
    return blowerState;
} 