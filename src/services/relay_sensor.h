#pragma once

#include <Arduino.h>

// คลาสสำหรับควบคุมรีเลย์
class RelaySensor {
private:
    // กำหนดค่าพินสำหรับรีเลย์
    const int RELAY_1_PIN = 50;
    const int RELAY_2_PIN = 52;
    
    // สถานะของรีเลย์
    bool relay1State = false;
    bool relay2State = false;
    
    // ค่าคงที่สำหรับคำสั่ง
    const char* CMD_PREFIX = "[control]:";
    const char* CMD_RELAY = "relay:";
    const char* CMD_ON = "on";
    const char* CMD_OFF = "off";
    const char* CMD_STATUS = "status";
    
    // บัฟเฟอร์สำหรับเก็บคำสั่งที่รับเข้ามา
    String commandBuffer;
    
    // ฟังก์ชันช่วยตรวจสอบหมายเลขรีเลย์
    bool isValidRelayNumber(int relayNum);
    
    // ฟังก์ชันช่วยดึงหมายเลขพินของรีเลย์
    int getRelayPin(int relayNum);
    
    // ฟังก์ชันช่วยดึงตัวแปรสถานะของรีเลย์
    bool* getRelayStatePtr(int relayNum);
    
    // ฟังก์ชันประมวลผลคำสั่ง
    void processCommand(String command);
    
    // ฟังก์ชันเปลี่ยนสถานะรีเลย์
    void toggleRelay(int relayNum, bool turnOn);
    
    // ฟังก์ชันส่งสถานะรีเลย์
    void sendStatus();

public:
    // คอนสตรัคเตอร์
    RelaySensor();
    
    // ฟังก์ชันเริ่มต้นการทำงาน
    void begin();
    
    // ฟังก์ชันอัพเดทข้อมูลจาก Serial
    void update();
    
    // ฟังก์ชันดึงสถานะของรีเลย์ที่ระบุ
    bool getRelayState(int relayNum);
}; 