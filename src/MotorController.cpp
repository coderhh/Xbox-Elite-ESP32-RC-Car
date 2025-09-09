#include "MotorController.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#endif

#define PWM_FREQUENCY 1000
#define PWM_RESOLUTION 8
#define MAX_SPEED 255

MotorController::MotorController(const MotorPins& leftMotor, const MotorPins& rightMotor)
    : leftMotorPins(leftMotor), rightMotorPins(rightMotor), motorsEnabled(false) {
    
    leftMotorState = {0, true, false};
    rightMotorState = {0, true, false};
}

void MotorController::initialize() {
#ifndef UNIT_TEST
    // Set all motor pins as outputs
    pinMode(leftMotorPins.enablePin, OUTPUT);
    pinMode(leftMotorPins.input1Pin, OUTPUT);
    pinMode(leftMotorPins.input2Pin, OUTPUT);
    pinMode(rightMotorPins.enablePin, OUTPUT);
    pinMode(rightMotorPins.input1Pin, OUTPUT);
    pinMode(rightMotorPins.input2Pin, OUTPUT);
    
    // Setup PWM channels
    setupPWM(leftMotorPins.enablePin, leftMotorPins.pwmChannel);
    setupPWM(rightMotorPins.enablePin, rightMotorPins.pwmChannel);
#endif
    
    // Initialize to stopped state
    stopAllMotors();
    motorsEnabled = true;
}

void MotorController::setupPWM(int pin, int channel) {
#ifndef UNIT_TEST
    // Compatible with both old and new ESP32 Arduino Core
    ledcSetup(channel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(pin, channel);
#endif
}

void MotorController::setLeftMotor(int speed, bool direction) {
    setMotor(leftMotorPins, leftMotorState, speed, direction);
}

void MotorController::setRightMotor(int speed, bool direction) {
    setMotor(rightMotorPins, rightMotorState, speed, direction);
}

void MotorController::setBothMotors(int leftSpeed, bool leftDir, int rightSpeed, bool rightDir) {
    setLeftMotor(leftSpeed, leftDir);
    setRightMotor(rightSpeed, rightDir);
}

void MotorController::setMotor(const MotorPins& pins, MotorState& state, int speed, bool direction) {
    if (!motorsEnabled) {
        return;
    }
    
    // Constrain speed
    speed = constrain(speed, 0, MAX_SPEED);
    
    // Update state
    state.speed = speed;
    state.direction = direction;
    state.enabled = (speed > 0);

#ifndef UNIT_TEST
    // Set PWM speed
    ledcWrite(pins.pwmChannel, speed);
    
    // Set direction
    if (direction) {
        digitalWrite(pins.input1Pin, HIGH);
        digitalWrite(pins.input2Pin, LOW);
    } else {
        digitalWrite(pins.input1Pin, LOW);
        digitalWrite(pins.input2Pin, HIGH);
    }
#endif
}

void MotorController::stopAllMotors() {
#ifndef UNIT_TEST
    ledcWrite(leftMotorPins.pwmChannel, 0);
    ledcWrite(rightMotorPins.pwmChannel, 0);
    
    digitalWrite(leftMotorPins.input1Pin, LOW);
    digitalWrite(leftMotorPins.input2Pin, LOW);
    digitalWrite(rightMotorPins.input1Pin, LOW);
    digitalWrite(rightMotorPins.input2Pin, LOW);
#endif
    
    leftMotorState = {0, true, false};
    rightMotorState = {0, true, false};
}

void MotorController::enableMotors(bool enable) {
    motorsEnabled = enable;
    if (!enable) {
        stopAllMotors();
    }
}