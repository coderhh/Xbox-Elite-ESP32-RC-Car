#include <unity.h>
#include "ControllerProcessor.h"

ControllerProcessor* processor;

void setUp(void) {
    processor = new ControllerProcessor(10, 50, 255); // deadzone=10, minSpeed=50, maxSpeed=255
}

void tearDown(void) {
    delete processor;
    processor = nullptr;
}

void test_controller_processor_initialization(void) {
    TEST_ASSERT_EQUAL(10, processor->getDeadzone());
    TEST_ASSERT_EQUAL(50, processor->getMinSpeed());
    TEST_ASSERT_EQUAL(255, processor->getMaxSpeed());
}

void test_no_input_returns_stopped_motors(void) {
    ControllerInput input = {0, 0, 0, false, false, false, false};
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_FALSE(command.valid);
    TEST_ASSERT_EQUAL(0, command.leftSpeed);
    TEST_ASSERT_EQUAL(0, command.rightSpeed);
}

void test_deadzone_filtering(void) {
    // Input within deadzone should be ignored
    ControllerInput input = {5, 5, 500, false, false, false, false}; // within deadzone of 10
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_FALSE(command.valid);
    TEST_ASSERT_EQUAL(0, command.leftSpeed);
    TEST_ASSERT_EQUAL(0, command.rightSpeed);
}

void test_forward_movement(void) {
    ControllerInput input = {0, 300, 1023, false, false, false, false}; // full forward, max trigger
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_GREATER_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(command.leftSpeed, command.rightSpeed); // should be equal for straight forward
}

void test_backward_movement(void) {
    ControllerInput input = {0, -300, 1023, false, false, false, false}; // full backward, max trigger
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_LESS_THAN(0, command.leftSpeed);
    TEST_ASSERT_LESS_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(command.leftSpeed, command.rightSpeed); // should be equal for straight backward
}

void test_right_turn_forward(void) {
    ControllerInput input = {300, 300, 1023, false, false, false, false}; // forward + right turn
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_GREATER_THAN(0, command.rightSpeed);
    TEST_ASSERT_GREATER_THAN(command.leftSpeed, command.rightSpeed); // left motor faster for right turn
}

void test_left_turn_forward(void) {
    ControllerInput input = {-300, 300, 1023, false, false, false, false}; // forward + left turn
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_GREATER_THAN(0, command.rightSpeed);
    TEST_ASSERT_GREATER_THAN(command.rightSpeed, command.leftSpeed); // right motor faster for left turn
}

void test_pivot_right(void) {
    ControllerInput input = {300, 0, 1023, false, false, false, false}; // pure right turn
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_GREATER_THAN(0, command.leftSpeed);
    TEST_ASSERT_LESS_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(abs(command.leftSpeed), abs(command.rightSpeed)); // equal and opposite for pivot
}

void test_pivot_left(void) {
    ControllerInput input = {-300, 0, 1023, false, false, false, false}; // pure left turn
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_LESS_THAN(0, command.leftSpeed);
    TEST_ASSERT_GREATER_THAN(0, command.rightSpeed);
    TEST_ASSERT_EQUAL(abs(command.leftSpeed), abs(command.rightSpeed)); // equal and opposite for pivot
}

void test_trigger_speed_modulation(void) {
    // Test minimum trigger
    ControllerInput inputMin = {0, 300, 0, false, false, false, false};
    MotorCommand commandMin = processor->processInput(inputMin);
    
    // Test maximum trigger
    ControllerInput inputMax = {0, 300, 1023, false, false, false, false};
    MotorCommand commandMax = processor->processInput(inputMax);
    
    TEST_ASSERT_TRUE(commandMin.valid);
    TEST_ASSERT_TRUE(commandMax.valid);
    TEST_ASSERT_GREATER_THAN(commandMax.leftSpeed, commandMin.leftSpeed);
    TEST_ASSERT_GREATER_THAN(commandMax.rightSpeed, commandMin.rightSpeed);
}

void test_speed_constraints(void) {
    // Test that output never exceeds maximum speed
    ControllerInput input = {512, 512, 1023, false, false, false, false}; // max input
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_LESS_OR_EQUAL(abs(command.leftSpeed), 255);
    TEST_ASSERT_LESS_OR_EQUAL(abs(command.rightSpeed), 255);
}

void test_set_deadzone(void) {
    processor->setDeadzone(20);
    TEST_ASSERT_EQUAL(20, processor->getDeadzone());
    
    // Input within new deadzone should be ignored
    ControllerInput input = {15, 15, 500, false, false, false, false};
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_FALSE(command.valid);
}

void test_set_speed_limits(void) {
    processor->setSpeedLimits(75, 200);
    TEST_ASSERT_EQUAL(75, processor->getMinSpeed());
    TEST_ASSERT_EQUAL(200, processor->getMaxSpeed());
    
    // Test that new limits are respected
    ControllerInput input = {0, 300, 1023, false, false, false, false}; // max trigger
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_LESS_OR_EQUAL(abs(command.leftSpeed), 200);
    TEST_ASSERT_LESS_OR_EQUAL(abs(command.rightSpeed), 200);
}

void test_backward_right_turn(void) {
    ControllerInput input = {300, -300, 1023, false, false, false, false}; // backward + right turn
    MotorCommand command = processor->processInput(input);
    
    TEST_ASSERT_TRUE(command.valid);
    TEST_ASSERT_LESS_THAN(0, command.leftSpeed);  // both negative (backward)
    TEST_ASSERT_LESS_THAN(0, command.rightSpeed);
    TEST_ASSERT_GREATER_THAN(abs(command.rightSpeed), abs(command.leftSpeed)); // right motor works harder
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_controller_processor_initialization);
    RUN_TEST(test_no_input_returns_stopped_motors);
    RUN_TEST(test_deadzone_filtering);
    RUN_TEST(test_forward_movement);
    RUN_TEST(test_backward_movement);
    RUN_TEST(test_right_turn_forward);
    RUN_TEST(test_left_turn_forward);
    RUN_TEST(test_pivot_right);
    RUN_TEST(test_pivot_left);
    RUN_TEST(test_trigger_speed_modulation);
    RUN_TEST(test_speed_constraints);
    RUN_TEST(test_set_deadzone);
    RUN_TEST(test_set_speed_limits);
    RUN_TEST(test_backward_right_turn);
    
    return UNITY_END();
}