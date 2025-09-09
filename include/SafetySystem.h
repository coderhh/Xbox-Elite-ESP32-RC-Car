#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include "../test/mocks/MockArduino.h"
#endif

class SafetySystem {
public:
    enum SystemState {
        DISCONNECTED,
        CONNECTED_DISARMED,
        CONNECTED_ARMED,
        EMERGENCY_STOP
    };

    SafetySystem(unsigned long watchdogTimeout = 2000);
    
    void initialize();
    void update();
    void setControllerConnected(bool connected);
    void updateControllerActivity();
    void armSystem();
    void disarmSystem();
    void triggerEmergencyStop();
    void clearEmergencyStop();
    
    // State queries
    SystemState getSystemState() const { return currentState; }
    bool isArmed() const { return currentState == CONNECTED_ARMED; }
    bool isEmergencyStop() const { return currentState == EMERGENCY_STOP; }
    bool canOperateMotors() const { return isArmed() && !isEmergencyStop(); }
    
    // For testing
    void setCurrentTime(unsigned long time) { currentTime = time; }
    unsigned long getTimeSinceLastActivity() const;

private:
    SystemState currentState;
    bool controllerConnected;
    unsigned long lastControllerActivity;
    unsigned long watchdogTimeout;
    unsigned long currentTime;
    
    void updateState();
    bool isWatchdogTriggered() const;
    
#ifndef UNIT_TEST
    unsigned long getCurrentTime() const { return millis(); }
#else
    unsigned long getCurrentTime() const { return currentTime; }
#endif
};

#endif