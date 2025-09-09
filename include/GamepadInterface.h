#ifndef GAMEPAD_INTERFACE_H
#define GAMEPAD_INTERFACE_H

// Unified gamepad interface that works on M1 MacBook and other platforms

#ifdef UNIT_TEST
// Use mock for testing
#include "../test/mocks/MockBluepad32.h"
#elif defined(USE_ESP32_BLE_GAMEPAD)
// Use ESP32-BLE-Gamepad library (M1 compatible)
#include <BleGamepad.h>

class GamepadController {
private:
    BleGamepad* bleGamepad;
    bool connected;
    
public:
    GamepadController() : bleGamepad(nullptr), connected(false) {}
    
    void initialize() {
        // This library makes the ESP32 act as a gamepad, not receive gamepad input
        // For this demo, we'll simulate input or use serial commands
        connected = false;
    }
    
    void update() {
        // Check for serial commands or simulate input
    }
    
    bool isConnected() const { return connected; }
    int axisX() const { return 0; }        // Simulate no input
    int axisY() const { return 0; }
    int throttle() const { return 0; }
    bool a() const { return false; }
    bool b() const { return false; }
    bool x() const { return false; }
    bool y() const { return false; }
    int index() const { return 0; }
};

typedef GamepadController* ControllerPtr;

class GamepadManager {
public:
    static void setup(void (*onConnected)(ControllerPtr), void (*onDisconnected)(ControllerPtr)) {
        // Setup callbacks (simplified for demo)
    }
    static void update() {}
    static void forgetBluetoothKeys() {}
    static ControllerPtr controller(int index) { 
        static GamepadController dummy;
        return &dummy; 
    }
};

#define BP32_MAX_GAMEPADS 4
extern GamepadManager BP32;

#else
// Fallback stub implementation
class SimpleController {
public:
    bool isConnected() const { return false; }
    int axisX() const { return 0; }
    int axisY() const { return 0; }
    int throttle() const { return 0; }
    bool a() const { return false; }
    bool b() const { return false; }
    bool x() const { return false; }
    bool y() const { return false; }
    int index() const { return 0; }
};

typedef SimpleController* ControllerPtr;

class SimpleGamepadManager {
public:
    static void setup(void (*onConnected)(ControllerPtr), void (*onDisconnected)(ControllerPtr)) {}
    static void update() {}
    static void forgetBluetoothKeys() {}
    static ControllerPtr controller(int index) { return nullptr; }
};

#define BP32_MAX_GAMEPADS 4
extern SimpleGamepadManager BP32;

#endif

#endif // GAMEPAD_INTERFACE_H