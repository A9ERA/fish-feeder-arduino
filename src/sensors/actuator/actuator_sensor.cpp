#include "../../../include/actuator_sensor.h"

// ตัวแปรสถานะ Actuator
static bool actuatorMoving = false;
static int actuatorDirection = ACTUATOR_STOPPED;
static unsigned long actuatorStartTime = 0;
static const unsigned long MAX_ACTUATOR_RUN_TIME = 30000; // 30 seconds max run time

void initActuator() {
  // Initialize Actuator Motor pins
  pinMode(ACTUATOR_UP_PIN, OUTPUT);
  pinMode(ACTUATOR_DOWN_PIN, OUTPUT);
  pinMode(ACTUATOR_ENABLE_PIN, OUTPUT);
  
  // Set all pins to safe state
  digitalWrite(ACTUATOR_ENABLE_PIN, LOW);
  digitalWrite(ACTUATOR_UP_PIN, LOW);
  digitalWrite(ACTUATOR_DOWN_PIN, LOW);
  
  actuatorMoving = false;
  actuatorDirection = ACTUATOR_STOPPED;
  
  Serial.println("🔧 เริ่มต้นระบบ Actuator Motor...");
}

void startActuatorUp() {
  actuatorDirection = ACTUATOR_MOVING_UP;
  actuatorMoving = true;
  actuatorStartTime = millis();
  updateActuator();
  
  Serial.println("⬆️ Actuator เคลื่อนที่ขึ้น");
}

void startActuatorDown() {
  actuatorDirection = ACTUATOR_MOVING_DOWN;
  actuatorMoving = true;
  actuatorStartTime = millis();
  updateActuator();
  
  Serial.println("⬇️ Actuator เคลื่อนที่ลง");
}

void stopActuator() {
  actuatorDirection = ACTUATOR_STOPPED;
  actuatorMoving = false;
  updateActuator();
  
  Serial.println("⏹️ Actuator หยุดการทำงาน");
}

void setActuatorDirection(int direction) {
  if (direction == ACTUATOR_MOVING_UP) {
    startActuatorUp();
  } else if (direction == ACTUATOR_MOVING_DOWN) {
    startActuatorDown();
  } else {
    stopActuator();
  }
}

void updateActuator() {
  // Safety check: auto-stop after maximum run time
  if (actuatorMoving && (millis() - actuatorStartTime > MAX_ACTUATOR_RUN_TIME)) {
    Serial.println("⚠️ Actuator หยุดอัตโนมัติ (เกินเวลากำหนด)");
    stopActuator();
    return;
  }
  
  if (actuatorMoving) {
    digitalWrite(ACTUATOR_ENABLE_PIN, HIGH);
    
    if (actuatorDirection == ACTUATOR_MOVING_UP) {
      digitalWrite(ACTUATOR_UP_PIN, HIGH);
      digitalWrite(ACTUATOR_DOWN_PIN, LOW);
    } else if (actuatorDirection == ACTUATOR_MOVING_DOWN) {
      digitalWrite(ACTUATOR_UP_PIN, LOW);
      digitalWrite(ACTUATOR_DOWN_PIN, HIGH);
    }
  } else {
    digitalWrite(ACTUATOR_ENABLE_PIN, LOW);
    digitalWrite(ACTUATOR_UP_PIN, LOW);
    digitalWrite(ACTUATOR_DOWN_PIN, LOW);
  }
}

bool getActuatorStatus() {
  return actuatorMoving;
}

int getActuatorDirection() {
  return actuatorDirection;
}

StaticJsonDocument<256> readActuatorStatus() {
  StaticJsonDocument<256> doc;
  doc["name"] = ACTUATOR_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  // อัปเดต Actuator ก่อนอ่านสถานะ
  updateActuator();
  
  // สถานะการเคลื่อนที่
  JsonObject movingValue = values.createNestedObject();
  movingValue["type"] = "moving";
  movingValue["unit"] = "boolean";
  movingValue["value"] = actuatorMoving;

  // ทิศทางการเคลื่อนที่
  JsonObject directionValue = values.createNestedObject();
  directionValue["type"] = "direction";
  directionValue["unit"] = "state";
  if (actuatorDirection == ACTUATOR_MOVING_UP) {
    directionValue["value"] = "up";
  } else if (actuatorDirection == ACTUATOR_MOVING_DOWN) {
    directionValue["value"] = "down";
  } else {
    directionValue["value"] = "stopped";
  }

  // เวลาที่เริ่มทำงาน (ถ้ากำลังเคลื่อนที่)
  if (actuatorMoving) {
    JsonObject runtimeValue = values.createNestedObject();
    runtimeValue["type"] = "runtime";
    runtimeValue["unit"] = "ms";
    runtimeValue["value"] = millis() - actuatorStartTime;
  }

  Serial.print("🔧 Actuator Status - Moving: ");
  Serial.print(actuatorMoving ? "YES" : "NO");
  Serial.print(", Direction: ");
  if (actuatorDirection == ACTUATOR_MOVING_UP) {
    Serial.println("UP");
  } else if (actuatorDirection == ACTUATOR_MOVING_DOWN) {
    Serial.println("DOWN");
  } else {
    Serial.println("STOPPED");
  }

  return doc;
}

void processActuatorCommand(const String& command) {
  if (command == "up") {
    startActuatorUp();
  } else if (command == "down") {
    startActuatorDown();
  } else if (command == "stop") {
    stopActuator();
  } else {
    Serial.println("❌ คำสั่ง Actuator ไม่ถูกต้อง: " + command);
  }
} 