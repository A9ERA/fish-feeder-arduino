// === Fish Feeder Arduino System - Production Version ===
#include <Arduino.h>
#include <DHT.h>
#include <HX711.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <avr/wdt.h>  // Watchdog timer for production stability

// === พินเซ็นเซอร์และควบคุม ===
#define DHTPIN_BOX 48     // DHT22 ตู้ควบคุม
#define DHTPIN_FEED 46    // DHT22 ถังอาหาร
#define DHTTYPE DHT22
#define ONE_WIRE_BUS 44   // DS18B20
#define SOIL_PIN A2       // AO Soil Sensor
#define HX_DT 20
#define HX_SCK 21

// === พิน Power Monitoring ===
#define SOLAR_VOLTAGE_PIN A3
#define SOLAR_CURRENT_PIN A4
#define LOAD_VOLTAGE_PIN A1
#define LOAD_CURRENT_PIN A0

// === พินควบคุม ===
#define RPWM 5
#define LPWM 6
#define RELAY_IN1 50
#define RELAY_IN2 52
#define ACT_ENA 11
#define ACT_IN1 12
#define ACT_IN2 13
#define AUG_ENA 8
#define AUG_IN1 9
#define AUG_IN2 10

// === EEPROM Address ===
#define EEPROM_SCALE_ADDR 0
#define EEPROM_OFFSET_ADDR (EEPROM_SCALE_ADDR + sizeof(float))
#define EEPROM_FEED_COUNT_ADDR (EEPROM_OFFSET_ADDR + sizeof(long))

// === Production Configuration ===
#define PRODUCTION_MODE true
#define ENABLE_WATCHDOG true
#define SENSOR_INTERVAL 10000    // 10 seconds for production
#define HEARTBEAT_INTERVAL 30000 // 30 seconds
#define SENSOR_TIMEOUT 5000      // 5 second timeout for sensor readings
#define MAX_RETRY_COUNT 3        // Maximum retry attempts
#define SAFETY_TIMEOUT 300000    // 5 minutes safety timeout for operations

// === เซ็นเซอร์ประกาศ ===
DHT dhtBox(DHTPIN_BOX, DHTTYPE);
DHT dhtFeed(DHTPIN_FEED, DHTTYPE);
HX711 scale;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature waterTemp(&oneWire);

// === ตัวแปรหน่วยน้ำหนัก ===
float scaleFactor = 1.0;
long offset = 0;
uint16_t feedCount = 0;

// === ตัวแปรระบบควบคุม ===
int blowerSpeed = 250;
bool isBlowerRunning = false;
bool isActuatorRunning = false;
bool isAugerRunning = false;
bool relay1State = false;
bool relay2State = false;
bool systemError = false;
String lastError = "";

// === ตัวแปร Safety & Timing ===
unsigned long blowerStartTime = 0;
unsigned long actuatorStartTime = 0;
unsigned long augerStartTime = 0;
unsigned long systemStartTime = 0;

// === ตัวแปร Power Monitor ===
const float vRef = 5.0;
const float vFactor = 4.50;
const float sensitivity = 0.066;
const float zeroCurrentVoltage = 2.500;

// === ตัวแปรระบบ ===
int printCount = 1;
bool showSensorData = PRODUCTION_MODE ? false : true; // ใน Production ไม่แสดง debug
bool showReadableDisplay = PRODUCTION_MODE ? false : true;
char currentMenu = 'M';
unsigned long lastSensorRead = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastWatchdogReset = 0;

// === Function Declarations ===
int freeMemory();
void showSimpleMenu();
void handleSimpleCommand(char cmd);
void testAllSensors();
void testPowerSensors();
void emergencyShutdown();
void resetWatchdog();
bool checkSensorHealth();
void logSystemEvent(String event);

// === ฟังก์ชัน Emergency Shutdown ===
void emergencyShutdown() {
  Serial.println("[EMERGENCY] System shutdown initiated!");
  
  // หยุดทุกการทำงาน
  digitalWrite(RPWM, LOW);
  digitalWrite(LPWM, LOW);
  digitalWrite(ACT_ENA, LOW);
  digitalWrite(AUG_ENA, LOW);
  digitalWrite(RELAY_IN1, HIGH);
  digitalWrite(RELAY_IN2, HIGH);
  
  // Reset states
  isBlowerRunning = false;
  isActuatorRunning = false;
  isAugerRunning = false;
  relay1State = false;
  relay2State = false;
  
  systemError = true;
  lastError = "Emergency shutdown activated";
  
  Serial.println("[EMERGENCY] All systems stopped");
}

// === ฟังก์ชัน Watchdog Reset ===
void resetWatchdog() {
  if (ENABLE_WATCHDOG && millis() - lastWatchdogReset > 1000) {
    wdt_reset();
    lastWatchdogReset = millis();
  }
}

// === ตรวจสอบสุขภาพเซ็นเซอร์ ===
bool checkSensorHealth() {
  bool healthy = true;
  
  // ตรวจ DHT sensors
  float temp1 = dhtBox.readTemperature();
  float hum1 = dhtBox.readHumidity();
  if (isnan(temp1) || isnan(hum1)) {
    lastError = "DHT Box sensor error";
    healthy = false;
  }
  
  float temp2 = dhtFeed.readTemperature();
  float hum2 = dhtFeed.readHumidity();
  if (isnan(temp2) || isnan(hum2)) {
    lastError = "DHT Feed sensor error";
    healthy = false;
  }
  
  // ตรวจ Water temperature
  waterTemp.requestTemperatures();
  float waterTempC = waterTemp.getTempCByIndex(0);
  if (waterTempC == -127.00) {
    lastError = "Water temperature sensor error";
    healthy = false;
  }
  
  return healthy;
}

// === บันทึกเหตุการณ์ระบบ ===
void logSystemEvent(String event) {
  if (PRODUCTION_MODE) {
    StaticJsonDocument<256> doc;
    doc["timestamp"] = millis();
    doc["event"] = event;
    doc["uptime"] = millis() - systemStartTime;
    doc["free_memory"] = freeMemory();
    
    Serial.print("[LOG] ");
    serializeJson(doc, Serial);
    Serial.println();
  } else {
    Serial.println("📝 " + event);
  }
}

// === ฟังก์ชันประเมินเปอร์เซ็นต์แบตเตอรี่ ===
float estimateBatteryPercentage(float voltage) {
  const float minV = 11.70;
  const float maxV = 12.70;
  if (voltage >= maxV) return 100.0;
  if (voltage <= minV) return 0.0;
  return ((voltage - minV) / (maxV - minV)) * 100.0;
}

// === อ่านค่าเซ็นเซอร์พลังงานแบบ Production ===
void readPowerSensors(float& solarV, float& solarI, float& loadV, float& loadI) {
  const int sampleCount = PRODUCTION_MODE ? 30 : 50; // ลด sample ใน production
  long sumVS = 0, sumIS = 0;
  long sumVL = 0, sumIL = 0;

  for (int i = 0; i < sampleCount; i++) {
    sumVS += analogRead(SOLAR_VOLTAGE_PIN);
    sumIS += analogRead(SOLAR_CURRENT_PIN);
    sumVL += analogRead(LOAD_VOLTAGE_PIN);
    sumIL += analogRead(LOAD_CURRENT_PIN);
    delay(1);
    resetWatchdog(); // Reset watchdog during sampling
  }

  solarV = (sumVS / (float)sampleCount / 1023.0) * vRef * vFactor;
  loadV  = (sumVL / (float)sampleCount / 1023.0) * vRef * vFactor;
  solarI = (((sumIS / (float)sampleCount) / 1023.0) * vRef - zeroCurrentVoltage) / sensitivity;
  loadI  = (((sumIL / (float)sampleCount) / 1023.0) * vRef - zeroCurrentVoltage) / sensitivity;

  // Filter noise
  if (solarV < 1.0) solarV = 0.0;
  if (abs(solarI) < 0.2) solarI = 0.0;
  if (loadI < 0.0) loadI = -loadI;
}

// === แสดงข้อมูลเซ็นเซอร์แบบ Production (เฉพาะ debug mode) ===
void displaySensorReadout() {
  if (!showSensorData || !showReadableDisplay || PRODUCTION_MODE) return;

  static int displayCount = 1;
  
  // Clear screen และแสดงหัวข้อ
  Serial.println("\n" + String('=').substring(0, 80));
  Serial.println("🐟 FISH FEEDER MONITORING DASHBOARD - Reading #" + String(displayCount++));
  Serial.println("⏰ Runtime: " + String(millis() / 1000) + " seconds | 💾 Free RAM: " + String(freeMemory()) + " bytes");
  Serial.println(String('=').substring(0, 80));
  
  // === Environmental Sensors ===
  Serial.println("🌡️  ENVIRONMENTAL SENSORS");
  Serial.println(String('-').substring(0, 40));
  
  float temp1 = dhtBox.readTemperature();
  float hum1 = dhtBox.readHumidity();
  float temp2 = dhtFeed.readTemperature();
  float hum2 = dhtFeed.readHumidity();
  
  Serial.print("📦 Control Box  : "); Serial.print(temp1, 1); Serial.print(" °C | "); Serial.print(hum1, 1); Serial.println(" %");
  Serial.print("🍚 Feed Tank    : "); Serial.print(temp2, 1); Serial.print(" °C | "); Serial.print(hum2, 1); Serial.println(" %");
  
  // Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  float soilPct = map(soilRaw, 300, 1023, 100, 0);
  soilPct = constrain(soilPct, 0, 100);
  Serial.print("🌱 Soil Moisture: "); Serial.print(soilPct, 1); Serial.print(" % (Raw: "); Serial.print(soilRaw); Serial.println(")");
  
  // Water Temperature
  waterTemp.requestTemperatures();
  float waterTempC = waterTemp.getTempCByIndex(0);
  if (waterTempC == -127.00) {
    Serial.println("🌊 Water Temp   : ❌ Sensor Error");
  } else {
    Serial.print("🌊 Water Temp   : "); Serial.print(waterTempC, 1); Serial.println(" °C");
  }
  
  // Weight
  float weight = scale.get_units(3); // ลด sample ใน production
  Serial.print("⚖️ Feed Weight  : "); Serial.print(weight, 3); Serial.println(" kg");
  
  Serial.println();
  
  // === Power Monitoring ===
  Serial.println("🔋 POWER MONITORING");
  Serial.println(String('-').substring(0, 40));
  
  float solarV, solarI, loadV, loadI;
  readPowerSensors(solarV, solarI, loadV, loadI);
  float batteryPct = estimateBatteryPercentage(loadV);
  
  Serial.print("☀️ Solar Panel : "); Serial.print(solarV, 1); Serial.print(" V | "); Serial.print(solarI, 3); Serial.println(" A");
  Serial.print("🔋 Battery Load : "); Serial.print(loadV, 1); Serial.print(" V | "); Serial.print(loadI, 3); Serial.println(" A");
  
  // Battery status with visual indicator
  String batteryBar = "";
  int barLength = (int)(batteryPct / 5); // 20 segments for 100%
  for (int i = 0; i < 20; i++) {
    if (i < barLength) batteryBar += "█";
    else batteryBar += "░";
  }
  
  if (solarV > 5.0) {
    Serial.print("🔋 Battery Status: ["); Serial.print(batteryBar); Serial.print("] "); Serial.print(batteryPct, 1); Serial.println("% ⚡ CHARGING");
  } else {
    Serial.print("🔋 Battery Status: ["); Serial.print(batteryBar); Serial.print("] "); Serial.print(batteryPct, 1); Serial.println("%");
  }
  
  Serial.println();
  
  // === System Status ===
  Serial.println("⚙️  SYSTEM STATUS");
  Serial.println(String('-').substring(0, 40));
  
  Serial.print("🌀 Blower       : "); Serial.print(isBlowerRunning ? "🟢 RUNNING" : "🔴 STOPPED");
  if (isBlowerRunning) { Serial.print(" (Speed: "); Serial.print(blowerSpeed); Serial.print("/255)"); }
  Serial.println();
  
  Serial.print("🦾 Actuator     : "); Serial.println(isActuatorRunning ? "🟢 ACTIVE" : "🔴 IDLE");
  Serial.print("⚙️ Auger        : "); Serial.println(isAugerRunning ? "🟢 FEEDING" : "🔴 STOPPED");
  Serial.print("🔌 Relay 1      : "); Serial.println(relay1State ? "🟢 ON" : "🔴 OFF");
  Serial.print("🔌 Relay 2      : "); Serial.println(relay2State ? "🟢 ON" : "🔴 OFF");
  
  if (systemError) {
    Serial.println("❌ SYSTEM ERROR: " + lastError);
  }
  
  Serial.println(String('=').substring(0, 80));
}

// === ฟังก์ชันดู Memory ===
int freeMemory() {
  char top;
  extern char *__brkval;
  extern char __bss_end;
  return __brkval ? &top - __brkval : &top - &__bss_end;
}

// === ส่ง Heartbeat ===
void sendHeartbeat() {
  StaticJsonDocument<256> doc;
  doc["heartbeat"]["uptime"] = millis();
  doc["heartbeat"]["free_memory"] = freeMemory();
  doc["heartbeat"]["system_ready"] = true;
  
  Serial.print("[HEARTBEAT] - ");
  serializeJson(doc, Serial);
  Serial.println();
}

// === ควบคุมพัดลม ===
void controlBlower(String action, int speed = -1) {
  if (action == "start") {
    analogWrite(RPWM, blowerSpeed);
    digitalWrite(LPWM, LOW);
    isBlowerRunning = true;
    blowerStartTime = millis();
    Serial.println("[OK] - Blower เริ่มทำงาน");
  } else if (action == "stop") {
    analogWrite(RPWM, 0);
    digitalWrite(LPWM, 0);
    isBlowerRunning = false;
    Serial.println("[OK] - Blower หยุด");
  } else if (action == "speed" && speed >= 0 && speed <= 255) {
    blowerSpeed = speed;
    if (isBlowerRunning) {
      analogWrite(RPWM, blowerSpeed);
    }
    Serial.println("[OK] - Blower speed: " + String(speed));
  }
}

// === ควบคุม Actuator ===
void controlActuator(String action) {
  if (action == "up") {
    digitalWrite(ACT_IN1, HIGH);
    digitalWrite(ACT_IN2, LOW);
    analogWrite(ACT_ENA, 255);
    isActuatorRunning = true;
    actuatorStartTime = millis();
    Serial.println("[OK] - Actuator ดันออก");
  } else if (action == "down") {
    digitalWrite(ACT_IN1, LOW);
    digitalWrite(ACT_IN2, HIGH);
    analogWrite(ACT_ENA, 255);
    isActuatorRunning = true;
    actuatorStartTime = millis();
    Serial.println("[OK] - Actuator ดึงกลับ");
  } else if (action == "stop") {
    analogWrite(ACT_ENA, 0);
    isActuatorRunning = false;
    Serial.println("[OK] - Actuator หยุด");
  }
}

// === ควบคุม Auger ===
void controlAuger(String action) {
  if (action == "forward") {
    digitalWrite(AUG_IN1, HIGH);
    digitalWrite(AUG_IN2, LOW);
    analogWrite(AUG_ENA, 200);
    isAugerRunning = true;
    augerStartTime = millis();
    Serial.println("[OK] - Auger เดินหน้า");
  } else if (action == "reverse") {
    digitalWrite(AUG_IN1, LOW);
    digitalWrite(AUG_IN2, HIGH);
    analogWrite(AUG_ENA, 200);
    isAugerRunning = true;
    augerStartTime = millis();
    Serial.println("[OK] - Auger ถอยหลัง");
  } else if (action == "stop") {
    analogWrite(AUG_ENA, 0);
    isAugerRunning = false;
    Serial.println("[OK] - Auger หยุด");
  }
}

// === ควบคุม Relay ===
void controlRelay(int relayNum, bool state) {
  if (relayNum == 1) {
    digitalWrite(RELAY_IN1, state ? LOW : HIGH);
    relay1State = state;
    Serial.println("[OK] - Relay 1: " + String(state ? "ON" : "OFF"));
  } else if (relayNum == 2) {
    digitalWrite(RELAY_IN2, state ? LOW : HIGH);
    relay2State = state;
    Serial.println("[OK] - Relay 2: " + String(state ? "ON" : "OFF"));
  }
}

// === ประมวลผลคำสั่งจาก Serial ===
void processCommand() {
  if (!Serial.available()) return;
  
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  // ถ้าเป็นคำสั่ง JSON แบบเดิม
  if (command.startsWith("[control]:")) {
    command = command.substring(10); // remove "[control]:"
    
    // Parse command: device:action:parameter
    int firstColon = command.indexOf(':');
    if (firstColon == -1) {
      Serial.println("[ERROR] - Invalid command format");
      return;
    }
    
    String device = command.substring(0, firstColon);
    String rest = command.substring(firstColon + 1);
    
    // Log received command
    Serial.println("[CMD_RECV] - Device: " + device + ", Action: " + rest);
    
    // Process commands
    if (device == "blower") {
      if (rest == "start") controlBlower("start");
      else if (rest == "stop") controlBlower("stop");
      else if (rest.startsWith("speed:")) {
        int speed = rest.substring(6).toInt();
        controlBlower("speed", speed);
      }
    } else if (device == "actuator") {
      controlActuator(rest);
    } else if (device == "auger") {
      controlAuger(rest);
    } else if (device == "relay") {
      if (rest.startsWith("1:")) {
        bool state = rest.substring(2) == "on";
        controlRelay(1, state);
      } else if (rest.startsWith("2:")) {
        bool state = rest.substring(2) == "on";
        controlRelay(2, state);
      }
    } else if (device == "system") {
      if (rest == "sensors:toggle") {
        showSensorData = !showSensorData;
        Serial.println("[OK] - Sensor display: " + String(showSensorData ? "ON" : "OFF"));
      } else if (rest == "display:toggle") {
        showReadableDisplay = !showReadableDisplay;
        Serial.println("[OK] - Readable display: " + String(showReadableDisplay ? "ON" : "OFF"));
      } else if (rest == "calibrate") {
        Serial.println("[INFO] - Send weight value for calibration");
        // Wait for weight input (implement calibration logic)
      }
    } else {
      Serial.println("[ERROR] - Unknown device: " + device);
    }
    
    // Send command acknowledgment
    Serial.println("[CMD_ACK] - " + device + ":" + rest + " executed");
  } 
  // ถ้าเป็นคำสั่งแบบง่าย ๆ (1 ตัวอักษร)
  else if (command.length() == 1) {
    char cmd = command.charAt(0);
    handleSimpleCommand(cmd);
  }
  // ถ้าเป็นคำสั่งเมนู
  else {
    Serial.println("[ERROR] - Unknown command: " + command);
    showSimpleMenu();
  }
}

// === เมนูง่าย ๆ สำหรับเทส ===
void showSimpleMenu() {
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║        🐟 FISH FEEDER TEST MENU      ║");
  Serial.println("╠══════════════════════════════════════╣");
  Serial.println("║ 🌀 BLOWER CONTROL:                  ║");
  Serial.println("║   1 = Start Blower                   ║");
  Serial.println("║   2 = Stop Blower                    ║");
  Serial.println("║                                      ║");
  Serial.println("║ 🦾 ACTUATOR CONTROL:                ║");
  Serial.println("║   3 = Actuator Up                    ║");
  Serial.println("║   4 = Actuator Down                  ║");
  Serial.println("║   5 = Actuator Stop                  ║");
  Serial.println("║                                      ║");
  Serial.println("║ ⚙️ AUGER CONTROL:                   ║");
  Serial.println("║   6 = Auger Forward                  ║");
  Serial.println("║   7 = Auger Reverse                  ║");
  Serial.println("║   8 = Auger Stop                     ║");
  Serial.println("║                                      ║");
  Serial.println("║ 🔌 RELAY CONTROL:                   ║");
  Serial.println("║   a = Relay 1 ON                     ║");
  Serial.println("║   b = Relay 1 OFF                    ║");
  Serial.println("║   c = Relay 2 ON                     ║");
  Serial.println("║   d = Relay 2 OFF                    ║");
  Serial.println("║                                      ║");
  Serial.println("║ 📊 SYSTEM:                          ║");
  Serial.println("║   s = Toggle Sensor Display          ║");
  Serial.println("║   r = Toggle Readable Display        ║");
  Serial.println("║   t = Test All Sensors               ║");
  Serial.println("║   p = Test Power Sensors             ║");
  Serial.println("║   m = Show This Menu                 ║");
  Serial.println("╚══════════════════════════════════════╝");
}

// === จัดการคำสั่งง่าย ๆ ===
void handleSimpleCommand(char cmd) {
  Serial.print("💡 Command received: "); Serial.println(cmd);
  
  switch (cmd) {
    // Blower
    case '1':
      controlBlower("start");
      break;
    case '2':
      controlBlower("stop");
      break;
    
    // Actuator
    case '3':
      controlActuator("up");
      break;
    case '4':
      controlActuator("down");
      break;
    case '5':
      controlActuator("stop");
      break;
    
    // Auger
    case '6':
      controlAuger("forward");
      break;
    case '7':
      controlAuger("reverse");
      break;
    case '8':
      controlAuger("stop");
      break;
    
    // Relay
    case 'a':
    case 'A':
      controlRelay(1, true);
      break;
    case 'b':
    case 'B':
      controlRelay(1, false);
      break;
    case 'c':
    case 'C':
      controlRelay(2, true);
      break;
    case 'd':
    case 'D':
      controlRelay(2, false);
      break;
    
    // System
    case 's':
    case 'S':
      showSensorData = !showSensorData;
      Serial.println("🔄 Sensor display: " + String(showSensorData ? "ON" : "OFF"));
      break;
    case 'r':
    case 'R':
      showReadableDisplay = !showReadableDisplay;
      Serial.println("🔄 Readable display: " + String(showReadableDisplay ? "ON" : "OFF"));
      break;
    case 't':
    case 'T':
      testAllSensors();
      break;
    case 'p':
    case 'P':
      testPowerSensors();
      break;
    case 'm':
    case 'M':
      showSimpleMenu();
      break;
    
    default:
      Serial.println("❌ Unknown command: " + String(cmd));
      showSimpleMenu();
      break;
  }
}

// === เทสเซ็นเซอร์ทั้งหมด ===
void testAllSensors() {
  Serial.println("\n🧪 TESTING ALL SENSORS...");
  Serial.println("================================");
  
  // DHT Sensors
  Serial.print("📦 DHT Box (Pin 48): ");
  float temp1 = dhtBox.readTemperature();
  float hum1 = dhtBox.readHumidity();
  if (isnan(temp1) || isnan(hum1)) {
    Serial.println("❌ ERROR");
  } else {
    Serial.print(temp1); Serial.print("°C, "); Serial.print(hum1); Serial.println("%");
  }
  
  Serial.print("🍚 DHT Feed (Pin 46): ");
  float temp2 = dhtFeed.readTemperature();
  float hum2 = dhtFeed.readHumidity();
  if (isnan(temp2) || isnan(hum2)) {
    Serial.println("❌ ERROR");
  } else {
    Serial.print(temp2); Serial.print("°C, "); Serial.print(hum2); Serial.println("%");
  }
  
  // Soil Sensor
  Serial.print("🌱 Soil (Pin A2): ");
  int soilRaw = analogRead(SOIL_PIN);
  Serial.print("Raw: "); Serial.print(soilRaw);
  float soilPct = map(soilRaw, 300, 1023, 100, 0);
  soilPct = constrain(soilPct, 0, 100);
  Serial.print(" -> "); Serial.print(soilPct); Serial.println("%");
  
  // Water Temperature
  Serial.print("🌊 Water Temp (Pin 44): ");
  waterTemp.requestTemperatures();
  float waterTempC = waterTemp.getTempCByIndex(0);
  if (waterTempC == -127.00) {
    Serial.println("❌ ERROR - Sensor not found");
  } else {
    Serial.print(waterTempC); Serial.println("°C");
  }
  
  // Weight
  Serial.print("⚖️ Weight (Pins 20,21): ");
  float weight = scale.get_units(5);
  Serial.print(weight, 3); Serial.println(" kg");
  
  Serial.println("================================\n");
}

// === เทสเซ็นเซอร์พลังงาน ===
void testPowerSensors() {
  Serial.println("\n🔋 TESTING POWER SENSORS...");
  Serial.println("================================");
  
  // อ่านค่า Raw ADC
  int solarVRaw = analogRead(SOLAR_VOLTAGE_PIN);
  int solarIRaw = analogRead(SOLAR_CURRENT_PIN);
  int loadVRaw = analogRead(LOAD_VOLTAGE_PIN);
  int loadIRaw = analogRead(LOAD_CURRENT_PIN);
  
  Serial.print("☀️ Solar Voltage (Pin A3): Raw="); Serial.print(solarVRaw);
  Serial.print(" -> "); Serial.print((solarVRaw / 1023.0) * vRef * vFactor, 2); Serial.println("V");
  
  Serial.print("☀️ Solar Current (Pin A4): Raw="); Serial.print(solarIRaw);
  float solarI = (((solarIRaw / 1023.0) * vRef) - zeroCurrentVoltage) / sensitivity;
  Serial.print(" -> "); Serial.print(solarI, 3); Serial.println("A");
  
  Serial.print("🔋 Load Voltage (Pin A1): Raw="); Serial.print(loadVRaw);
  Serial.print(" -> "); Serial.print((loadVRaw / 1023.0) * vRef * vFactor, 2); Serial.println("V");
  
  Serial.print("🔋 Load Current (Pin A0): Raw="); Serial.print(loadIRaw);
  float loadI = (((loadIRaw / 1023.0) * vRef) - zeroCurrentVoltage) / sensitivity;
  Serial.print(" -> "); Serial.print(loadI, 3); Serial.println("A");
  
  Serial.println("\n🔧 Constants:");
  Serial.print("vRef: "); Serial.println(vRef);
  Serial.print("vFactor: "); Serial.println(vFactor);
  Serial.print("sensitivity: "); Serial.println(sensitivity, 3);
  Serial.print("zeroCurrentVoltage: "); Serial.println(zeroCurrentVoltage, 3);
  
  Serial.println("================================\n");
}

// === ส่งข้อมูลเซ็นเซอร์ทั้งหมดผ่าน JSON ===
void sendAllSensorData() {
  StaticJsonDocument<1024> doc;
  
  // Environmental Sensors
  doc["sensors"]["dht_box"]["temperature"] = dhtBox.readTemperature();
  doc["sensors"]["dht_box"]["humidity"] = dhtBox.readHumidity();
  doc["sensors"]["dht_feed"]["temperature"] = dhtFeed.readTemperature();
  doc["sensors"]["dht_feed"]["humidity"] = dhtFeed.readHumidity();
  
  // Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  float soilPct = map(soilRaw, 300, 1023, 100, 0);
  soilPct = constrain(soilPct, 0, 100);
  doc["sensors"]["soil"]["moisture"] = soilPct;
  
  // Water Temperature
  waterTemp.requestTemperatures();
  float tempC = waterTemp.getTempCByIndex(0);
  doc["sensors"]["water_temp"]["temperature"] = (tempC == -127.00) ? 0 : tempC;
  
  // Weight
  float weight = scale.get_units(5); // ลด sample เพื่อประสิทธิภาพ
  doc["sensors"]["weight"]["value"] = weight;
  
  // Power Monitoring
  float solarV, solarI, loadV, loadI;
  readPowerSensors(solarV, solarI, loadV, loadI);
  doc["power"]["solar"]["voltage"] = solarV;
  doc["power"]["solar"]["current"] = solarI;
  doc["power"]["battery"]["voltage"] = loadV;
  doc["power"]["battery"]["current"] = loadI;
  doc["power"]["battery"]["percentage"] = estimateBatteryPercentage(loadV);
  
  // System Status
  doc["status"]["blower"]["running"] = isBlowerRunning;
  doc["status"]["blower"]["speed"] = blowerSpeed;
  doc["status"]["actuator"]["running"] = isActuatorRunning;
  doc["status"]["auger"]["running"] = isAugerRunning;
  doc["status"]["relay"]["relay1"] = relay1State;
  doc["status"]["relay"]["relay2"] = relay2State;
  
  Serial.print("[SEND] - ");
  serializeJson(doc, Serial);
  Serial.println();
}

void setup() {
  // Record system start time
  systemStartTime = millis();
  
  // Initialize Serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for Serial port to connect
  }
  
  if (PRODUCTION_MODE) {
    Serial.println("[INIT] Fish Feeder System Starting - PRODUCTION MODE");
    Serial.println("[INIT] Watchdog: " + String(ENABLE_WATCHDOG ? "ENABLED" : "DISABLED"));
  } else {
    Serial.println("=== Fish Feeder System Starting ===");
    Serial.println("🚀 Based on PASSIEE02 Original Code");
    Serial.println("📋 Simple Menu + JSON Commands Available");
    Serial.println("🔧 DEBUG MODE - Full logging enabled");
  }
  
  // Initialize watchdog timer if enabled
  if (ENABLE_WATCHDOG) {
    wdt_enable(WDTO_8S); // 8 second watchdog
    Serial.println("[INIT] Watchdog timer enabled - 8 seconds");
  }
  
  // Initialize sensors with error checking
  dhtBox.begin();
  dhtFeed.begin();
  waterTemp.begin();
  scale.begin(HX_DT, HX_SCK);
  
  // Check sensor initialization
  int deviceCount = waterTemp.getDeviceCount();
  if (deviceCount == 0) {
    Serial.println("[WARNING] No DS18B20 sensors found");
  } else {
    Serial.println("[INIT] Found " + String(deviceCount) + " DS18B20 sensor(s)");
  }
  
  // Load calibration from EEPROM
  EEPROM.get(EEPROM_SCALE_ADDR, scaleFactor);
  EEPROM.get(EEPROM_OFFSET_ADDR, offset);
  EEPROM.get(EEPROM_FEED_COUNT_ADDR, feedCount);
  
  // Validate EEPROM data
  if (scaleFactor == 0 || isnan(scaleFactor)) {
    scaleFactor = 1.0;
    Serial.println("[WARNING] Invalid scale factor, using default: 1.0");
  }
  
  scale.set_scale(scaleFactor);
  scale.set_offset(offset);
  
  // Initialize control pins
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(RELAY_IN1, OUTPUT);
  pinMode(RELAY_IN2, OUTPUT);
  pinMode(ACT_ENA, OUTPUT);
  pinMode(ACT_IN1, OUTPUT);
  pinMode(ACT_IN2, OUTPUT);
  pinMode(AUG_ENA, OUTPUT);
  pinMode(AUG_IN1, OUTPUT);
  pinMode(AUG_IN2, OUTPUT);
  
  // Set initial states (all OFF for safety)
  digitalWrite(RPWM, LOW);
  digitalWrite(LPWM, LOW);
  digitalWrite(ACT_ENA, LOW);
  digitalWrite(AUG_ENA, LOW);
  digitalWrite(RELAY_IN1, HIGH); // Active LOW relays
  digitalWrite(RELAY_IN2, HIGH);
  
  // Initialize state variables
  isBlowerRunning = false;
  isActuatorRunning = false;
  isAugerRunning = false;
  relay1State = false;
  relay2State = false;
  systemError = false;
  lastError = "";
  
  if (PRODUCTION_MODE) {
    Serial.println("[INIT] System initialized - Production ready");
    Serial.println("[INIT] Scale factor: " + String(scaleFactor, 6));
    Serial.println("[INIT] Feed count: " + String(feedCount));
  } else {
    Serial.println("✅ All systems initialized successfully");
    Serial.println("📡 Waiting for commands...");
    Serial.println();
    showSimpleMenu(); // แสดงเมนูเฉพาะ debug mode
  }
  
  // Send initial system status
  logSystemEvent("System started successfully");
  sendHeartbeat();
  
  // Reset watchdog for the first time
  resetWatchdog();
}

// === Loop แบบ Production ===
void loop() {
  resetWatchdog(); // Reset watchdog timer
  
  // Check for system errors
  if (systemError) {
    if (millis() % 10000 == 0) { // Every 10 seconds
      logSystemEvent("System in error state: " + lastError);
    }
    delay(100);
    return;
  }
  
  // Safety timeouts
  unsigned long currentTime = millis();
  
  // Blower safety timeout
  if (isBlowerRunning && (currentTime - blowerStartTime > SAFETY_TIMEOUT)) {
    controlBlower("stop");
    logSystemEvent("Blower stopped - safety timeout");
  }
  
  // Actuator safety timeout
  if (isActuatorRunning && (currentTime - actuatorStartTime > SAFETY_TIMEOUT)) {
    controlActuator("stop");
    logSystemEvent("Actuator stopped - safety timeout");
  }
  
  // Auger safety timeout
  if (isAugerRunning && (currentTime - augerStartTime > SAFETY_TIMEOUT)) {
    controlAuger("stop");
    logSystemEvent("Auger stopped - safety timeout");
  }
  
  // Process incoming commands
  processCommand();
  
  // Send sensor data at regular intervals
  if (currentTime - lastSensorRead >= SENSOR_INTERVAL) {
    // Check sensor health
    if (!checkSensorHealth()) {
      systemError = true;
      logSystemEvent("Sensor health check failed");
      emergencyShutdown();
      return;
    }
    
    if (!PRODUCTION_MODE) {
      displaySensorReadout();  // แสดงผลแบบสวยงาม (เฉพาะ debug)
    }
    sendAllSensorData();     // ส่ง JSON ให้ Raspberry Pi
    lastSensorRead = currentTime;
  }
  
  // Send heartbeat signal
  if (currentTime - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = currentTime;
  }
  
  // Small delay to prevent CPU hogging
  delay(PRODUCTION_MODE ? 50 : 10); // เพิ่ม delay ใน production
}
