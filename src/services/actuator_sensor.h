#pragma once

#include <Arduino.h>

// คลาสสำหรับควบคุม actuator ด้วย L298N motor driver
class ActuatorSensor {
private:
    // กำหนดค่าพินสำหรับ L298N
    const int ENA_PIN = 11;  // PWM ควบคุมความเร็ว
    const int IN1_PIN = 12;  // ควบคุมทิศทาง
    const int IN2_PIN = 13;  // ควบคุมทิศทาง
    
    // กำหนดค่าพินสำหรับ L298N (Feeder Motor)
    const int FEEDER_ENA_PIN = 8;  // PWM ควบคุมความเร็ว Feeder
    const int FEEDER_IN1_PIN = 9;  // ควบคุมทิศทาง Feeder
    const int FEEDER_IN2_PIN = 10; // ควบคุมทิศทาง Feeder
    
    // สถานะของ actuator
    bool isMoving = false;
    bool isExtending = false;  // true = ดันออก, false = ดึงกลับ
    unsigned long moveStartTime = 0;  // เวลาเริ่มเคลื่อนที่
    const unsigned long TIMEOUT_MS = 10000;  // timeout 10 วินาที
    
    // ค่าคงที่สำหรับคำสั่ง
    const char* CMD_PREFIX = "[control]:";
    const char* CMD_ACTUATOR = "actuator:";
    const char* CMD_EXTEND = "extend";    // ดันออก
    const char* CMD_RETRACT = "retract";  // ดึงกลับ
    const char* CMD_STOP = "stop";        // หยุด
    const char* CMD_STATUS = "status";    // สถานะ
    
    // บัฟเฟอร์สำหรับเก็บคำสั่งที่รับเข้ามา
    String commandBuffer;
    
    // ฟังก์ชันประมวลผลคำสั่ง
    void processCommand(String command);
    
    // ฟังก์ชันควบคุม actuator
    void extend();    // ดันออก
    void retract();   // ดึงกลับ
    void stop();      // หยุด
    
    // ฟังก์ชันส่งสถานะ
    void sendStatus();
    
    // ฟังก์ชันตรวจสอบ timeout
    void checkTimeout();
    
    // ฟังก์ชันตรวจสอบไฟเลี้ยง
    bool checkPower();
    
    // ฟังก์ชันควบคุม Feeder Motor
    void feederForward();
    void feederBackward();
    void feederStop();

public:
    // คอนสตรัคเตอร์
    ActuatorSensor();
    
    // ฟังก์ชันเริ่มต้นการทำงาน
    void begin();
    
    // ฟังก์ชันอัพเดทข้อมูลจาก Serial
    void update();
    
    // ฟังก์ชันดึงสถานะการเคลื่อนที่
    bool isActuatorMoving() { return isMoving; }
    
    // ฟังก์ชันดึงทิศทางการเคลื่อนที่
    bool isActuatorExtending() { return isExtending; }
    
    // ฟังก์ชันดึงสถานะปัจจุบันในรูปแบบ JSON
    String getStatusJson();
}; 