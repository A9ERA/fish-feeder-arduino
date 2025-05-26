#include "../../../include/blower.h"

// กำหนดความเร็วเริ่มต้นของ Blower (0-255)
int currentSpeed = 170;
// สถานะการทำงานของ Blower (true = ทำงาน, false = หยุด)
bool isRunning = false;
// กำหนดทิศทางการหมุนของ Blower (true = หมุนย้อนกลับ, false = หมุนปกติ)
bool isReverse = false;

// ฟังก์ชันเริ่มต้น กำหนดโหมดขา PWM สำหรับควบคุม Blower
void initBlower() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

// ฟังก์ชันเริ่มการทำงานของ Blower
void startBlower() {
  isRunning = true;
  updateBlower();
}

// ฟังก์ชันหยุดการทำงานของ Blower
void stopBlower() {
  isRunning = false;
  updateBlower();
}

// ฟังก์ชันตั้งค่าความเร็วของ Blower (0-255)
void setBlowerSpeed(int speed) {
  if (speed < 0) speed = 0;
  if (speed > 255) speed = 255;
  currentSpeed = speed;
  updateBlower();
}

// ฟังก์ชันตั้งค่าทิศทางการหมุนของ Blower
void setBlowerDirection(bool reverse) {
  isReverse = reverse;
  updateBlower();
}

// ฟังก์ชันอัปเดตสถานะของ Blower ตามค่าปัจจุบัน
void updateBlower() {
  if (isRunning) {
    if (isReverse) {
      analogWrite(LPWM, currentSpeed); // หมุนย้อนกลับ
      analogWrite(RPWM, 0);
    } else {
      analogWrite(RPWM, currentSpeed); // หมุนปกติ
      analogWrite(LPWM, 0);
    }
  } else {
    analogWrite(RPWM, 0); // หยุดการทำงาน
    analogWrite(LPWM, 0);
  }
}
