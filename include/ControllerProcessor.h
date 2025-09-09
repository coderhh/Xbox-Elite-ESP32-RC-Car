#ifndef CONTROLLER_PROCESSOR_H
#define CONTROLLER_PROCESSOR_H

struct ControllerInput {
    int leftStickX;    // -512 to 512
    int leftStickY;    // -512 to 512
    int rightTrigger;  // 0 to 1023
    bool buttonA;
    bool buttonB;
    bool buttonX;
    bool buttonY;
};

struct MotorCommand {
    int leftSpeed;     // -255 to 255 (negative = reverse)
    int rightSpeed;    // -255 to 255 (negative = reverse)
    bool valid;        // false if motors should be stopped
};

class ControllerProcessor {
public:
    ControllerProcessor(int deadzone = 10, int minSpeed = 50, int maxSpeed = 255);
    
    MotorCommand processInput(const ControllerInput& input);
    
    // Configuration
    void setDeadzone(int deadzone) { this->deadzone = deadzone; }
    void setSpeedLimits(int minSpeed, int maxSpeed);
    
    // Getters for testing
    int getDeadzone() const { return deadzone; }
    int getMinSpeed() const { return minSpeed; }
    int getMaxSpeed() const { return maxSpeed; }

private:
    int deadzone;
    int minSpeed;
    int maxSpeed;
    
    int applyDeadzone(int input);
    int calculateBaseSpeed(int triggerValue);
    void calculateTankSteering(int forwardInput, int turnInput, int baseSpeed, 
                              int& leftOutput, int& rightOutput);
};

#endif