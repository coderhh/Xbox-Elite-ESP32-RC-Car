# Xbox Elite ESP32 Two-Wheel Balance Robot - Wiring Guide

## Components Required

### Core Components
- ESP32 Development Board
- Dual H-Bridge Motor Driver (L298N or DRV8833)
- 2x High-Torque DC Motors (12V with encoders)
- MPU6050 6-Axis IMU Sensor
- 2x Rotary Encoders
- 11.1V Li-Po Battery (3S for extended operation)
- Jumper Wires and Connectors
- Prototyping Board or Custom PCB

### Optional Components
- LED (external status indicator)
- Buzzer (audio feedback)
- Switch (power on/off)
- Voltage Regulator (5V/3.3V)
- Low-ESR Capacitors (for power filtering)

## Pin Assignments

### ESP32 to Motor Driver
```
ESP32 Pin  →  Motor Driver Pin  →  Function
GPIO 13    →  ENA               →  Left Motor Speed Control (PWM)
GPIO 5     →  IN1               →  Left Motor Direction Control 1
GPIO 4     →  IN2               →  Left Motor Direction Control 2
GPIO 14    →  ENB               →  Right Motor Speed Control (PWM)
GPIO 27    →  IN3               →  Right Motor Direction Control 1
GPIO 26    →  IN4               →  Right Motor Direction Control 2
3.3V       →  VCC (Logic)       →  Logic Power (3.3V compatible)
GND        →  GND               →  Common Ground
```

### ESP32 to MPU6050 IMU Sensor
```
ESP32 Pin  →  MPU6050 Pin  →  Function
GPIO 21    →  SDA          →  I2C Data Line
GPIO 22    →  SCL          →  I2C Clock Line
GPIO 19    →  INT          →  Interrupt Pin (optional)
3.3V       →  VCC          →  Power Supply
GND        →  GND          →  Ground
```

### ESP32 to Rotary Encoders
```
ESP32 Pin  →  Left Encoder  →  Function
GPIO 18    →  A             →  Left Encoder Channel A
GPIO 19    →  B             →  Left Encoder Channel B
3.3V       →  VCC           →  Power Supply
GND        →  GND           →  Ground

ESP32 Pin  →  Right Encoder →  Function
GPIO 16    →  A             →  Right Encoder Channel A
GPIO 17    →  B             →  Right Encoder Channel B
3.3V       →  VCC           →  Power Supply
GND        →  GND           →  Ground
```

### ESP32 Optional Connections
```
ESP32 Pin  →  Component     →  Function
GPIO 2     →  Built-in LED →  System Status (built-in)
GPIO 25    →  External LED →  Additional Status Indicator
GPIO 23    →  Buzzer (+)   →  Audio Feedback
GPIO 33    →  Battery Monitor →  Voltage monitoring (ADC)
GND        →  Buzzer (-)   →  Buzzer Ground
```

### Motor Driver to Motors and Power
```
Motor Driver Pin  →  Connection    →  Function
OUT1             →  Left Motor (+) →  Left Motor Positive
OUT2             →  Left Motor (-) →  Left Motor Negative  
OUT3             →  Right Motor (+) →  Right Motor Positive
OUT4             →  Right Motor (-) →  Right Motor Negative
VIN              →  Battery (+)    →  Motor Power (12V)
GND              →  Battery (-)    →  Power Ground
```

### Encoder Connections
```
Left Encoder     →  Motor Shaft    →  Function
Encoder A        →  GPIO 18        →  Channel A Signal
Encoder B        →  GPIO 19        →  Channel B Signal
VCC              →  3.3V           →  Power Supply
GND              →  GND            →  Ground

Right Encoder    →  Motor Shaft    →  Function
Encoder A        →  GPIO 16        →  Channel A Signal
Encoder B        →  GPIO 17        →  Channel B Signal
VCC              →  3.3V           →  Power Supply
GND              →  GND            →  Ground
```

## Wiring Diagram

```
                    11.1V Li-Po Battery (3S)
                         |
                    +----+----+
                    |         |
                   VIN       GND
                    |         |
              ┌─────+─────────+─────┐
              │   Motor Driver      │
              │   (L298N/DRV8833)   │
              │                     │
              │ ENA ENB VCC GND     │
              │  |   |   |   |      │
              │ IN1 IN2 IN3 IN4     │
              │  |   |   |   |      │
              │OUT1OUT2OUT3OUT4     │
              └──+───+───+───+──────┘
                 |   |   |   |
            Left Motor  |   Right Motor
            (with Encoder) | (with Encoder)
                         |   |
         ┌───────────────+───+───────────────┐
         │              ESP32                │
         │                                   │
         │ Motor Control:                    │
         │ GPIO 13 → ENA (PWM)               │
         │ GPIO 5  → IN1                     │
         │ GPIO 4  → IN2                     │
         │ GPIO 14 → ENB (PWM)               │
         │ GPIO 27 → IN3                     │
         │ GPIO 26 → IN4                     │
         │                                   │
         │ IMU Sensor (MPU6050):             │
         │ GPIO 21 → SDA (I2C)               │
         │ GPIO 22 → SCL (I2C)               │
         │ GPIO 19 → INT                     │
         │                                   │
         │ Encoders:                         │
         │ GPIO 18 → Left Encoder A          │
         │ GPIO 19 → Left Encoder B          │
         │ GPIO 16 → Right Encoder A         │
         │ GPIO 17 → Right Encoder B         │
         │                                   │
         │ Status & Feedback:                │
         │ GPIO 2  → Built-in LED            │
         │ GPIO 25 → External LED            │
         │ GPIO 23 → Buzzer                  │
         │ GPIO 33 → Battery Monitor         │
         │                                   │
         │ Power:                            │
         │ 3.3V    → VCC (Logic)             │
         │ GND     → GND                     │
         └───────────────────────────────────┘
```

## Power Distribution

### Power Requirements
- **ESP32**: 3.3V (regulated internally from 5V/USB or VIN)
- **Motor Driver Logic**: 5V (but 3.3V compatible inputs)
- **Motors**: 12V (from main battery for high torque)
- **IMU Sensor**: 3.3V (I2C communication)
- **Encoders**: 3.3V (digital signals)
- **Total Current**: ~3-6A (depending on motors, balance load, and movement)

### Power Connection Options

#### Option 1: Single Battery (Recommended)
- Use 11.1V Li-Po battery (3S)
- Connect battery directly to motor driver VIN/GND
- ESP32 powered via USB during development
- For standalone operation, use voltage regulator (12V → 5V → 3.3V)

#### Option 2: Dual Power Supply
- 11.1V Li-Po for motors (motor driver VIN)
- Separate 3.7V Li-Po or power bank for ESP32 and sensors
- Common ground connection required
- Use voltage regulator for sensor power (5V/3.3V)

## Safety Considerations

### Electrical Safety
1. **Always connect grounds first** before power connections
2. **Double-check polarity** on motors, encoders, and power supplies
3. **Use appropriate wire gauge** for motor current (18-16 AWG recommended for 12V)
4. **Add fuse protection** on main power line (10A fuse recommended)
5. **Use shielded cables** for encoder signals to prevent interference
6. **Add pull-up resistors** on I2C lines (4.7kΩ recommended)

### Mechanical Safety
1. **Secure all connections** to prevent disconnection during operation
2. **Protect exposed wires** from moving parts and wheels
3. **Mount components securely** to chassis with vibration damping
4. **Ensure proper ventilation** for motor driver heat dissipation
5. **Mount IMU sensor securely** - any movement affects balance accuracy
6. **Protect encoders** from dust and mechanical damage

## Assembly Steps

### Step 1: Prepare Components
1. Test ESP32 with USB connection
2. Verify motor driver module functionality
3. Check motor operation with direct battery connection
4. Test IMU sensor with I2C scanner
5. Verify encoder functionality
6. Test buzzer and LED components

### Step 2: Wire Power System
1. Connect battery to motor driver VIN/GND
2. Add power switch if desired
3. Connect ESP32 ground to motor driver ground
4. Add voltage regulator for sensor power
5. Test power distribution

### Step 3: Wire Control Signals
1. Connect ESP32 GPIO pins to motor driver inputs (see table above)
2. Connect IMU sensor via I2C (SDA/SCL)
3. Connect encoder signals with shielded cables
4. Connect optional LED and buzzer
5. Verify all connections with multimeter

### Step 4: Connect Motors and Encoders
1. Connect motors to motor driver outputs
2. Mount encoders to motor shafts
3. Connect encoder signals to ESP32
4. Test motor direction (swap wires if needed)
5. Test encoder functionality
6. Secure all motor and encoder connections

### Step 5: Final Assembly
1. Mount all components to balance robot chassis
2. Ensure IMU sensor is mounted securely and level
3. Organize and secure wiring with cable management
4. Perform final electrical tests
5. Upload and test code
6. Perform IMU calibration

## Troubleshooting

### Common Issues

**Motors don't respond:**
- Check power connections
- Verify GPIO pin assignments in code
- Test motor driver enable pins (ENA/ENB)
- Ensure system is armed (press A button)

**Balance system not working:**
- Check IMU sensor connections (I2C)
- Verify IMU sensor is mounted securely
- Perform IMU calibration (press Y button)
- Check that robot is on level surface

**Wrong motor direction:**
- Swap motor wires on problematic motor
- Or modify code to invert direction logic

**Encoder issues:**
- Check encoder connections and power
- Verify encoder mounting on motor shaft
- Test encoder signals with oscilloscope
- Check for electrical interference

**Erratic behavior:**
- Check for loose connections
- Verify common ground connections
- Ensure adequate power supply capacity
- Check IMU sensor mounting stability

**Controller won't connect:**
- Verify Bluepad32 library installation
- Check serial monitor for pairing messages
- Reset ESP32 and try pairing again

### Testing Points

**Voltage Tests:**
- Battery output: 11.1V (fresh) to 9.9V (low)
- Motor driver VCC: ~3.3V
- ESP32 VIN: ~5V (if powered externally)
- IMU sensor VCC: 3.3V
- Encoder VCC: 3.3V

**Signal Tests:**
- GPIO outputs: 0V (LOW) to 3.3V (HIGH)
- PWM signals: Variable duty cycle at ~1kHz
- Enable pins: Should show PWM signal during operation
- I2C signals: SDA/SCL should show digital signals
- Encoder signals: Should show quadrature signals during motor rotation

**IMU Sensor Tests:**
- I2C communication: Use I2C scanner to verify address (0x68)
- Sensor data: Check for valid accelerometer/gyroscope readings
- Calibration: Verify sensor responds to movement

## Maintenance

### Regular Checks
1. **Inspect connections** for looseness or corrosion
2. **Check battery voltage** and charging state
3. **Clean motor brushes** if using brushed DC motors
4. **Verify software updates** for Bluepad32 library
5. **Check IMU sensor mounting** for stability
6. **Clean encoder sensors** from dust and debris
7. **Verify encoder alignment** on motor shafts

### Performance Optimization
1. **Calibrate controller deadzone** in software
2. **Adjust speed curves** for smoother control
3. **Fine-tune PWM frequency** for quieter operation
4. **Monitor system temperature** during extended use
5. **Recalibrate IMU sensors** periodically
6. **Tune PID parameters** for optimal balance performance
7. **Check encoder resolution** and adjust if needed