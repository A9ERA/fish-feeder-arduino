#ifndef BLOWER_H
#define BLOWER_H

#include "Arduino.h"
// กำหนดขา PWM สำหรับควบคุมทิศทางของ Blower
#define RPWM 5 // ขา PWM สำหรับหมุนปกติ
#define LPWM 6 // ขา PWM สำหรับหมุนย้อนกลับ

void initBlower();
void startBlower();
void stopBlower();
void setBlowerSpeed(int speed);
void setBlowerDirection(bool reverse);
void updateBlower();

#endif
