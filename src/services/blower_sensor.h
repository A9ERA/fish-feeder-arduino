#pragma once

#include <Arduino.h>

// คลาสสำหรับควบคุม Blower
class BlowerSensor {
private:
    // กำหนดค่าพินสำหรับ Blower (ใช้รีเลย์ 1)
    const int BLOWER_PIN = 50;  // ใช้พินเดียวกับ RELAY_1_PIN
    
    // สถานะของ Blower
    bool blowerState = false;
    
    // ค่าคงที่สำหรับคำสั่ง
    const char* CMD_PREFIX = "[control]:";
    const char* CMD_BLOWER = "blower:";
    const char* CMD_ON = "on";
    const char* CMD_OFF = "off";
    const char* CMD_STATUS = "status";
    
    // บัฟเฟอร์สำหรับเก็บคำสั่งที่รับเข้ามา
    String commandBuffer;
    
    // ฟังก์ชันประมวลผลคำสั่ง
    void processCommand(String command);
    
    // ฟังก์ชันเปลี่ยนสถานะ Blower
    void toggleBlower(bool turnOn);
    
    // ฟังก์ชันส่งสถานะ Blower
    void sendStatus();

public:
    // คอนสตรัคเตอร์
    BlowerSensor();
    
    // ฟังก์ชันเริ่มต้นการทำงาน
    void begin();
    
    // ฟังก์ชันอัพเดทข้อมูลจาก Serial
    void update();
    
    // ฟังก์ชันดึงสถานะของ Blower
    bool getState();
}; 