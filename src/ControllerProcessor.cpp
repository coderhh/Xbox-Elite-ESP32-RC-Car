#include "ControllerProcessor.h"
#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include "../test/mocks/MockArduino.h"
#endif

ControllerProcessor::ControllerProcessor(int deadzone, int minSpeed, int maxSpeed)
    : deadzone(deadzone), minSpeed(minSpeed), maxSpeed(maxSpeed) {
}

void ControllerProcessor::setSpeedLimits(int minSpeed, int maxSpeed) {
    this->minSpeed = constrain(minSpeed, 0, 255);
    this->maxSpeed = constrain(maxSpeed, minSpeed, 255);
}

MotorCommand ControllerProcessor::processInput(const ControllerInput& input) {
    MotorCommand command = {0, 0, false};
    
    // Apply deadzone to stick inputs
    int stickY = applyDeadzone(input.leftStickY);
    int stickX = applyDeadzone(input.leftStickX);
    
    // If no meaningful input, return stopped motors
    if (stickY == 0 && stickX == 0) {
        return command;
    }
    
    // Calculate base speed from trigger
    int baseSpeed = calculateBaseSpeed(input.rightTrigger);
    
    // Calculate tank steering
    calculateTankSteering(stickY, stickX, baseSpeed, command.leftSpeed, command.rightSpeed);
    
    command.valid = true;
    return command;
}

int ControllerProcessor::applyDeadzone(int input) {
    if (abs(input) < deadzone) {
        return 0;
    }
    return input;
}

int ControllerProcessor::calculateBaseSpeed(int triggerValue) {
    return map(triggerValue, 0, 1023, minSpeed, maxSpeed);
}

void ControllerProcessor::calculateTankSteering(int forwardInput, int turnInput, int baseSpeed,
                                               int& leftOutput, int& rightOutput) {
    leftOutput = 0;
    rightOutput = 0;
    
    if (forwardInput != 0 || turnInput != 0) {
        // Convert stick inputs to proportional speeds (0.0 to 1.0)
        float forwardRatio = (float)abs(forwardInput) / 512.0f;
        float turnRatio = (float)abs(turnInput) / 512.0f;
        
        // Calculate base forward speed for both motors
        int forwardSpeed = (int)(forwardRatio * baseSpeed);
        
        if (forwardInput != 0) {  // Forward or backward with optional turning
            if (turnInput > 0) {  // Turn right
                // For right turn: left motor faster, right motor slower
                leftOutput = forwardSpeed;
                rightOutput = (int)(forwardSpeed * (1.0f - turnRatio * 0.5f));
            } else if (turnInput < 0) {  // Turn left  
                // For left turn: right motor faster, left motor slower
                rightOutput = forwardSpeed;
                leftOutput = (int)(forwardSpeed * (1.0f - turnRatio * 0.5f));
            } else {  // Straight forward/backward
                leftOutput = forwardSpeed;
                rightOutput = forwardSpeed;
            }
            
            // Apply direction
            if (forwardInput < 0) {
                leftOutput = -leftOutput;
                rightOutput = -rightOutput;
            }
            
        } else {  // Pure turning (pivot)
            int pivotSpeed = (int)(turnRatio * baseSpeed);
            if (turnInput > 0) {  // Turn right
                leftOutput = pivotSpeed;
                rightOutput = -pivotSpeed;
            } else {  // Turn left
                leftOutput = -pivotSpeed;
                rightOutput = pivotSpeed;
            }
        }
    }
    
    // Constrain outputs to valid range
    leftOutput = constrain(leftOutput, -maxSpeed, maxSpeed);
    rightOutput = constrain(rightOutput, -maxSpeed, maxSpeed);
}