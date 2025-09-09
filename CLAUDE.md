# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview
This is an embedded IoT project that creates a two-wheel self-balancing robot controlled by an Xbox Elite Series 2 controller via Bluetooth through an ESP32 microcontroller. The codebase follows excellent embedded systems engineering practices with modular architecture, advanced PID control algorithms, and comprehensive testing.

## Essential Commands

### Development Workflow
```bash
# Build and upload to ESP32
pio run -t upload

# Monitor serial output
pio device monitor

# Build, upload, and monitor (combined)
make upload-monitor
make um  # shortcut

# Run all tests
pio test -e native -v
make test

# Pre-commit validation (run before committing)
make pre-commit

# Format code
make format
```

### Testing Commands
```bash
# Unit tests only
make test-unit

# Integration tests  
make test-integration

# Hardware tests (requires ESP32 connected)
make test-esp32

# Run specific test
pio test -e native -f test_motor_controller
```

### Build System
```bash
# Clean build
pio run -t clean

# Build without upload
pio run
make build

# Check code quality
make check

# Development environment setup
make dev-setup
```

## Architecture Overview

### Core Components (1200+ total lines of code)
- **RCCarSystem** (`src/RCCarSystem.cpp`, 400+ lines) - Main orchestrator managing all subsystems
- **MotorController** (`src/MotorController.cpp`, 180+ lines) - PWM motor control abstraction with safety limits
- **SafetySystem** (`src/SafetySystem.cpp`, 150+ lines) - Arm/disarm states, emergency stop, fall detection, watchdog timer
- **ControllerProcessor** (`src/ControllerProcessor.cpp`, 160+ lines) - Xbox controller input processing with deadzone handling
- **GamepadInterface** (`src/GamepadInterface.cpp`, 110+ lines) - Bluetooth controller interface using Bluepad32
- **BalanceController** (`src/BalanceController.cpp`, 200+ lines) - PID balance control algorithms and sensor fusion
- **IMUController** (`src/IMUController.cpp`, 150+ lines) - MPU6050 sensor management and data processing
- **EncoderController** (`src/EncoderController.cpp`, 120+ lines) - Wheel encoder feedback and position tracking

### Design Patterns
- **Dependency Injection**: Components accept interfaces for testing
- **Hardware Abstraction**: Mock objects in `test/mocks/` enable testing without hardware
- **Single Responsibility**: Each class has focused purpose
- **State Machine**: SafetySystem manages Armed/Disarmed/Emergency/Fallen states
- **Control Loop Pattern**: BalanceController implements real-time PID control
- **Sensor Fusion**: IMUController combines accelerometer and gyroscope data
- **Observer Pattern**: EncoderController provides position feedback to balance system

## Technology Stack

### Hardware Platform
- **ESP32** with PlatformIO framework
- **Bluepad32 library** (v3.10.3+) for Xbox controller communication
- **Dual H-Bridge motor driver** (L298N/DRV8833) controlled via PWM
- **MPU6050 IMU sensor** for balance feedback via I2C
- **Rotary encoders** for wheel position feedback
- **Unity framework** for embedded testing

### Build System
- **PlatformIO**: Primary build system with multiple environments (esp32, native testing)
- **Comprehensive Makefile**: 30+ commands for development workflow
- **GitHub Actions**: Automated CI/CD with testing and code quality checks

## Testing Architecture

### Test-Driven Development
The project uses Unity framework with complete hardware mocking:
- **Native tests**: Fast host-machine execution without hardware
- **Hardware tests**: ESP32-specific testing when device connected
- **Mocked dependencies**: Arduino and Bluepad32 APIs fully mocked in `test/mocks/`

### Key Test Categories
- Motor controller PWM logic and safety limits
- Controller input processing with deadzone handling  
- Safety system state transitions and watchdog behavior
- Balance control PID algorithms and stability
- IMU sensor data processing and calibration
- Encoder position tracking and feedback
- Integration tests for complete balance control workflows

## Key Implementation Details

### Control System
- **Self-balancing**: Advanced PID control algorithms for stable balance (±1° precision)
- **Differential steering**: Independent left/right motor control with balance compensation
- **Sub-20ms latency**: Real-time responsive control via Bluetooth
- **Variable speed**: Analog trigger input with configurable limits
- **Multiple inputs**: Analog sticks, face buttons, triggers supported
- **Sensor fusion**: IMU data combined with encoder feedback for precise control

### Safety Features
- **Arm/Disarm mechanism**: Prevents accidental motor activation
- **Emergency stop**: Immediate motor shutdown capability
- **Fall detection**: Automatic shutdown if robot tilts beyond recovery angle (±30°)
- **Watchdog timer**: 2-second timeout stops motors on connection loss
- **Connection monitoring**: Visual/audio feedback for controller status
- **IMU failure detection**: Backup balance algorithms if sensor fails
- **Power monitoring**: Low battery detection and safe shutdown

### Hardware Interface
- **PWM frequency**: 1kHz default for motor control
- **Pin assignments**: Defined in `src/config.h`
- **LED indicators**: Built-in ESP32 LED plus external status LEDs
- **Buzzer feedback**: Audio confirmation for state changes
- **I2C communication**: MPU6050 IMU sensor at address 0x68
- **Encoder interfaces**: Quadrature signals for wheel position feedback
- **ADC monitoring**: Battery voltage monitoring via GPIO 33

## Development Guidelines

### Code Organization
- All headers in `include/` with implementations in `src/`
- Test files mirror source structure in `test/`
- Mock objects provide hardware abstraction for testing
- Configuration constants centralized in `include/config.h`
- Balance control algorithms in dedicated `BalanceController` class
- Sensor management separated into `IMUController` and `EncoderController`

### Workflow Best Practices
1. **Always run tests** before committing: `make test`
2. **Use pre-commit validation**: `make pre-commit`
3. **Test without hardware** using native tests for rapid development
4. **Upload and monitor** during hardware testing: `make um`
5. **Calibrate IMU sensors** before testing balance functionality
6. **Test balance algorithms** in safe, open areas
7. **Monitor system performance** during extended operation

## Documentation
- **README.md**: Complete user guide with setup and troubleshooting
- **WIRING.md**: Hardware connections and pin assignments for balance robot
- **TEST.md**: Testing methodology and TDD practices
- **GitHub Pages**: Live documentation site in `/docs/`
- **Project Plan**: Comprehensive development roadmap in `xbox-esp32-rc-car-project-plan.md`