#ifndef RC_CAR_SYSTEM_H
#define RC_CAR_SYSTEM_H

#include "MotorController.h"
#include "SafetySystem.h"
#include "ControllerProcessor.h"

#include "GamepadInterface.h"

class RCCarSystem {
public:
    RCCarSystem();
    ~RCCarSystem();
    
    void initialize();
    void update();
    
    // Status getters for testing
    bool isSystemReady() const;
    SafetySystem::SystemState getSystemState() const;
    MotorController::MotorState getLeftMotorState() const;
    MotorController::MotorState getRightMotorState() const;

    // For testing - allow injection of dependencies
    void setMotorController(MotorController* mc) { motorController = mc; ownMotorController = false; }
    void setSafetySystem(SafetySystem* ss) { safetySystem = ss; ownSafetySystem = false; }
    void setControllerProcessor(ControllerProcessor* cp) { controllerProcessor = cp; ownControllerProcessor = false; }

private:
    MotorController* motorController;
    SafetySystem* safetySystem;
    ControllerProcessor* controllerProcessor;
    
    // Ownership flags for cleanup
    bool ownMotorController;
    bool ownSafetySystem;
    bool ownControllerProcessor;
    
    // State tracking
    bool lastButtonAState;
    bool lastButtonBState;
    bool lastButtonXState;
    
    // Hardware pin configurations
    static const MotorController::MotorPins LEFT_MOTOR_PINS;
    static const MotorController::MotorPins RIGHT_MOTOR_PINS;
    
    // Callback functions for Bluepad32
    static void onControllerConnected(ControllerPtr ctl);
    static void onControllerDisconnected(ControllerPtr ctl);
    static RCCarSystem* instance; // For static callback access
    
    // Internal methods
    void processControllerInput(ControllerPtr ctl);
    void handleButtonPress(ControllerPtr ctl);
    void processMovementInput(ControllerPtr ctl);
    void updateFeedbackSystems();
    void playTone(int frequency, int duration);
    void updateStatusLED();
    
    // Hardware abstraction
    void setupHardware();
    void setupStatusLED();
    void setupBuzzer();
};

#endif