#include "RCCarSystem.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include "../test/mocks/MockArduino.h"
#endif

// Hardware pin definitions
#define STATUS_LED 2
#define BUZZER_PIN 23
#define EXTERNAL_LED 22

// Static member definitions
RCCarSystem* RCCarSystem::instance = nullptr;

const MotorController::MotorPins RCCarSystem::LEFT_MOTOR_PINS = {13, 5, 4, 0};   // ENA, IN1, IN2, PWM_CHANNEL
const MotorController::MotorPins RCCarSystem::RIGHT_MOTOR_PINS = {14, 27, 26, 1}; // ENB, IN3, IN4, PWM_CHANNEL

RCCarSystem::RCCarSystem() 
    : motorController(nullptr), safetySystem(nullptr), controllerProcessor(nullptr),
      ownMotorController(true), ownSafetySystem(true), ownControllerProcessor(true),
      lastButtonAState(false), lastButtonBState(false), lastButtonXState(false) {
    
    instance = this;
    
    // Create default components
    motorController = new MotorController(LEFT_MOTOR_PINS, RIGHT_MOTOR_PINS);
    safetySystem = new SafetySystem(2000); // 2 second watchdog timeout
    controllerProcessor = new ControllerProcessor(10, 50, 255); // deadzone=10, minSpeed=50, maxSpeed=255
}

RCCarSystem::~RCCarSystem() {
    if (ownMotorController && motorController) {
        delete motorController;
    }
    if (ownSafetySystem && safetySystem) {
        delete safetySystem;
    }
    if (ownControllerProcessor && controllerProcessor) {
        delete controllerProcessor;
    }
    
    if (instance == this) {
        instance = nullptr;
    }
}

void RCCarSystem::initialize() {
    Serial.begin(115200);
    Serial.println("Xbox Elite Series 2 + ESP32 RC Car Starting...");
    
    // Initialize components
    if (motorController) {
        motorController->initialize();
    }
    if (safetySystem) {
        safetySystem->initialize();
    }
    
    // Setup hardware
    setupHardware();
    
    // Initialize Bluepad32
    BP32.setup(&onControllerConnected, &onControllerDisconnected);
    BP32.forgetBluetoothKeys();
    
    // Startup sequence
    for (int i = 0; i < 3; i++) {
#ifndef UNIT_TEST
        digitalWrite(STATUS_LED, HIGH);
        digitalWrite(EXTERNAL_LED, HIGH);
        delay(200);
        digitalWrite(STATUS_LED, LOW);
        digitalWrite(EXTERNAL_LED, LOW);
        delay(200);
#endif
    }
    
    playTone(800, 100);
#ifndef UNIT_TEST
    delay(100);
#endif
    playTone(1000, 100);
    
    Serial.println("System ready. Waiting for Xbox controller...");
}

void RCCarSystem::update() {
    // Update Bluepad32
    BP32.update();
    
    // Update safety system
    if (safetySystem) {
        safetySystem->update();
    }
    
    // Process controllers
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr controller = BP32.controller(i);
        if (controller && controller->isConnected()) {
            processControllerInput(controller);
            if (safetySystem) {
                safetySystem->updateControllerActivity();
            }
        }
    }
    
    // Update feedback systems
    updateFeedbackSystems();
    
    // Ensure motors are stopped if system cannot operate
    if (safetySystem && motorController && !safetySystem->canOperateMotors()) {
        motorController->stopAllMotors();
    }
    
#ifndef UNIT_TEST
    delay(10);
#endif
}

void RCCarSystem::processControllerInput(ControllerPtr ctl) {
    handleButtonPress(ctl);
    processMovementInput(ctl);
}

void RCCarSystem::handleButtonPress(ControllerPtr ctl) {
    if (!safetySystem) return;
    
    // Emergency stop (Y button)
    if (ctl->y()) {
        safetySystem->triggerEmergencyStop();
        if (motorController) {
            motorController->stopAllMotors();
        }
        
        // Emergency stop feedback
        for (int i = 0; i < 5; i++) {
#ifndef UNIT_TEST
            digitalWrite(EXTERNAL_LED, HIGH);
#endif
            playTone(2000, 100);
#ifndef UNIT_TEST
            digitalWrite(EXTERNAL_LED, LOW);
            delay(100);
#endif
        }
        Serial.println("EMERGENCY STOP ACTIVATED!");
        return;
    }
    
    // System arm/disarm (A button)
    if (ctl->a() && !lastButtonAState) {
        if (safetySystem->isEmergencyStop()) {
            safetySystem->clearEmergencyStop();
            playTone(600, 150);
            Serial.println("Emergency stop CLEARED");
        } else if (safetySystem->isArmed()) {
            safetySystem->disarmSystem();
            playTone(600, 150);
            Serial.println("System DISARMED");
        } else {
            safetySystem->armSystem();
            playTone(1200, 150);
            Serial.println("System ARMED");
        }
    }
    lastButtonAState = ctl->a();
    
    // Horn (B button)
    if (ctl->b() && !lastButtonBState) {
        playTone(800, 100);
    }
    lastButtonBState = ctl->b();
    
    // Toggle lights (X button)
    if (ctl->x() && !lastButtonXState) {
#ifndef UNIT_TEST
        digitalWrite(EXTERNAL_LED, !digitalRead(EXTERNAL_LED));
#endif
    }
    lastButtonXState = ctl->x();
}

void RCCarSystem::processMovementInput(ControllerPtr ctl) {
    if (!safetySystem || !motorController || !controllerProcessor) return;
    
    // Only process movement if system can operate motors
    if (!safetySystem->canOperateMotors()) {
        motorController->stopAllMotors();
        return;
    }
    
    // Convert controller input to our standard format
    ControllerInput input;
    input.leftStickX = ctl->axisX();
    input.leftStickY = ctl->axisY();
    input.rightTrigger = ctl->throttle();
    input.buttonA = ctl->a();
    input.buttonB = ctl->b();
    input.buttonX = ctl->x();
    input.buttonY = ctl->y();
    
    // Process input to get motor commands
    MotorCommand command = controllerProcessor->processInput(input);
    
    if (command.valid) {
        // Apply motor commands
        bool leftDirection = command.leftSpeed >= 0;
        bool rightDirection = command.rightSpeed >= 0;
        
        motorController->setBothMotors(
            abs(command.leftSpeed), leftDirection,
            abs(command.rightSpeed), rightDirection
        );
    } else {
        motorController->stopAllMotors();
    }
}

void RCCarSystem::updateFeedbackSystems() {
    updateStatusLED();
}

void RCCarSystem::updateStatusLED() {
    if (!safetySystem) return;
    
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
#ifndef UNIT_TEST
    unsigned long currentTime = millis();
#else
    static unsigned long mockTime = 0;
    mockTime += 10;
    unsigned long currentTime = mockTime;
#endif
    
    SafetySystem::SystemState state = safetySystem->getSystemState();
    
    switch (state) {
        case SafetySystem::EMERGENCY_STOP:
            // Fast blink for emergency stop
            if (currentTime - lastBlink > 100) {
                ledState = !ledState;
#ifndef UNIT_TEST
                digitalWrite(STATUS_LED, ledState);
#endif
                lastBlink = currentTime;
            }
            break;
            
        case SafetySystem::DISCONNECTED:
            // Slow blink when disconnected
            if (currentTime - lastBlink > 1000) {
                ledState = !ledState;
#ifndef UNIT_TEST
                digitalWrite(STATUS_LED, ledState);
#endif
                lastBlink = currentTime;
            }
            break;
            
        case SafetySystem::CONNECTED_DISARMED:
            // Medium blink when connected but disarmed
            if (currentTime - lastBlink > 500) {
                ledState = !ledState;
#ifndef UNIT_TEST
                digitalWrite(STATUS_LED, ledState);
#endif
                lastBlink = currentTime;
            }
            break;
            
        case SafetySystem::CONNECTED_ARMED:
            // Solid on when armed and ready
#ifndef UNIT_TEST
            digitalWrite(STATUS_LED, HIGH);
#endif
            break;
    }
}

void RCCarSystem::setupHardware() {
#ifndef UNIT_TEST
    pinMode(STATUS_LED, OUTPUT);
    pinMode(EXTERNAL_LED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
#endif
}

void RCCarSystem::playTone(int frequency, int duration) {
#ifndef UNIT_TEST
    tone(BUZZER_PIN, frequency, duration);
#endif
}

// Static callback functions
void RCCarSystem::onControllerConnected(ControllerPtr ctl) {
    if (instance && instance->safetySystem) {
        instance->safetySystem->setControllerConnected(true);
    }
    
    Serial.printf("Controller connected, index=%d\n", ctl->index());
    
#ifndef UNIT_TEST
    digitalWrite(STATUS_LED, HIGH);
#endif
    if (instance) {
        instance->playTone(1000, 200);
    }
}

void RCCarSystem::onControllerDisconnected(ControllerPtr ctl) {
    if (instance && instance->safetySystem) {
        instance->safetySystem->setControllerConnected(false);
    }
    
    if (instance && instance->motorController) {
        instance->motorController->stopAllMotors();
    }
    
    Serial.printf("Controller disconnected from index=%d\n", ctl->index());
    
#ifndef UNIT_TEST
    digitalWrite(STATUS_LED, LOW);
#endif
    if (instance) {
        instance->playTone(500, 500);
    }
}

// Status getters for testing
bool RCCarSystem::isSystemReady() const {
    return motorController && safetySystem && controllerProcessor;
}

SafetySystem::SystemState RCCarSystem::getSystemState() const {
    return safetySystem ? safetySystem->getSystemState() : SafetySystem::DISCONNECTED;
}

MotorController::MotorState RCCarSystem::getLeftMotorState() const {
    return motorController ? motorController->getLeftMotorState() : MotorController::MotorState{0, true, false};
}

MotorController::MotorState RCCarSystem::getRightMotorState() const {
    return motorController ? motorController->getRightMotorState() : MotorController::MotorState{0, true, false};
}