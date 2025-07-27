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
- **Solenoid Valve** (for feeding mechanism)

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

### Solenoid Valve Control Commands

Control the feeding mechanism solenoid valve:

```
[control]:solenoid:open
[control]:solenoid:close
```

**Commands:**
- `open`: Open solenoid valve to dispense food
- `close`: Close solenoid valve (stops operation)

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

### Controlling Feeder Solenoid Valve
```
[control]:solenoid:open
[control]:solenoid:stop
[control]:solenoid:close
```

### Controlling Relays
```
[control]:relay:led:on
[control]:relay:fan:on
[control]:relay:all:off
```

### Automated Feeder Sequence
The Arduino now supports an automated feeding sequence that can be triggered with a single command from the Pi Server. The sequence uses weight-based control for precise feeding:

```
[control]:feeder:start:feedAmount:blowerDuration
[control]:feeder:stop
```

**Parameters:**
- `feedAmount`: Target weight reduction in grams
- `blowerDuration`: Duration in seconds for blower operation

**Fixed Timings:**
- Solenoid valve open: No time limit (controlled by weight reduction)
- Solenoid valve close: Immediate stop (no duration)

**Example:**
```
[control]:feeder:start:50:8
```
This will run the following automated sequence (blocking operation):
1. Start blower and open solenoid valve simultaneously
2. Wait for weight reduction of 50g (no time limit)
3. Close solenoid valve immediately (no duration)
4. Continue blower for remaining duration
5. Stop blower after total duration
6. Return when sequence is complete

**Weight Monitoring:**
- Checks weight every 100ms during feeding
- Uses 5g tolerance for weight measurement
- Maximum 30 seconds timeout for weight change
- Automatically stops sensor service printing during weight monitoring

**Emergency Stop:**
```
[control]:feeder:stop
```
This will interrupt the running sequence and immediately stop all feeder-related devices (solenoid valve, blower). The stop command is processed every 100ms during the sequence by calling `controlSensor()` within the delay loops, allowing for quick response even during blocking operations.

### Reversing Blower Direction
```
[control]:blower:direction:reverse
```

## System Configuration

- **Serial Communication**: 9600 baud rate
- **Loop Delay**: 5 seconds between sensor readings
- **Automated Feeder**: Supports multi-step feeding sequences with precise timing
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