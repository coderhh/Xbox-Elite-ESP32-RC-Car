#include <unity.h>
#include "../test/mocks/MockArduino.h"
#include "../test/mocks/MockBluepad32.h"

// Include all test headers
#include "MotorController.h"
#include "ControllerProcessor.h"
#include "SafetySystem.h"
#include "RCCarSystem.h"

// Global test objects (will be set up by individual test suites)
MotorController* motorController = nullptr;
ControllerProcessor* controllerProcessor = nullptr;
SafetySystem* safetySystem = nullptr;
RCCarSystem* rcCarSystem = nullptr;

// ===== MOTOR CONTROLLER TESTS =====
void setUp_MotorController(void) {
    MotorController::MotorPins leftMotor = {13, 5, 4, 0};
    MotorController::MotorPins rightMotor = {14, 27, 26, 1};
    motorController = new MotorController(leftMotor, rightMotor);
    motorController->initialize();
}

void tearDown_MotorController(void) {
    delete motorController;
    motorController = nullptr;
}

void test_motor_controller_initialization(void) {
    setUp_MotorController();
    TEST_ASSERT_TRUE(motorController->areMotorsEnabled());
    TEST_ASSERT_EQUAL(0, motorController->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, motorController->getRightMotorState().speed);
    tearDown_MotorController();
}

void test_set_left_motor_forward(void) {
    setUp_MotorController();
    motorController->setLeftMotor(150, true);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    TEST_ASSERT_EQUAL(150, leftState.speed);
    TEST_ASSERT_TRUE(leftState.direction);
    TEST_ASSERT_TRUE(leftState.enabled);
    tearDown_MotorController();
}

void test_set_both_motors(void) {
    setUp_MotorController();
    motorController->setBothMotors(120, true, 80, false);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    MotorController::MotorState rightState = motorController->getRightMotorState();
    
    TEST_ASSERT_EQUAL(120, leftState.speed);
    TEST_ASSERT_TRUE(leftState.direction);
    TEST_ASSERT_EQUAL(80, rightState.speed);
    TEST_ASSERT_FALSE(rightState.direction);
    tearDown_MotorController();
}

void test_motor_enable_disable(void) {
    setUp_MotorController();
    motorController->enableMotors(false);
    TEST_ASSERT_FALSE(motorController->areMotorsEnabled());
    
    motorController->setLeftMotor(150, true);
    TEST_ASSERT_EQUAL(0, motorController->getLeftMotorState().speed);
    
    motorController->enableMotors(true);
    motorController->setLeftMotor(150, true);
    TEST_ASSERT_EQUAL(150, motorController->getLeftMotorState().speed);
    tearDown_MotorController();
}

void test_stop_all_motors(void) {
    setUp_MotorController();
    motorController->setBothMotors(150, true, 200, false);
    motorController->stopAllMotors();
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    MotorController::MotorState rightState = motorController->getRightMotorState();
    
    TEST_ASSERT_EQUAL(0, leftState.speed);
    TEST_ASSERT_EQUAL(0, rightState.speed);
    tearDown_MotorController();
}

// ===== CONTROLLER PROCESSOR TESTS =====
void setUp_ControllerProcessor(void) {
    controllerProcessor = new ControllerProcessor(10, 50, 255);
}

void tearDown_ControllerProcessor(void) {
    delete controllerProcessor;
    controllerProcessor = nullptr;
}

void test_controller_processor_initialization(void) {
    setUp_ControllerProcessor();
    TEST_ASSERT_EQUAL(10, controllerProcessor->getDeadzone());
    TEST_ASSERT_EQUAL(50, controllerProcessor->getMinSpeed());
    TEST_ASSERT_EQUAL(255, controllerProcessor->getMaxSpeed());
    tearDown_ControllerProcessor();
}

void test_no_input_returns_stopped_motors(void) {
    setUp_ControllerProcessor();
    ControllerInput input = {0, 0, 0, false, false, false, false};
    MotorCommand command = controllerProcessor->processInput(input);
    
    TEST_ASSERT_FALSE(command.valid);
    TEST_ASSERT_EQUAL(0, command.leftSpeed);
    TEST_ASSERT_EQUAL(0, command.rightSpeed);
    tearDown_ControllerProcessor();
}

void test_deadzone_filtering(void) {
    setUp_ControllerProcessor();
    ControllerInput input = {5, 5, 500, false, false, false, false}; // within deadzone of 10
    MotorCommand command = controllerProcessor->processInput(input);
    
    TEST_ASSERT_FALSE(command.valid);
    TEST_ASSERT_EQUAL(0, command.leftSpeed);
    TEST_ASSERT_EQUAL(0, command.rightSpeed);
    tearDown_ControllerProcessor();
}

void test_forward_movement(void) {
    setUp_ControllerProcessor();
    ControllerInput input = {0, 300, 1023, false, false, false, false};
    MotorCommand command = controllerProcessor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_GREATER_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(command.leftSpeed, command.rightSpeed);
    tearDown_ControllerProcessor();
}

void test_pivot_turn_right(void) {
    setUp_ControllerProcessor();
    ControllerInput input = {300, 0, 1023, false, false, false, false}; // pure right turn
    MotorCommand command = controllerProcessor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_LESS_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(abs(command.leftSpeed), abs(command.rightSpeed)); // equal and opposite
    tearDown_ControllerProcessor();
}

// ===== SAFETY SYSTEM TESTS =====
void setUp_SafetySystem(void) {
    safetySystem = new SafetySystem(2000);
    safetySystem->initialize();
}

void tearDown_SafetySystem(void) {
    delete safetySystem;
    safetySystem = nullptr;
}

void test_safety_system_initialization(void) {
    setUp_SafetySystem();
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
    tearDown_SafetySystem();
}

void test_controller_connection(void) {
    setUp_SafetySystem();
    safetySystem->setControllerConnected(true);
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, safetySystem->getSystemState());
    TEST_ASSERT_FALSE(safetySystem->isArmed());
    tearDown_SafetySystem();
}

void test_arm_system(void) {
    setUp_SafetySystem();
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, safetySystem->getSystemState());
    TEST_ASSERT_TRUE(safetySystem->isArmed());
    TEST_ASSERT_TRUE(safetySystem->canOperateMotors());
    tearDown_SafetySystem();
}

void test_emergency_stop(void) {
    setUp_SafetySystem();
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    safetySystem->triggerEmergencyStop();
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, safetySystem->getSystemState());
    TEST_ASSERT_TRUE(safetySystem->isEmergencyStop());
    TEST_ASSERT_FALSE(safetySystem->canOperateMotors());
    tearDown_SafetySystem();
}

void test_watchdog_timeout(void) {
    setUp_SafetySystem();
    safetySystem->setControllerConnected(true);
    safetySystem->armSystem();
    safetySystem->updateControllerActivity();
    
    // Simulate timeout (3 seconds > 2 second timeout)
    safetySystem->setCurrentTime(3000);
    safetySystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, safetySystem->getSystemState());
    tearDown_SafetySystem();
}

// ===== INTEGRATION TESTS =====
void setUp_Integration(void) {
    rcCarSystem = new RCCarSystem();
    rcCarSystem->initialize();
}

void tearDown_Integration(void) {
    delete rcCarSystem;
    rcCarSystem = nullptr;
}

void test_system_initialization(void) {
    setUp_Integration();
    TEST_ASSERT_TRUE(rcCarSystem->isSystemReady());
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, rcCarSystem->getSystemState());
    tearDown_Integration();
}

void test_controller_connection_workflow(void) {
    setUp_Integration();
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, rcCarSystem->getSystemState());
    
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
    tearDown_Integration();
}

void test_arm_disarm_workflow(void) {
    setUp_Integration();
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
    
    // Simulate A button press to arm
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, rcCarSystem->getSystemState());
    tearDown_Integration();
}

void test_emergency_stop_workflow(void) {
    setUp_Integration();
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    // Arm system first
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Trigger emergency stop
    controller.setButtonY(true);
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, rcCarSystem->getSystemState());
    tearDown_Integration();
}

// ===== MAIN TEST RUNNER =====
int main(void) {
    UNITY_BEGIN();
    
    // Motor Controller Tests (5 tests)
    RUN_TEST(test_motor_controller_initialization);
    RUN_TEST(test_set_left_motor_forward);
    RUN_TEST(test_set_both_motors);
    RUN_TEST(test_motor_enable_disable);
    RUN_TEST(test_stop_all_motors);
    
    // Controller Processor Tests (5 tests)
    RUN_TEST(test_controller_processor_initialization);
    RUN_TEST(test_no_input_returns_stopped_motors);
    RUN_TEST(test_deadzone_filtering);
    RUN_TEST(test_forward_movement);
    RUN_TEST(test_pivot_turn_right);
    
    // Safety System Tests (6 tests)
    RUN_TEST(test_safety_system_initialization);
    RUN_TEST(test_controller_connection);
    RUN_TEST(test_arm_system);
    RUN_TEST(test_emergency_stop);
    RUN_TEST(test_watchdog_timeout);
    
    // Integration Tests (4 tests)
    RUN_TEST(test_system_initialization);
    RUN_TEST(test_controller_connection_workflow);
    RUN_TEST(test_arm_disarm_workflow);
    RUN_TEST(test_emergency_stop_workflow);
    
    return UNITY_END();
}