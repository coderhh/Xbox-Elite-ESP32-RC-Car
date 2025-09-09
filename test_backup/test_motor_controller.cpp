#include <unity.h>
#include "MotorController.h"

MotorController* motorController;

void setUp(void) {
    MotorController::MotorPins leftMotor = {13, 5, 4, 0};   // ENA, IN1, IN2, PWM_CHANNEL
    MotorController::MotorPins rightMotor = {14, 27, 26, 1}; // ENB, IN3, IN4, PWM_CHANNEL
    
    motorController = new MotorController(leftMotor, rightMotor);
    motorController->initialize();
}

void tearDown(void) {
    delete motorController;
    motorController = nullptr;
}

void test_motor_controller_initialization(void) {
    TEST_ASSERT_TRUE(motorController->areMotorsEnabled());
    TEST_ASSERT_EQUAL(0, motorController->getLeftMotorState().speed);
    TEST_ASSERT_EQUAL(0, motorController->getRightMotorState().speed);
    TEST_ASSERT_FALSE(motorController->getLeftMotorState().enabled);
    TEST_ASSERT_FALSE(motorController->getRightMotorState().enabled);
}

void test_set_left_motor_forward(void) {
    motorController->setLeftMotor(150, true);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    TEST_ASSERT_EQUAL(150, leftState.speed);
    TEST_ASSERT_TRUE(leftState.direction);
    TEST_ASSERT_TRUE(leftState.enabled);
}

void test_set_left_motor_reverse(void) {
    motorController->setLeftMotor(100, false);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    TEST_ASSERT_EQUAL(100, leftState.speed);
    TEST_ASSERT_FALSE(leftState.direction);
    TEST_ASSERT_TRUE(leftState.enabled);
}

void test_set_right_motor_forward(void) {
    motorController->setRightMotor(200, true);
    
    MotorController::MotorState rightState = motorController->getRightMotorState();
    TEST_ASSERT_EQUAL(200, rightState.speed);
    TEST_ASSERT_TRUE(rightState.direction);
    TEST_ASSERT_TRUE(rightState.enabled);
}

void test_set_both_motors(void) {
    motorController->setBothMotors(120, true, 80, false);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    MotorController::MotorState rightState = motorController->getRightMotorState();
    
    TEST_ASSERT_EQUAL(120, leftState.speed);
    TEST_ASSERT_TRUE(leftState.direction);
    TEST_ASSERT_EQUAL(80, rightState.speed);
    TEST_ASSERT_FALSE(rightState.direction);
}

void test_speed_constraints(void) {
    // Test maximum speed constraint
    motorController->setLeftMotor(300, true);
    TEST_ASSERT_EQUAL(255, motorController->getLeftMotorState().speed);
    
    // Test minimum speed constraint
    motorController->setRightMotor(-50, true);
    TEST_ASSERT_EQUAL(0, motorController->getRightMotorState().speed);
}

void test_stop_all_motors(void) {
    // First set some motor speeds
    motorController->setBothMotors(150, true, 200, false);
    
    // Then stop all motors
    motorController->stopAllMotors();
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    MotorController::MotorState rightState = motorController->getRightMotorState();
    
    TEST_ASSERT_EQUAL(0, leftState.speed);
    TEST_ASSERT_EQUAL(0, rightState.speed);
    TEST_ASSERT_FALSE(leftState.enabled);
    TEST_ASSERT_FALSE(rightState.enabled);
}

void test_enable_disable_motors(void) {
    // Disable motors
    motorController->enableMotors(false);
    TEST_ASSERT_FALSE(motorController->areMotorsEnabled());
    
    // Try to set motor speed when disabled
    motorController->setLeftMotor(150, true);
    TEST_ASSERT_EQUAL(0, motorController->getLeftMotorState().speed);
    
    // Re-enable motors
    motorController->enableMotors(true);
    TEST_ASSERT_TRUE(motorController->areMotorsEnabled());
    
    // Now motor should respond
    motorController->setLeftMotor(150, true);
    TEST_ASSERT_EQUAL(150, motorController->getLeftMotorState().speed);
}

void test_zero_speed_disables_motor(void) {
    motorController->setLeftMotor(0, true);
    
    MotorController::MotorState leftState = motorController->getLeftMotorState();
    TEST_ASSERT_EQUAL(0, leftState.speed);
    TEST_ASSERT_FALSE(leftState.enabled);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_motor_controller_initialization);
    RUN_TEST(test_set_left_motor_forward);
    RUN_TEST(test_set_left_motor_reverse);
    RUN_TEST(test_set_right_motor_forward);
    RUN_TEST(test_set_both_motors);
    RUN_TEST(test_speed_constraints);
    RUN_TEST(test_stop_all_motors);
    RUN_TEST(test_enable_disable_motors);
    RUN_TEST(test_zero_speed_disables_motor);
    
    return UNITY_END();
}