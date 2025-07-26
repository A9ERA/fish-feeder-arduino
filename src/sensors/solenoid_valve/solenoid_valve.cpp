#include "../../../include/solenoid_valve.h"

void initSolenoidValve() {
    pinMode(SOL_ENA, OUTPUT);
    pinMode(SOL_IN1, OUTPUT);
    pinMode(SOL_IN2, OUTPUT);
    
    // Stop initially
    digitalWrite(SOL_IN1, LOW);
    digitalWrite(SOL_IN2, LOW);
    analogWrite(SOL_ENA, 0);
}

void solenoidValveOpen() {
    digitalWrite(SOL_IN1, HIGH);
    digitalWrite(SOL_IN2, LOW);
    analogWrite(SOL_ENA, 255);  // Full speed
}

void solenoidValveClose() {
    digitalWrite(SOL_IN1, LOW);
    digitalWrite(SOL_IN2, HIGH);
    analogWrite(SOL_ENA, 255);  // Full speed
}

void solenoidValveStop() {
    digitalWrite(SOL_IN1, LOW);
    digitalWrite(SOL_IN2, LOW);
    analogWrite(SOL_ENA, 0);
} 