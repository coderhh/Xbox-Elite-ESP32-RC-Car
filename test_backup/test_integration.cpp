#include <unity.h>
#include "RCCarSystem.h"
#include "mocks/MockBluepad32.h"
#include "mocks/MockArduino.h"

RCCarSystem* rcCarSystem;

void setUp(void) {
    rcCarSystem = new RCCarSystem();
    rcCarSystem->initialize();
}

void tearDown(void) {
    delete rcCarSystem;
    rcCarSystem = nullptr;
}

void test_system_initialization(void) {
    TEST_ASSERT_TRUE(rcCarSystem->isSystemReady());
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, rcCarSystem->getSystemState());
    TEST_ASSERT_EQUAL(0, rcCarSystem->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, rcCarSystem->getRightMotorState().speed);
}

void test_controller_connection_workflow(void) {
    // Initially disconnected
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, rcCarSystem->getSystemState());
    
    // Simulate controller connection
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    // Should be connected but disarmed
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
}

void test_arm_disarm_workflow(void) {
    // Connect controller first
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    // Initially disarmed
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
    
    // Press A button to arm
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Should be armed now
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, rcCarSystem->getSystemState());
    
    // Press A button again to disarm
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Should be disarmed again
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
}

void test_movement_control_workflow(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_ARMED, rcCarSystem->getSystemState());
    
    // Test forward movement
    controller.setAxisY(300);        // Forward
    controller.setThrottle(1023);    // Full throttle
    rcCarSystem->update();
    
    // Both motors should be moving forward
    MotorController::MotorState leftMotor = rcCarSystem->getLeftMotorState();
    MotorController::MotorState rightMotor = rcCarSystem->getRightMotorState();
    
    TEST_ASSERT_GREATER_THAN(0, leftMotor.speed);
    TEST_ASSERT_GREATER_THAN(0, rightMotor.speed);
    TEST_ASSERT_TRUE(leftMotor.direction);   // Forward
    TEST_ASSERT_TRUE(rightMotor.direction);  // Forward
    TEST_ASSERT_TRUE(leftMotor.enabled);
    TEST_ASSERT_TRUE(rightMotor.enabled);
}

void test_turning_workflow(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Test right turn (pivot)
    controller.setAxisX(300);        // Right turn
    controller.setAxisY(0);          // No forward/backward
    controller.setThrottle(1023);    // Full throttle
    rcCarSystem->update();
    
    MotorController::MotorState leftMotor = rcCarSystem->getLeftMotorState();
    MotorController::MotorState rightMotor = rcCarSystem->getRightMotorState();
    
    // For right pivot: left motor forward, right motor reverse
    TEST_ASSERT_GREATER_THAN(0, leftMotor.speed);
    TEST_ASSERT_GREATER_THAN(0, rightMotor.speed);
    TEST_ASSERT_TRUE(leftMotor.direction);   // Forward
    TEST_ASSERT_FALSE(rightMotor.direction); // Reverse
}

void test_emergency_stop_workflow(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Set some movement
    controller.setAxisY(300);
    controller.setThrottle(1023);
    rcCarSystem->update();
    
    // Verify motors are running
    TEST_ASSERT_GREATER_THAN(0, rcCarSystem->getLeftMotorState().speed);
    
    // Trigger emergency stop
    controller.setButtonY(true);
    rcCarSystem->update();
    
    // Should be in emergency stop state
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, rcCarSystem->getSystemState());
    
    // Motors should be stopped
    TEST_ASSERT_EQUAL(0, rcCarSystem->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, rcCarSystem->getRightMotorState().speed);
}

void test_emergency_stop_recovery_workflow(void) {
    // Connect controller
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    
    // Trigger emergency stop
    controller.setButtonY(true);
    rcCarSystem->update();
    controller.setButtonY(false);
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::EMERGENCY_STOP, rcCarSystem->getSystemState());
    
    // Clear emergency stop by pressing A
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Should be disarmed but not in emergency stop
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
}

void test_disarmed_prevents_movement(void) {
    // Connect but don't arm
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    TEST_ASSERT_EQUAL(SafetySystem::CONNECTED_DISARMED, rcCarSystem->getSystemState());
    
    // Try to move
    MockController& controller = BP32.getMockController();
    controller.setAxisY(300);
    controller.setThrottle(1023);
    rcCarSystem->update();
    
    // Motors should remain stopped
    TEST_ASSERT_EQUAL(0, rcCarSystem->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, rcCarSystem->getRightMotorState().speed);
}

void test_controller_disconnect_stops_motors(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Set movement
    controller.setAxisY(300);
    controller.setThrottle(1023);
    rcCarSystem->update();
    
    // Verify motors are running
    TEST_ASSERT_GREATER_THAN(0, rcCarSystem->getLeftMotorState().speed);
    
    // Disconnect controller
    BP32.simulateControllerDisconnect();
    rcCarSystem->update();
    
    // Should be disconnected and motors stopped
    TEST_ASSERT_EQUAL(SafetySystem::DISCONNECTED, rcCarSystem->getSystemState());
    TEST_ASSERT_EQUAL(0, rcCarSystem->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, rcCarSystem->getRightMotorState().speed);
}

void test_deadzone_filtering_integration(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Set small movement within deadzone
    controller.setAxisY(5);          // Within deadzone
    controller.setAxisX(5);          // Within deadzone
    controller.setThrottle(1023);    // Full throttle
    rcCarSystem->update();
    
    // Motors should remain stopped due to deadzone
    TEST_ASSERT_EQUAL(0, rcCarSystem->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, rcCarSystem->getRightMotorState().speed);
}

void test_speed_modulation_integration(void) {
    // Connect and arm system
    BP32.simulateControllerConnect();
    rcCarSystem->update();
    
    MockController& controller = BP32.getMockController();
    controller.setButtonA(true);
    rcCarSystem->update();
    controller.setButtonA(false);
    rcCarSystem->update();
    
    // Test low throttle
    controller.setAxisY(300);
    controller.setThrottle(200);     // Low throttle
    rcCarSystem->update();
    
    int lowThrottleSpeed = rcCarSystem->getLeftMotorState().speed;
    
    // Test high throttle
    controller.setThrottle(1023);    // High throttle
    rcCarSystem->update();
    
    int highThrottleSpeed = rcCarSystem->getLeftMotorState().speed;
    
    // High throttle should result in higher speed
    TEST_ASSERT_GREATER_THAN(lowThrottleSpeed, highThrottleSpeed);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_system_initialization);
    RUN_TEST(test_controller_connection_workflow);
    RUN_TEST(test_arm_disarm_workflow);
    RUN_TEST(test_movement_control_workflow);
    RUN_TEST(test_turning_workflow);
    RUN_TEST(test_emergency_stop_workflow);
    RUN_TEST(test_emergency_stop_recovery_workflow);
    RUN_TEST(test_disarmed_prevents_movement);
    RUN_TEST(test_controller_disconnect_stops_motors);
    RUN_TEST(test_deadzone_filtering_integration);
    RUN_TEST(test_speed_modulation_integration);
    
    return UNITY_END();
}