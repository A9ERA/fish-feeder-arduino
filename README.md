# 🐟 Fish Feeder Arduino Mega 2560 Firmware

Arduino firmware สำหรับระบบให้อาหารปลาอัตโนมัติ IoT ที่ใช้ **Arduino Mega 2560** เป็นหลัก

## 📋 ฟีเจอร์หลัก

### 🔧 การควบคุมอุปกรณ์
- **Blower Motor**: ควบคุมพัดลมเป่าอาหาร (PWM Speed Control)
- **Linear Actuator**: ควบคุมการดันและดึงตัวขับ
- **Auger Motor**: ควบคุมเกลียวลำเลียงอาหาร (Forward/Reverse)
- **Relay Control**: ควบคุมรีเลย์สำหรับอุปกรณ์เสริม

### 📊 เซ็นเซอร์ที่รองรับ
- **DHT22** (x2): อุณหภูมิและความชื้น (ตู้ควบคุม + ถังอาหาร)
- **DS18B20**: เซ็นเซอร์อุณหภูมิน้ำแบบกันน้ำ
- **HX711 + Load Cell**: เซ็นเซอร์วัดน้ำหนักอาหาร (รองรับ Calibration)
- **Soil Moisture Sensor**: เซ็นเซอร์ความชื้นของเม็ดอาหาร
- **ACS712 Current Sensor**: ตรวจสอบกระแสไฟฟ้าแผงโซลาร์และโหลด

### 🤖 ฟีเจอร์อัตโนมัติ
- **Auto Feed Sequence**: ลำดับการให้อาหารอัตโนมัติ
- **JSON Command Interface**: รับคำสั่งและส่งข้อมูลแบบ JSON
- **Real-time Monitoring**: ส่งข้อมูลเซ็นเซอร์และสถานะอุปกรณ์
- **Emergency Stop**: หยุดอุปกรณ์ทั้งหมดในกรณีฉุกเฉิน
- **Load Cell Calibration**: คาลิเบรตเซ็นเซอร์น้ำหนักจากระยะไกล

## 🔌 Pin Assignment

### เซ็นเซอร์
| เซ็นเซอร์ | พิน | รายละเอียด |
|----------|------|-----------|
| DHT22 (Control Box) | 48 | อุณหภูมิ/ความชื้น ตู้ควบคุม |
| DHT22 (Feed Container) | 46 | อุณหภูมิ/ความชื้น ถังอาหาร |
| DS18B20 (Water) | 44 | อุณหภูมิน้ำ |
| HX711 DT | 20 | Load Cell Data |
| HX711 SCK | 21 | Load Cell Clock |
| Soil Moisture | A2 | ความชื้นเม็ดอาหาร |

### อุปกรณ์ควบคุม
| อุปกรณ์ | พิน | รายละเอียด |
|---------|------|-----------|
| Blower RPWM | 5 | PWM ขวา |
| Blower LPWM | 6 | PWM ซ้าย |
| Actuator ENA | 11 | Enable |
| Actuator IN1 | 12 | Direction 1 |
| Actuator IN2 | 13 | Direction 2 |
| Auger ENA | 8 | Enable |
| Auger IN1 | 9 | Direction 1 |
| Auger IN2 | 10 | Direction 2 |
| Relay IN1 | 50 | Relay 1 |
| Relay IN2 | 52 | Relay 2 |

### ระบบพลังงาน
| สัญญาณ | พิน | รายละเอียด |
|--------|------|-----------|
| Solar Voltage | A3 | แรงดันแผงโซลาร์ |
| Solar Current | A4 | กระแสแผงโซลาร์ |
| Battery Voltage | A1 | แรงดันแบตเตอรี่ |
| Load Current | A0 | กระแสโหลด |

## 📨 JSON Command Interface

### คำสั่งที่รองรับ

#### 📊 อ่านข้อมูลเซ็นเซอร์
```json
{"command": "get_sensors"}
```

#### 📋 ตรวจสอบสถานะระบบ
```json
{"command": "get_status"}
```

#### 🌀 ควบคุมพัดลม
```json
{"command": "control_blower", "action": "start", "speed": 255}
{"command": "control_blower", "action": "stop"}
```

#### 🦾 ควบคุม Actuator
```json
{"command": "control_actuator", "action": "up"}
{"command": "control_actuator", "action": "down"}
{"command": "control_actuator", "action": "stop"}
```

#### ⚙️ ควบคุม Auger
```json
{"command": "control_auger", "action": "forward", "speed": 200}
{"command": "control_auger", "action": "reverse", "speed": 200}
{"command": "control_auger", "action": "stop"}
```

#### 🔌 ควบคุม Relay
```json
{"command": "control_relay", "relay_id": 1, "action": "on"}
{"command": "control_relay", "relay_id": 2, "action": "off"}
```

#### 🍽️ ลำดับการให้อาหารอัตโนมัติ
```json
{"command": "auto_feed"}
```

#### ⚖️ คาลิเบรต Load Cell
```json
{"command": "calibrate_scale", "weight": 1.5}
```

#### 🛑 หยุดฉุกเฉิน
```json
{"command": "emergency_stop"}
```

## 🔄 Auto Feed Sequence

ลำดับการให้อาหารอัตโนมัติมี 4 ขั้นตอน:

1. **Actuator Up** (3 วินาที): ดันตัวขับขึ้นเพื่อเปิดทางอาหาร
2. **Auger Forward** (6 วินาที): หมุนเกลียวลำเลียงเพื่อขนส่งอาหาร
3. **Blower Start** (5 วินาที): เปิดพัดลมเป่าอาหารลงไปในน้ำ
4. **Actuator Down** (3 วินาที): ดึงตัวขับลงเพื่อปิดทางอาหาร

รวมเวลาทั้งหมด: **17 วินาที**

## 📦 การติดตั้ง

### 1. Requirements
- **Arduino IDE** หรือ **PlatformIO**
- **Arduino Mega 2560**
- **USB Cable** (Type-A to Type-B)

### 2. Libraries ที่ต้องการ
```ini
adafruit/DHT sensor library@^1.4.4
adafruit/Adafruit Unified Sensor@^1.1.6
bogde/HX711@^0.7.5
paulstoffregen/OneWire@^2.3.7
milesburton/DallasTemperature@^3.11.0
arduino-libraries/ArduinoJson@^6.21.3
```

### 3. การติดตั้งด้วย PlatformIO
```bash
# Clone project
git clone <repository-url>
cd fish-feeder-arduino

# Build และ Upload
pio run --target upload

# Monitor Serial
pio device monitor
```

### 4. การติดตั้งด้วย Arduino IDE
1. เปิดไฟล์ `src/main.cpp`
2. ติดตั้ง Libraries ที่จำเป็น
3. เลือก Board: **Arduino Mega 2560**
4. เลือก Port ที่ถูกต้อง
5. Upload โค้ด

## 🔧 การใช้งาน

### การเชื่อมต่อกับ Pi
1. เชื่อมต่อ Arduino กับ Raspberry Pi ผ่าน USB
2. ตั้งค่า Serial Port ใน Pi (ปกติ `/dev/ttyUSB0` หรือ `/dev/ttyACM0`)
3. ใช้ baud rate **115200**

### การทดสอบ
```bash
# ทดสอบการเชื่อมต่อ
echo '{"command": "get_status"}' > /dev/ttyUSB0

# อ่านข้อมูลเซ็นเซอร์
echo '{"command": "get_sensors"}' > /dev/ttyUSB0

# ทดสอบพัดลม
echo '{"command": "control_blower", "action": "start", "speed": 128}' > /dev/ttyUSB0
echo '{"command": "control_blower", "action": "stop"}' > /dev/ttyUSB0
```

## 🔍 การ Debug

### Serial Monitor
- เปิด Serial Monitor ที่ **115200 baud**
- ดูข้อมูล JSON ที่ส่งออกมา
- ตรวจสอบ error messages

### LED Indicators
- **Built-in LED (Pin 13)**: กะพริบเมื่อมีการติดต่อ Serial
- **TX/RX LEDs**: แสดงการส่ง/รับข้อมูล Serial

## ⚡ ข้อมูลทางเทคนิค

### การใช้ Memory
- **Flash Memory**: ~35KB (11% ของ Arduino Mega)
- **SRAM**: ~2KB (12% ของ Arduino Mega)
- **EEPROM**: ใช้เก็บค่า Calibration Load Cell

### Performance
- **Sensor Reading**: ทุก 10 วินาที
- **Heartbeat**: ทุก 5 วินาที  
- **Command Response**: < 100ms
- **JSON Processing**: < 50ms

### Power Consumption
- **Idle**: ~50mA
- **All Devices Active**: ~2-3A (ขึ้นอยู่กับอุปกรณ์)

## 🛠️ การปรับแต่ง

### เปลี่ยนระยะเวลา Feed Sequence
แก้ไขใน function `executeAutoFeedSequence()`:
```cpp
// Step 1: Actuator up (3 seconds)
delay(3000);  // เปลี่ยนเป็นเวลาที่ต้องการ
```

### ปรับ Sensor Sampling
แก้ไขค่าคงที่:
```cpp
#define SENSOR_SAMPLE_COUNT 50    // จำนวนครั้งในการ sampling
#define HEARTBEAT_INTERVAL 5000   // ระยะเวลา heartbeat (ms)
```

### เปลี่ยน Pin Assignment
แก้ไขค่าคงที่ในส่วน Pin Definition:
```cpp
#define DHTPIN_BOX 48          // เปลี่ยนพินตามต้องการ
#define RELAY_IN1 50           // เปลี่ยนพินตามต้องการ
```

## 🐛 Troubleshooting

### Arduino ไม่ตอบสนองคำสั่ง
1. ตรวจสอบการเชื่อมต่อ USB
2. ตรวจสอบ baud rate (ต้องเป็น 115200)
3. ตรวจสอบ JSON format ที่ส่งไป

### เซ็นเซอร์อ่านค่าผิดพลาด
1. ตรวจสอบการเชื่อมต่อสายไฟ
2. ตรวจสอบแรงดันไฟฟ้า (5V สำหรับเซ็นเซอร์ส่วนใหญ่)
3. ตรวจสอบพิน assignment

### Load Cell ไม่แม่นยำ
1. ทำการ Calibration ใหม่
2. ตรวจสอบการเชื่อมต่อ HX711
3. ใช้น้ำหนักมาตรฐานในการ Calibration

## 📞 การติดต่อ

หากมีปัญหาหรือข้อสงสัย กรุณาติดต่อทีมพัฒนา

---

**Fish Feeder Arduino Firmware v1.0**  
Developed with ❤️ for IoT Fish Feeding System 