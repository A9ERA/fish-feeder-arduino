# Fish Feeder Arduino Project

An Arduino-based automated fish feeding system with environmental monitoring and control capabilities.

## Hardware Requirements

- **Arduino Mega 2560** (primary microcontroller)
- **DHT Sensors** (temperature and humidity monitoring)
- **Soil Sensor** (soil moisture detection)
- **Water Temperature Sensor** (Dallas Temperature DS18B20)
- **Weight Sensor** (HX711 load cell amplifier)
- **Current Sensor** (ACS712)
- **Voltage Sensor**
- **Blower Motor** (for air circulation)
- **Actuator Motor** (for feeding mechanism)

## Software Dependencies

The project uses PlatformIO and requires the following libraries:

- `adafruit/DHT sensor library@^1.4.6`
- `adafruit/Adafruit Unified Sensor@^1.1.14`
- `paulstoffregen/OneWire@^2.3.8`
- `milesburton/DallasTemperature@^4.0.4`
- `bogde/HX711@^0.7.5`
- `bblanchon/ArduinoJson@^6.21.4`

## Setup and Installation

1. **Install PlatformIO** in your preferred IDE (VS Code recommended)
2. **Clone the repository** to your local machine
3. **Connect the Arduino Mega 2560** to your computer
4. **Upload the firmware** using PlatformIO
5. **Set serial monitor** to 9600 baud rate

```bash
pio run --target upload
pio device monitor --baud 9600
```

## Sensor Control Commands

The system accepts control commands via serial communication. All commands must be sent with the prefix `[control]:` and terminated with a newline character (`\n`).

### Blower Control Commands

Control the air circulation blower:

```
[control]:blower:start
[control]:blower:stop
[control]:blower:speed:100
[control]:blower:direction:reverse
[control]:blower:direction:normal
```

**Parameters:**
- `speed`: Integer value (0-255) for PWM speed control
- `direction`: `reverse` or `normal` for motor direction

### Actuator Motor Control Commands

Control the feeding mechanism actuator:

```
[control]:actuator:up
[control]:actuator:down
[control]:actuator:stop
```

### Auger Motor Control Commands

Control the food dispensing auger motor:

```
[control]:auger:forward
[control]:auger:backward
[control]:auger:stop
[control]:auger:speedtest
```

**Commands:**
- `forward`: Rotate auger forward to dispense food
- `backward`: Rotate auger backward
- `stop`: Stop auger motor
- `speedtest`: Run auger speed test sequence

### Relay Control Commands

Control LED and fan relays:

```
[control]:relay:led:on
[control]:relay:led:off
[control]:relay:fan:on
[control]:relay:fan:off
[control]:relay:all:off
```

**Commands:**
- `led:on/off`: Control LED relay
- `fan:on/off`: Control fan relay
- `all:off`: Turn off all relays

## Sensor Data Output

The system automatically reads and outputs sensor data in JSON format via serial communication. Data is prefixed with `[SEND] -` for easy parsing.

### Available Sensors

- **DHT System Sensor**: Temperature and humidity for system environment
- **DHT Feeder Sensor**: Temperature and humidity for feeder area
- **Soil Sensor**: Soil moisture levels
- **Water Temperature**: Water temperature monitoring
- **Weight Sensor**: Load cell readings for food quantity
- **Current Sensor**: Power consumption monitoring
- **Voltage Sensor**: System voltage monitoring

## Usage Examples

### Starting the Blower
```
[control]:blower:start
```

### Setting Blower Speed
```
[control]:blower:speed:150
```

### Controlling Feeder Actuator
```
[control]:actuator:up
[control]:actuator:stop
[control]:actuator:down
```

### Dispensing Food with Auger
```
[control]:auger:forward
[control]:auger:stop
```

### Controlling Relays
```
[control]:relay:led:on
[control]:relay:fan:on
[control]:relay:all:off
```

### Reversing Blower Direction
```
[control]:blower:direction:reverse
```

## System Configuration

- **Serial Communication**: 9600 baud rate
- **Loop Delay**: 5 seconds between sensor readings
- **Platform**: Arduino Mega 2560 (ATmega2560)
- **Framework**: Arduino

## Project Structure

```
fish-feeder-arduino/
├── src/
│   ├── main.cpp              # Main program entry point
│   ├── sensors/              # Sensor implementation files
│   └── services/
│       └── sensor_service.cpp # Sensor control and data handling
├── include/                  # Header files
├── lib/                      # Local libraries
├── platformio.ini           # PlatformIO configuration
└── README.md               # This file
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

[Add your license information here]

## Troubleshooting

- **No response to commands**: Check serial connection and baud rate (9600)
- **Sensor readings incorrect**: Verify wiring and sensor connections
- **Motor not responding**: Check power supply and motor connections
- **JSON parsing errors**: Ensure proper command formatting with `[control]:` prefix

## Support

For issues and questions, please open an issue in the repository or contact the development team. 