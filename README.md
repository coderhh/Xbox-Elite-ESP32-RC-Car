# Xbox Elite Series 2 + ESP32 RC Car

A wireless RC car controlled by an Xbox Elite Series 2 controller via Bluetooth connection to an ESP32 microcontroller. Features real-time, low-latency control with variable speed, precise directional movement, and comprehensive safety systems.

## Features

- **Wireless Control**: Xbox Elite Series 2 controller via Bluetooth
- **Variable Speed**: Analog trigger-based speed control
- **Tank-Style Steering**: Independent left/right motor control for precise maneuvering
- **Safety Systems**: Emergency stop, watchdog timer, connection monitoring
- **Status Feedback**: LED indicators and buzzer audio feedback
- **Arm/Disarm System**: Prevent accidental activation
- **Low Latency**: Sub-50ms response time for real-time control

## Hardware Requirements

### Core Components
- ESP32 Development Board (original ESP32 for full BR/EDR + BLE support)
- Xbox Elite Series 2 Controller
- L298N Motor Driver Module
- 2x DC Motors (6-12V)
- RC Car Chassis/Frame
- 7.4V Li-Po Battery or 4xAA Battery Pack

### Supporting Components
- Jumper wires and breadboard/PCB
- Optional: External LED, Buzzer, Power switch

**Total estimated cost: $56-105** (excluding controller if already owned)

## Software Requirements

- PlatformIO or Arduino IDE
- Bluepad32 library v3.10.3+
- ESP32 Arduino Core

## Quick Start

### 1. Hardware Assembly
Follow the detailed wiring guide in [WIRING.md](WIRING.md):
- Connect ESP32 to L298N motor driver
- Wire motors to driver outputs
- Connect power supply and optional components

### 2. Software Setup
```bash
# Clone or download this project
# Open in PlatformIO or Arduino IDE

# Install required libraries (handled automatically by platformio.ini)
# Bluepad32 v3.10.3+
# ESP32Servo v0.13.0+
```

### 3. Upload and Test
1. Connect ESP32 to computer via USB
2. Upload the code using PlatformIO: `pio run -t upload`
3. Open serial monitor to see status messages
4. Power on the RC car with battery connected

### 4. Controller Pairing
1. Turn on Xbox Elite Series 2 controller
2. ESP32 will automatically enter pairing mode
3. Controller should connect within 10-15 seconds
4. Status LED will turn solid when connected

## Operation Guide

### Control Layout

**Left Analog Stick:**
- **Y-axis**: Forward/Backward movement
- **X-axis**: Left/Right turning (tank-style steering)

**Right Trigger:**
- **RT**: Speed modulation (0-100% throttle)

**Face Buttons:**
- **A Button**: Arm/Disarm system (press to toggle)
- **Y Button**: Emergency Stop (immediate motor shutdown)
- **B Button**: Horn/Buzzer activation
- **X Button**: Toggle external LED lights

**System States:**

| LED Pattern | Status | Description |
|-------------|--------|-------------|
| Off | Disconnected | No controller connected |
| Slow Blink (1s) | Connected, Disarmed | Controller connected but system disarmed |
| Medium Blink (0.5s) | Connected, Armed | Ready for operation |
| Solid On | Active | Motors can be controlled |
| Fast Blink (0.1s) | Emergency Stop | System in emergency stop mode |

### Safety Features

1. **Arm/Disarm System**: Press A button to arm before motors will respond
2. **Emergency Stop**: Press Y button for immediate motor shutdown
3. **Watchdog Timer**: Automatic stop if controller disconnects (2s timeout)
4. **Connection Monitoring**: Visual and audio feedback for connection status
5. **Speed Limiting**: Configurable maximum speed settings

### Basic Operation Steps

1. **Power On**: Connect battery and power on RC car
2. **Controller Connection**: Turn on Xbox controller, wait for pairing
3. **Arm System**: Press A button (hear confirmation beep)
4. **Control**: Use left stick for movement, right trigger for speed
5. **Disarm**: Press A button again to disarm when finished
6. **Emergency**: Press Y button anytime for immediate stop

## Advanced Configuration

### Speed and Response Tuning

Modify these constants in `src/main.cpp` to customize behavior:

```cpp
#define MAX_SPEED 255         // Maximum motor speed (0-255)
#define MIN_SPEED 50          // Minimum motor speed for movement
#define DEADZONE 10           // Analog stick deadzone
#define WATCHDOG_TIMEOUT 2000 // Controller timeout (milliseconds)
```

### Motor Direction

If motors turn in wrong direction, either:
1. Swap motor wires on the problematic motor
2. Or modify the direction logic in code:

```cpp
// In setMotorDirection function, invert the logic:
if (!motorADirection) {  // Changed from motorADirection
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
} else {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
}
```

### PWM Frequency Adjustment

For quieter motor operation, adjust PWM frequency:

```cpp
#define PWM_FREQUENCY 1000  // Change to 20000 for ultrasonic frequency
```

## Troubleshooting

### Controller Won't Connect
- Ensure Xbox controller is in pairing mode
- Check serial monitor for pairing messages
- Try resetting ESP32 and restarting controller
- Verify Bluepad32 library is properly installed

### Motors Not Responding
- Check that system is armed (press A button)
- Verify power connections (battery voltage)
- Check motor wire connections
- Test with emergency stop cleared (restart if needed)

### Erratic Movement
- Check for loose wiring connections
- Verify common ground between ESP32 and L298N
- Ensure adequate power supply (check battery level)
- Calibrate controller deadzone settings

### Poor Range/Connection
- Check ESP32 antenna orientation
- Remove obstacles between controller and ESP32
- Ensure battery voltage is adequate
- Try different physical locations

### Serial Monitor Output
```
Xbox Elite Series 2 + ESP32 RC Car Starting...
System ready. Waiting for Xbox controller...
Controller connected, index=0
System ARMED
System DISARMED
EMERGENCY STOP ACTIVATED!
Controller timeout - stopping motors
```

## Development and Customization

### Adding New Features

The code architecture supports easy expansion:

1. **New Controller Inputs**: Add handlers in `processControllerInput()`
2. **Additional Sensors**: Add to main loop with appropriate delays
3. **Custom Behaviors**: Implement in separate functions
4. **Status Indicators**: Extend `updateStatusLED()` function

### Code Structure

```
src/main.cpp
├── Setup Functions
│   ├── setupMotors()      - Initialize motor pins
│   ├── setupPWM()         - Configure PWM channels
│   └── setup()            - Main initialization
├── Control Functions
│   ├── processControllerInput() - Handle controller data
│   ├── setMotorSpeed()          - PWM speed control
│   └── setMotorDirection()      - Direction control
├── Safety Functions
│   ├── emergencyStopSystem()    - Emergency stop handler
│   ├── checkWatchdog()          - Connection monitoring
│   └── stopMotors()             - Safe motor shutdown
└── Feedback Functions
    ├── updateStatusLED()        - LED status management
    └── playTone()               - Audio feedback
```

### Performance Optimization

1. **Reduce main loop delay** for higher responsiveness (currently 10ms)
2. **Implement motor ramping** for smoother acceleration/deceleration
3. **Add gyroscope integration** for stability control
4. **Implement PID control** for precise movement

## Future Enhancements

### Planned Features
- [ ] FPV camera system with live streaming
- [ ] Ultrasonic obstacle detection and avoidance
- [ ] GPS tracking and return-to-home function
- [ ] Web-based control interface
- [ ] Multiple controller support
- [ ] Data logging and telemetry

### Hardware Upgrades
- [ ] Servo-based car-style steering
- [ ] LED lighting system (headlights, taillights)
- [ ] Battery level monitoring
- [ ] Gyroscope/accelerometer for stability

## Technical Specifications

- **Bluetooth Range**: 10+ meters line of sight
- **Response Latency**: <50ms for control inputs
- **Battery Life**: 1+ hours continuous operation
- **Top Speed**: Configurable, safety-limited
- **Operating Voltage**: 3.3V logic, 6-12V motors
- **PWM Frequency**: 1kHz (configurable)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly with hardware
5. Submit a pull request with detailed description

## License

This project is open source. See LICENSE file for details.

## Support

For issues, questions, or contributions:
- Check the troubleshooting section above
- Review wiring diagram in WIRING.md
- Open an issue with detailed problem description
- Include serial monitor output for debugging

---

**Safety Note**: Always test in a safe, open area. Ensure emergency stop functionality before first use. This project involves moving mechanical parts and electrical components - use appropriate safety precautions.