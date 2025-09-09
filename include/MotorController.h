#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include "../test/mocks/MockArduino.h"
#endif

class MotorController {
public:
    struct MotorPins {
        int enablePin;
        int input1Pin;
        int input2Pin;
        int pwmChannel;
    };

    struct MotorState {
        int speed;           // 0-255
        bool direction;      // true = forward, false = reverse
        bool enabled;
    };

    MotorController(const MotorPins& leftMotor, const MotorPins& rightMotor);
    
    void initialize();
    void setLeftMotor(int speed, bool direction);
    void setRightMotor(int speed, bool direction);
    void setBothMotors(int leftSpeed, bool leftDir, int rightSpeed, bool rightDir);
    void stopAllMotors();
    void enableMotors(bool enable);
    
    // Getters for testing
    MotorState getLeftMotorState() const { return leftMotorState; }
    MotorState getRightMotorState() const { return rightMotorState; }
    bool areMotorsEnabled() const { return motorsEnabled; }

private:
    MotorPins leftMotorPins;
    MotorPins rightMotorPins;
    MotorState leftMotorState;
    MotorState rightMotorState;
    bool motorsEnabled;
    
    void setMotor(const MotorPins& pins, MotorState& state, int speed, bool direction);
    void setupPWM(int pin, int channel);
};

#endif