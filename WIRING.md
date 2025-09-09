# Xbox Elite ESP32 RC Car - Wiring Guide

## Components Required

### Core Components
- ESP32 Development Board
- L298N Motor Driver Module
- 2x DC Motors (6-12V)
- 7.4V Li-Po Battery or 4xAA Battery Pack
- Jumper Wires
- Breadboard or PCB

### Optional Components
- LED (external status indicator)
- Buzzer (audio feedback)
- Switch (power on/off)

## Pin Assignments

### ESP32 to L298N Motor Driver
```
ESP32 Pin  →  L298N Pin    →  Function
GPIO 13    →  ENA          →  Motor A Speed Control (PWM)
GPIO 5     →  IN1          →  Motor A Direction Control 1
GPIO 4     →  IN2          →  Motor A Direction Control 2
GPIO 14    →  ENB          →  Motor B Speed Control (PWM)
GPIO 27    →  IN3          →  Motor B Direction Control 1
GPIO 26    →  IN4          →  Motor B Direction Control 2
3.3V       →  VCC (Logic)  →  Logic Power (3.3V compatible)
GND        →  GND          →  Common Ground
```

### ESP32 Optional Connections
```
ESP32 Pin  →  Component     →  Function
GPIO 2     →  Built-in LED →  System Status (built-in)
GPIO 22    →  External LED →  Additional Status Indicator
GPIO 23    →  Buzzer (+)   →  Audio Feedback
GND        →  Buzzer (-)   →  Buzzer Ground
```

### L298N to Motors and Power
```
L298N Pin  →  Connection    →  Function
OUT1       →  Motor A (+)   →  Left Motor Positive
OUT2       →  Motor A (-)   →  Left Motor Negative  
OUT3       →  Motor B (+)   →  Right Motor Positive
OUT4       →  Motor B (-)   →  Right Motor Negative
VIN        →  Battery (+)   →  Motor Power (6-12V)
GND        →  Battery (-)   →  Power Ground
```

## Wiring Diagram

```
                    7.4V Li-Po Battery
                         |
                    +----+----+
                    |         |
                   VIN       GND
                    |         |
              ┌─────+─────────+─────┐
              │     L298N Motor     │
              │     Driver Module   │
              │                     │
              │ ENA ENB VCC GND     │
              │  |   |   |   |      │
              │ IN1 IN2 IN3 IN4     │
              │  |   |   |   |      │
              │OUT1OUT2OUT3OUT4     │
              └──+───+───+───+──────┘
                 |   |   |   |
            Motor A   |   |   Motor B
           (Left)     |   |   (Right)
                     |   |
         ESP32 Connections:
         GPIO 13 → ENA (PWM)
         GPIO 5  → IN1
         GPIO 4  → IN2
         GPIO 14 → ENB (PWM)
         GPIO 27 → IN3
         GPIO 26 → IN4
         3.3V    → VCC
         GND     → GND

         Optional Connections:
         GPIO 22 → LED (+) → GND
         GPIO 23 → Buzzer (+) → GND
```

## Power Distribution

### Power Requirements
- **ESP32**: 3.3V (regulated internally from 5V/USB or VIN)
- **L298N Logic**: 5V (but 3.3V compatible inputs)
- **Motors**: 6-12V (from main battery)
- **Total Current**: ~2-4A (depending on motors and load)

### Power Connection Options

#### Option 1: Single Battery (Recommended)
- Use 7.4V Li-Po battery
- Connect battery directly to L298N VIN/GND
- ESP32 powered via USB during development
- For standalone operation, use voltage regulator or separate 3.7V battery

#### Option 2: Dual Power Supply
- 7.4V Li-Po for motors (L298N VIN)
- Separate 3.7V Li-Po or power bank for ESP32
- Common ground connection required

## Safety Considerations

### Electrical Safety
1. **Always connect grounds first** before power connections
2. **Double-check polarity** on motors and power supplies
3. **Use appropriate wire gauge** for motor current (22-18 AWG recommended)
4. **Add fuse protection** on main power line (5A fuse recommended)

### Mechanical Safety
1. **Secure all connections** to prevent disconnection during operation
2. **Protect exposed wires** from moving parts
3. **Mount components securely** to chassis
4. **Ensure proper ventilation** for L298N heat dissipation

## Assembly Steps

### Step 1: Prepare Components
1. Test ESP32 with USB connection
2. Verify L298N module functionality
3. Check motor operation with direct battery connection
4. Test buzzer and LED components

### Step 2: Wire Power System
1. Connect battery to L298N VIN/GND
2. Add power switch if desired
3. Connect ESP32 ground to L298N ground
4. Test power distribution

### Step 3: Wire Control Signals
1. Connect ESP32 GPIO pins to L298N inputs (see table above)
2. Connect optional LED and buzzer
3. Verify all connections with multimeter

### Step 4: Connect Motors
1. Connect motors to L298N outputs
2. Test motor direction (swap wires if needed)
3. Secure all motor connections

### Step 5: Final Assembly
1. Mount all components to chassis
2. Organize and secure wiring
3. Perform final electrical tests
4. Upload and test code

## Troubleshooting

### Common Issues

**Motors don't respond:**
- Check power connections
- Verify GPIO pin assignments in code
- Test L298N enable pins (ENA/ENB)

**Wrong motor direction:**
- Swap motor wires on problematic motor
- Or modify code to invert direction logic

**Erratic behavior:**
- Check for loose connections
- Verify common ground connections
- Ensure adequate power supply capacity

**Controller won't connect:**
- Verify Bluepad32 library installation
- Check serial monitor for pairing messages
- Reset ESP32 and try pairing again

### Testing Points

**Voltage Tests:**
- Battery output: 7.4V (fresh) to 6.5V (low)
- L298N VCC: ~3.3V
- ESP32 VIN: ~5V (if powered externally)

**Signal Tests:**
- GPIO outputs: 0V (LOW) to 3.3V (HIGH)
- PWM signals: Variable duty cycle at ~1kHz
- Enable pins: Should show PWM signal during operation

## Maintenance

### Regular Checks
1. **Inspect connections** for looseness or corrosion
2. **Check battery voltage** and charging state
3. **Clean motor brushes** if using brushed DC motors
4. **Verify software updates** for Bluepad32 library

### Performance Optimization
1. **Calibrate controller deadzone** in software
2. **Adjust speed curves** for smoother control
3. **Fine-tune PWM frequency** for quieter operation
4. **Monitor system temperature** during extended use