# Xbox Elite ESP32 RC Car - Test Documentation

## Overview

This project follows Test-Driven Development (TDD) principles with comprehensive unit and integration tests. The codebase is structured to be highly testable with proper separation of concerns and dependency injection.

## Test Architecture

### Test Framework
- **Unity**: C testing framework optimized for embedded systems
- **Native Testing**: Tests run on the host machine for fast feedback
- **Hardware Mocking**: Mock objects simulate ESP32 hardware for testing without physical devices

### Test Categories

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test complete workflows and component interactions
3. **Hardware Abstraction**: Mock objects enable testing without physical hardware

## Test Structure

```
test/
├── test_motor_controller.cpp    # Motor control unit tests
├── test_controller_processor.cpp # Input processing unit tests
├── test_safety_system.cpp      # Safety system unit tests
├── test_integration.cpp         # End-to-end integration tests
└── mocks/
    ├── MockArduino.h           # Arduino framework mocks
    ├── MockArduino.cpp
    ├── MockBluepad32.h         # Bluepad32 library mocks
    └── MockBluepad32.cpp
```

## Running Tests

### Prerequisites
- PlatformIO installed
- Native platform support (automatic with PlatformIO)

### Execute All Tests
```bash
# Run all tests on native platform
pio test -e native

# Run specific test file
pio test -e native -f test_motor_controller

# Run with verbose output
pio test -e native -v
```

### ESP32 Hardware Tests (Optional)
```bash
# Run tests on actual ESP32 hardware
pio test -e esp32dev_test --upload-port /dev/cu.usbserial*
```

## Test Coverage

### MotorController Tests (test_motor_controller.cpp)

**Features Tested:**
- ✅ Motor initialization and configuration
- ✅ Individual motor speed and direction control
- ✅ Dual motor coordination
- ✅ Speed constraints and validation
- ✅ Motor enable/disable functionality
- ✅ Emergency stop behavior

**Test Cases:**
```cpp
test_motor_controller_initialization()    // Verify clean startup
test_set_left_motor_forward()            // Left motor forward motion
test_set_left_motor_reverse()            // Left motor reverse motion
test_set_right_motor_forward()           // Right motor forward motion
test_set_both_motors()                   // Coordinated motor control
test_speed_constraints()                 // Speed limiting validation
test_stop_all_motors()                   // Emergency stop functionality
test_enable_disable_motors()             // Motor enable/disable
test_zero_speed_disables_motor()         // Automatic motor disable
```

### ControllerProcessor Tests (test_controller_processor.cpp)

**Features Tested:**
- ✅ Xbox controller input mapping
- ✅ Deadzone filtering
- ✅ Tank-style steering calculations
- ✅ Speed modulation via triggers
- ✅ Direction and turning logic
- ✅ Input validation and constraints

**Test Cases:**
```cpp
test_controller_processor_initialization() // Component setup
test_no_input_returns_stopped_motors()     // Default behavior
test_deadzone_filtering()                  // Input noise filtering
test_forward_movement()                    // Forward motion
test_backward_movement()                   // Reverse motion
test_right_turn_forward()                  // Right turn while moving
test_left_turn_forward()                   // Left turn while moving
test_pivot_right()                         // In-place right turn
test_pivot_left()                          // In-place left turn
test_trigger_speed_modulation()            // Speed control
test_speed_constraints()                   // Output limiting
test_set_deadzone()                        // Runtime configuration
test_set_speed_limits()                    // Runtime speed limits
```

### SafetySystem Tests (test_safety_system.cpp)

**Features Tested:**
- ✅ System state management
- ✅ Controller connection monitoring
- ✅ Arm/disarm functionality
- ✅ Emergency stop system
- ✅ Watchdog timer implementation
- ✅ Safety override conditions

**Test Cases:**
```cpp
test_safety_system_initialization()       // Clean startup state
test_controller_connection()               // Connection handling
test_controller_disconnection()            // Disconnect safety
test_arm_system()                         // System arming
test_disarm_system()                      // System disarming
test_cannot_arm_when_disconnected()       // Connection requirement
test_emergency_stop()                     // Emergency stop trigger
test_clear_emergency_stop_when_connected() // Emergency recovery
test_watchdog_timeout()                   // Timeout detection
test_watchdog_reset_on_activity()         // Activity monitoring
```

### Integration Tests (test_integration.cpp)

**Features Tested:**
- ✅ Complete system workflows
- ✅ Component interaction validation
- ✅ End-to-end functionality
- ✅ Real-world usage scenarios
- ✅ Safety system integration
- ✅ User interaction flows

**Test Cases:**
```cpp
test_system_initialization()              // System startup
test_controller_connection_workflow()     // Connection process
test_arm_disarm_workflow()                // User arming sequence
test_movement_control_workflow()          // Movement control
test_turning_workflow()                   // Turning operations
test_emergency_stop_workflow()            // Emergency procedures
test_emergency_stop_recovery_workflow()   // Recovery process
test_disarmed_prevents_movement()         // Safety enforcement
test_controller_disconnect_stops_motors() // Disconnect safety
test_deadzone_filtering_integration()     // Input filtering
test_speed_modulation_integration()       // Speed control
```

## Mocking System

### Hardware Abstraction
The test suite uses comprehensive mocking to simulate hardware dependencies:

**MockArduino.h/cpp**: 
- GPIO functions (pinMode, digitalWrite, digitalRead)
- PWM control (ledcSetup, ledcAttachPin, ledcWrite)
- Timing functions (millis, delay)
- Serial communication
- Mathematical utilities (map, constrain)

**MockBluepad32.h/cpp**:
- Xbox controller simulation
- Button and analog input simulation
- Connection state management
- Callback system testing

### Test-Specific Features
```cpp
// Mock controller input simulation
MockController& controller = BP32.getMockController();
controller.setAxisY(300);        // Simulate forward stick
controller.setThrottle(1023);    // Simulate full trigger
controller.setButtonA(true);     // Simulate button press

// Mock timing control
safetySystem->setCurrentTime(1000); // Control time for watchdog tests
```

## TDD Workflow

### 1. Red Phase (Write Failing Test)
```cpp
void test_new_feature(void) {
    // Arrange: Set up test conditions
    setupTestEnvironment();
    
    // Act: Perform the action being tested
    result = performNewFeature();
    
    // Assert: Verify expected behavior
    TEST_ASSERT_EQUAL(expected, result);
}
```

### 2. Green Phase (Make Test Pass)
Implement minimal code to make the test pass:
```cpp
// Implement just enough functionality
int performNewFeature() {
    return expected_value; // Minimal implementation
}
```

### 3. Refactor Phase (Improve Code)
Clean up implementation while keeping tests green:
```cpp
// Refactored, maintainable implementation
int performNewFeature() {
    // Proper implementation with good design
    return calculated_result;
}
```

## Continuous Integration

### GitHub Actions Workflow (.github/workflows/test.yml)
```yaml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-python@v4
        with:
          python-version: '3.9'
      - name: Install PlatformIO
        run: pip install platformio
      - name: Run Tests
        run: pio test -e native
```

### Local Pre-commit Hook
```bash
#!/bin/sh
# .git/hooks/pre-commit
echo "Running tests before commit..."
pio test -e native
if [ $? -ne 0 ]; then
    echo "Tests failed! Commit aborted."
    exit 1
fi
echo "All tests passed. Proceeding with commit."
```

## Test Best Practices

### Writing Effective Tests

1. **Follow AAA Pattern**: Arrange, Act, Assert
2. **One Concept Per Test**: Each test should verify one specific behavior
3. **Descriptive Names**: Test names should clearly indicate what is being tested
4. **Independent Tests**: Tests should not depend on each other
5. **Fast Execution**: Unit tests should run quickly for rapid feedback

### Test Naming Convention
```cpp
test_[component]_[behavior]_[expected_result]()

// Examples:
test_motor_controller_initialization_sets_default_state()
test_safety_system_emergency_stop_disables_motors()
test_controller_processor_deadzone_filters_small_inputs()
```

### Mock Usage Guidelines
```cpp
// DO: Use mocks to isolate the unit under test
void test_motor_speed_setting(void) {
    // Mock dependencies, test only motor controller logic
    motorController->setLeftMotor(150, true);
    TEST_ASSERT_EQUAL(150, motorController->getLeftMotorState().speed);
}

// DON'T: Test mock behavior instead of real logic
void test_mock_returns_value(void) {
    mockObject.setValue(100);
    TEST_ASSERT_EQUAL(100, mockObject.getValue()); // This tests the mock, not real code
}
```

## Debugging Failed Tests

### Common Debugging Steps

1. **Run Single Test**: Isolate the failing test
   ```bash
   pio test -e native -f test_specific_function
   ```

2. **Add Debug Output**: Use Unity's message functions
   ```cpp
   TEST_MESSAGE("Debug: Current state is %d", currentState);
   ```

3. **Check Mock State**: Verify mock objects are set up correctly
   ```cpp
   MockController& controller = BP32.getMockController();
   TEST_MESSAGE("Controller X axis: %d", controller.axisX());
   ```

4. **Verify Test Environment**: Ensure setUp() and tearDown() work correctly

### Memory Management
```cpp
void tearDown(void) {
    // Always clean up dynamically allocated objects
    if (testObject) {
        delete testObject;
        testObject = nullptr;
    }
}
```

## Performance Testing

### Timing Tests
```cpp
void test_response_time_under_threshold(void) {
    unsigned long startTime = getCurrentTime();
    
    processControllerInput(input);
    
    unsigned long endTime = getCurrentTime();
    unsigned long responseTime = endTime - startTime;
    
    TEST_ASSERT_LESS_THAN(50, responseTime); // <50ms response time
}
```

### Memory Usage Tests
```cpp
void test_memory_usage_within_limits(void) {
    size_t initialMemory = getFreeMemory();
    
    createLargeDataStructure();
    
    size_t finalMemory = getFreeMemory();
    size_t memoryUsed = initialMemory - finalMemory;
    
    TEST_ASSERT_LESS_THAN(MAX_ALLOWED_MEMORY, memoryUsed);
}
```

## Adding New Tests

### 1. Create Test File
```cpp
#include <unity.h>
#include "NewComponent.h"

NewComponent* component;

void setUp(void) {
    component = new NewComponent();
}

void tearDown(void) {
    delete component;
    component = nullptr;
}

void test_new_component_behavior(void) {
    // Test implementation
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_new_component_behavior);
    return UNITY_END();
}
```

### 2. Add to PlatformIO Test Discovery
Tests in the `test/` directory are automatically discovered by PlatformIO.

### 3. Update Documentation
Add new test descriptions to this documentation file.

## Test Metrics

### Coverage Goals
- **Unit Test Coverage**: >90% of non-hardware code
- **Integration Test Coverage**: All major user workflows
- **Safety System Coverage**: 100% of safety-critical paths

### Quality Metrics
- **Test Execution Time**: All tests <30 seconds
- **Test Reliability**: <1% flaky test rate
- **Maintenance**: Tests updated with code changes

---

## Troubleshooting

### Common Issues

**Tests fail on native platform but pass on hardware:**
- Check mock implementations match hardware behavior
- Verify timing dependencies in mocks

**Memory leaks in tests:**
- Ensure all dynamically allocated objects are freed in tearDown()
- Use valgrind on native platform: `valgrind pio test -e native`

**Inconsistent test results:**
- Check for static variables that persist between tests
- Ensure proper test isolation in setUp()/tearDown()

**Mock objects not behaving correctly:**
- Verify mock setup in setUp() function
- Check that mock state is reset between tests

This comprehensive test suite ensures the Xbox Elite ESP32 RC Car system is reliable, maintainable, and safe for operation.