#include "../../../include/weight_sensor.h"

HX711 scale;

void initWeight() {
  Serial.println("📦 เริ่มต้นระบบชั่งน้ำหนัก...");
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(FIXED_SCALE_FACTOR);
  
  // Read offset from EEPROM
  long storedOffset;
  EEPROM.get(EEPROM_OFFSET_ADDR, storedOffset);
  
  // Check if valid offset exists (not default EEPROM value)
  if (storedOffset != -1 && storedOffset != 0xFFFFFFFF) {
    scale.set_offset(storedOffset);
    Serial.print("📏 โหลดค่า offset จาก EEPROM: ");
    Serial.println(storedOffset);
  } else {
    Serial.println("⚠️  ไม่พบค่า offset ใน EEPROM - ใช้ค่าเริ่มต้น");
  }
  
  Serial.println("✅ ระบบชั่งน้ำหนักพร้อมใช้งาน");
}

StaticJsonDocument<256> readWeight() {
  StaticJsonDocument<256> doc;
  doc["name"] = WEIGHT_SENSOR;
  JsonArray values = doc.createNestedArray("value");

  float weight = scale.get_units(10);

  JsonObject weightValue = values.createNestedObject();
  weightValue["type"] = "weight";
  weightValue["unit"] = "kg";
  weightValue["value"] = round(weight * 1000.0) / 1000.0;

  return doc;
}

void calibrateWeight() {
  Serial.println("🔧 HX711 Weight Calibration");
  Serial.println("📏 กำลังเริ่มต้นระบบ scale...");

  Serial.println("📊 ข้อมูลก่อนการตั้งค่า scale:");
  Serial.print("   ↪ read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("   ↪ read average: \t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("   ↪ get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("   ↪ get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided
						// by the SCALE parameter (not set yet)

  // Set scale factor and tare
  scale.set_scale(FIXED_SCALE_FACTOR);          // set scale factor to fixed value
  scale.tare();				        // reset the scale to 0
  
  // Get and save offset to EEPROM
  long currentOffset = scale.get_offset();
  EEPROM.put(EEPROM_OFFSET_ADDR, currentOffset);
  
  Serial.print("💾 บันทึกค่า offset ลง EEPROM: ");
  Serial.println(currentOffset);

  Serial.println("✅ หลังจากตั้งค่า scale:");
  Serial.print("   ↪ read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("   ↪ read average: \t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("   ↪ get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("   ↪ get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
						// by the SCALE parameter set with set_scale

  Serial.println("🎯 การตั้งค่า scale เสร็จสิ้น!");
  Serial.print("📏 Scale Factor: ");
  Serial.println(FIXED_SCALE_FACTOR, 1);
  
  // Test readings
  Serial.println("🧪 ทดสอบการอ่านค่า:");
  Serial.print("   ↪ one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);
}