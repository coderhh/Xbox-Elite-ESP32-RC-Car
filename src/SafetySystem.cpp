#include "SafetySystem.h"

SafetySystem::SafetySystem(unsigned long watchdogTimeout) 
    : currentState(DISCONNECTED), controllerConnected(false), 
      lastControllerActivity(0), watchdogTimeout(watchdogTimeout), currentTime(0) {
}

void SafetySystem::initialize() {
    currentState = DISCONNECTED;
    controllerConnected = false;
    lastControllerActivity = getCurrentTime();
}

void SafetySystem::update() {
    updateState();
}

void SafetySystem::setControllerConnected(bool connected) {
    controllerConnected = connected;
    if (connected) {
        updateControllerActivity();
        if (currentState == DISCONNECTED) {
            currentState = CONNECTED_DISARMED;
        }
    } else {
        currentState = DISCONNECTED;
    }
}

void SafetySystem::updateControllerActivity() {
    lastControllerActivity = getCurrentTime();
}

void SafetySystem::armSystem() {
    if (controllerConnected && currentState != EMERGENCY_STOP) {
        currentState = CONNECTED_ARMED;
    }
}

void SafetySystem::disarmSystem() {
    if (controllerConnected && currentState != EMERGENCY_STOP) {
        currentState = CONNECTED_DISARMED;
    }
}

void SafetySystem::triggerEmergencyStop() {
    currentState = EMERGENCY_STOP;
}

void SafetySystem::clearEmergencyStop() {
    if (controllerConnected) {
        currentState = CONNECTED_DISARMED;
    } else {
        currentState = DISCONNECTED;
    }
}

void SafetySystem::updateState() {
    // Check for watchdog timeout
    if (controllerConnected && isWatchdogTriggered()) {
        currentState = DISCONNECTED;
        controllerConnected = false;
    }
}

bool SafetySystem::isWatchdogTriggered() const {
    return (getCurrentTime() - lastControllerActivity) > watchdogTimeout;
}

unsigned long SafetySystem::getTimeSinceLastActivity() const {
    return getCurrentTime() - lastControllerActivity;
}