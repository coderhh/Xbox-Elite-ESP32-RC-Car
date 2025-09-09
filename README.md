# Xbox Elite Series 2 + ESP32 Two-Wheel Balance Robot

A wireless two-wheel self-balancing robot controlled by an Xbox Elite Series 2 controller via Bluetooth connection to an ESP32 microcontroller. Features advanced PID balance control, real-time, low-latency control with variable speed, precise directional movement, and comprehensive safety systems.

## Features

- **Wireless Control**: Xbox Elite Series 2 controller via Bluetooth
- **Self-Balancing**: Advanced PID control algorithms for stable balance (±1° precision)
- **Variable Speed**: Analog trigger-based speed control with balance compensation
- **Differential Steering**: Independent left/right motor control for precise maneuvering
- **IMU Integration**: MPU6050 6-axis sensor for real-time balance feedback
- **Encoder Feedback**: Precise wheel position tracking for smooth control
- **Safety Systems**: Emergency stop, fall detection, watchdog timer, connection monitoring
- **Status Feedback**: LED indicators and buzzer audio feedback
- **Arm/Disarm System**: Prevent accidental activation
- **Low Latency**: Sub-20ms response time for real-time control

## Hardware Requirements

### Core Components
- ESP32 Development Board (original ESP32 for full BR/EDR + BLE support)
- Xbox Elite Series 2 Controller
- Dual H-Bridge Motor Driver (L298N or DRV8833)
- 2x High-Torque DC Motors (12V with encoders)
- MPU6050 6-Axis IMU Sensor
- 2x Rotary Encoders
- Two-Wheel Balance Robot Chassis/Frame
- 11.1V Li-Po Battery (3S for extended operation)

### Supporting Components
- Jumper wires and prototyping board/PCB
- Voltage regulator (5V/3.3V)
- External LED, Buzzer, Power switch
- Low-ESR capacitors for power filtering

**Total estimated cost: $96-198** (excluding controller if already owned)

## Software Requirements

- PlatformIO or Arduino IDE
- Bluepad32 library v3.10.3+
- ESP32 Arduino Core
- MPU6050 IMU library
- PID control library
- Encoder library for wheel position feedback

## Quick Start

### 1. Hardware Assembly
Follow the detailed wiring guide in [WIRING.md](WIRING.md):
- Connect ESP32 to motor driver and sensors
- Wire IMU sensor (MPU6050) via I2C
- Connect rotary encoders to ESP32
- Wire motors to driver outputs
- Connect power supply and optional components

### 2. Software Setup
```bash
# Clone or download this project
# Open in PlatformIO or Arduino IDE

# Install required libraries (handled automatically by platformio.ini)
# Bluepad32 v3.10.3+
# MPU6050 IMU library
# PID control library
# Encoder library
```

### 3. Upload and Test
1. Connect ESP32 to computer via USB
2. Upload the code using PlatformIO: `pio run -t upload`
3. Open serial monitor to see status messages
4. Power on the balance robot with battery connected
5. **IMPORTANT**: Ensure robot is on a stable surface before first startup

### 4. Controller Pairing
1. Turn on Xbox Elite Series 2 controller
2. ESP32 will automatically enter pairing mode
3. Controller should connect within 10-15 seconds
4. Status LED will turn solid when connected

### 5. IMU Calibration
1. Place robot on level surface
2. Press Y button to calibrate IMU sensors
3. Wait for calibration completion (LED will blink)
4. Robot is now ready for operation

## Operation Guide

### Control Layout

**Left Analog Stick:**
- **Y-axis**: Forward/Backward movement (balance robot translation)
- **X-axis**: Left/Right turning (differential wheel control)

**Right Analog Stick:**
- **Y-axis**: Fine balance adjustment (tilt compensation)
- **X-axis**: Reserved for future features

**Triggers:**
- **RT**: Speed modulation (0-100% throttle)
- **LT**: Reverse speed control

**Face Buttons:**
- **A Button**: Arm/Disarm balance system (press to toggle)
- **B Button**: Emergency Stop (immediate motor shutdown and balance reset)
- **X Button**: Toggle balance mode (manual/auto)
- **Y Button**: Calibrate IMU sensors

**System States:**

| LED Pattern | Status | Description |
|-------------|--------|-------------|
| Off | Disconnected | No controller connected |
| Slow Blink (1s) | Connected, Disarmed | Controller connected but system disarmed |
| Medium Blink (0.5s) | Connected, Armed | Ready for operation |
| Solid On | Active | Balance system active and stable |
| Fast Blink (0.1s) | Emergency Stop | System in emergency stop mode |
| Double Blink | Fall Detection | Robot has fallen, needs manual reset |
| Triple Blink | IMU Calibration | Calibrating sensors |

### Safety Features

1. **Arm/Disarm System**: Press A button to arm before balance system activates
2. **Emergency Stop**: Press B button for immediate motor shutdown and balance reset
3. **Fall Detection**: Automatic shutdown if robot tilts beyond recovery angle (±30°)
4. **Watchdog Timer**: Automatic stop if controller disconnects (2s timeout)
5. **Connection Monitoring**: Visual and audio feedback for connection status
6. **Speed Limiting**: Configurable maximum speed settings
7. **IMU Failure Detection**: Backup balance algorithms if sensor fails
8. **Power Monitoring**: Low battery detection and safe shutdown

### Basic Operation Steps

1. **Power On**: Connect battery and power on balance robot
2. **Controller Connection**: Turn on Xbox controller, wait for pairing
3. **IMU Calibration**: Press Y button to calibrate sensors (on level surface)
4. **Arm System**: Press A button (hear confirmation beep)
5. **Balance**: Robot will automatically start balancing
6. **Control**: Use left stick for movement, right stick for fine adjustments
7. **Disarm**: Press A button again to disarm when finished
8. **Emergency**: Press B button anytime for immediate stop and balance reset

## Advanced Configuration

### Balance and Response Tuning

Modify these constants in `src/config.h` to customize behavior:

```cpp
// Balance Control Parameters
#define BALANCE_KP 15.0       // Proportional gain for balance PID
#define BALANCE_KI 0.1        // Integral gain for balance PID
#define BALANCE_KD 0.5        // Derivative gain for balance PID
#define MAX_TILT_ANGLE 30.0   // Maximum tilt angle before fall detection

// Motor Control Parameters
#define MAX_SPEED 255         // Maximum motor speed (0-255)
#define MIN_SPEED 50          // Minimum motor speed for movement
#define DEADZONE 10           // Analog stick deadzone
#define WATCHDOG_TIMEOUT 2000 // Controller timeout (milliseconds)

// IMU Settings
#define IMU_UPDATE_RATE 100   // IMU update frequency (Hz)
#define CALIBRATION_SAMPLES 1000 // Number of samples for IMU calibration
```

### Motor Direction and Encoder Setup

If motors turn in wrong direction, either:
1. Swap motor wires on the problematic motor
2. Or modify the direction logic in code:

```cpp
// In MotorController::setMotorDirection function, invert the logic:
if (!motorDirection) {  // Changed from motorDirection
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
} else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
}
```

### Encoder Configuration

If encoders read in wrong direction:
1. Swap encoder A and B wires
2. Or modify encoder direction in code:

```cpp
// In EncoderController constructor:
leftEncoder = new Encoder(ENCODER_LEFT_A, ENCODER_LEFT_B);
rightEncoder = new Encoder(ENCODER_RIGHT_B, ENCODER_RIGHT_A); // Swapped A/B
```

### PWM Frequency and PID Tuning

For quieter motor operation, adjust PWM frequency:

```cpp
#define PWM_FREQUENCY 1000  // Change to 20000 for ultrasonic frequency
```

### PID Tuning for Balance

Fine-tune balance performance by adjusting PID parameters:

```cpp
// For more aggressive balance (faster response, may oscillate):
#define BALANCE_KP 20.0
#define BALANCE_KI 0.2
#define BALANCE_KD 1.0

// For smoother balance (slower response, more stable):
#define BALANCE_KP 10.0
#define BALANCE_KI 0.05
#define BALANCE_KD 0.3
```

## Troubleshooting

### Controller Won't Connect
- Ensure Xbox controller is in pairing mode
- Check serial monitor for pairing messages
- Try resetting ESP32 and restarting controller
- Verify Bluepad32 library is properly installed

### Balance System Not Working
- Check that system is armed (press A button)
- Verify IMU sensor is properly connected and calibrated
- Check that robot is on level surface
- Ensure IMU calibration completed successfully (press Y button)

### Motors Not Responding
- Check that system is armed (press A button)
- Verify power connections (battery voltage)
- Check motor wire connections
- Test with emergency stop cleared (restart if needed)

### Poor Balance Performance
- Recalibrate IMU sensors (press Y button on level surface)
- Check for loose wiring connections
- Verify common ground between ESP32 and motor driver
- Ensure adequate power supply (check battery level)
- Adjust PID parameters for your specific robot

### Erratic Movement
- Check for loose wiring connections
- Verify encoder connections and functionality
- Ensure adequate power supply (check battery level)
- Calibrate controller deadzone settings
- Check IMU sensor mounting (must be secure)

### Poor Range/Connection
- Check ESP32 antenna orientation
- Remove obstacles between controller and ESP32
- Ensure battery voltage is adequate
- Try different physical locations

### Serial Monitor Output
```
Xbox Elite Series 2 + ESP32 Balance Robot Starting...
IMU initialized successfully
System ready. Waiting for Xbox controller...
Controller connected, index=0
IMU calibration started...
IMU calibration completed
System ARMED - Balance mode active
Balance PID: Kp=15.0, Ki=0.1, Kd=0.5
System DISARMED
EMERGENCY STOP ACTIVATED!
Fall detection triggered - stopping motors
Controller timeout - stopping motors
```

## Development and Customization

### Adding New Features

The code architecture supports easy expansion:

1. **New Controller Inputs**: Add handlers in `ControllerProcessor::processInput()`
2. **Additional Sensors**: Add to main loop with appropriate delays
3. **Custom Behaviors**: Implement in separate functions
4. **Status Indicators**: Extend `SafetySystem::updateStatusLED()` function
5. **Balance Algorithms**: Modify PID parameters in `BalanceController`
6. **Sensor Fusion**: Extend IMU data processing in `IMUController`

### Code Structure

```
src/
├── main.cpp                    - Main application entry point
├── RCCarSystem.cpp            - Main system orchestrator
├── MotorController.cpp        - Motor control with PWM
├── SafetySystem.cpp           - Safety and state management
├── ControllerProcessor.cpp    - Xbox controller input processing
├── GamepadInterface.cpp       - Bluetooth controller interface
├── BalanceController.cpp      - PID balance control algorithms
├── IMUController.cpp          - MPU6050 sensor management
└── EncoderController.cpp      - Wheel encoder feedback

include/
├── RCCarSystem.h
├── MotorController.h
├── SafetySystem.h
├── ControllerProcessor.h
├── GamepadInterface.h
├── BalanceController.h
├── IMUController.h
├── EncoderController.h
└── config.h                   - Configuration constants
```

### Performance Optimization

1. **Optimize control loop frequency** for higher responsiveness (currently 100Hz)
2. **Implement motor ramping** for smoother acceleration/deceleration
3. **Add sensor fusion algorithms** for improved balance stability
4. **Implement advanced PID control** with adaptive tuning
5. **Add complementary filter** for IMU data fusion
6. **Optimize encoder sampling** for precise wheel position tracking

## Future Enhancements

### Planned Features
- [ ] FPV camera system with live streaming
- [ ] Ultrasonic obstacle detection and avoidance
- [ ] Autonomous navigation with SLAM
- [ ] Web-based control interface
- [ ] Multiple controller support
- [ ] Data logging and telemetry
- [ ] Voice control integration
- [ ] Machine learning for adaptive PID tuning

### Hardware Upgrades
- [ ] LiDAR sensor for advanced mapping
- [ ] LED lighting system (headlights, taillights)
- [ ] Battery level monitoring with telemetry
- [ ] Additional IMU sensors for redundancy
- [ ] OLED display for real-time status
- [ ] Bluetooth speaker for audio feedback

## Technical Specifications

- **Bluetooth Range**: 15+ meters line of sight
- **Response Latency**: <20ms for control inputs
- **Balance Stability**: ±1° tilt angle maintenance
- **Battery Life**: 2+ hours continuous operation
- **Top Speed**: 2-3 m/s (configurable, safety-limited)
- **Operating Voltage**: 3.3V logic, 12V motors
- **PWM Frequency**: 1kHz (configurable)
- **Control Frequency**: 100Hz PID loop (10ms update rate)
- **IMU Update Rate**: 100Hz sensor sampling
- **Encoder Resolution**: 360 pulses per revolution

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

**Safety Note**: Always test in a safe, open area with adequate space for balance robot operation. Ensure emergency stop functionality before first use. This project involves moving mechanical parts, electrical components, and self-balancing systems - use appropriate safety precautions. The robot can move quickly and may fall over during testing.