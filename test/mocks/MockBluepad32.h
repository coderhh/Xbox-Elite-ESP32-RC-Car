#ifndef MOCK_BLUEPAD32_H
#define MOCK_BLUEPAD32_H

#ifdef UNIT_TEST

// Mock Bluepad32 for unit testing
class MockController {
public:
    MockController() : connected(false), x_axis(0), y_axis(0), throttle_val(0),
                       a_pressed(false), b_pressed(false), x_pressed(false), y_pressed(false) {}
    
    bool isConnected() const { return connected; }
    int axisX() const { return x_axis; }
    int axisY() const { return y_axis; }
    int throttle() const { return throttle_val; }
    bool a() const { return a_pressed; }
    bool b() const { return b_pressed; }
    bool x() const { return x_pressed; }
    bool y() const { return y_pressed; }
    int index() const { return 0; }
    
    // Test helper methods
    void setConnected(bool conn) { connected = conn; }
    void setAxisX(int val) { x_axis = val; }
    void setAxisY(int val) { y_axis = val; }
    void setThrottle(int val) { throttle_val = val; }
    void setButtonA(bool pressed) { a_pressed = pressed; }
    void setButtonB(bool pressed) { b_pressed = pressed; }
    void setButtonX(bool pressed) { x_pressed = pressed; }
    void setButtonY(bool pressed) { y_pressed = pressed; }

private:
    bool connected;
    int x_axis, y_axis, throttle_val;
    bool a_pressed, b_pressed, x_pressed, y_pressed;
};

typedef MockController* ControllerPtr;

class MockBP32 {
public:
    static void setup(void (*onConnected)(ControllerPtr), void (*onDisconnected)(ControllerPtr)) {
        onConnectedCallback = onConnected;
        onDisconnectedCallback = onDisconnected;
    }
    
    static void update() {}
    static void forgetBluetoothKeys() {}
    
    static ControllerPtr controller(int index) {
        if (index == 0) return &mockController;
        return nullptr;
    }
    
    // Test helper methods
    static void simulateControllerConnect() {
        mockController.setConnected(true);
        if (onConnectedCallback) {
            onConnectedCallback(&mockController);
        }
    }
    
    static void simulateControllerDisconnect() {
        mockController.setConnected(false);
        if (onDisconnectedCallback) {
            onDisconnectedCallback(&mockController);
        }
    }
    
    static MockController& getMockController() { return mockController; }

private:
    static MockController mockController;
    static void (*onConnectedCallback)(ControllerPtr);
    static void (*onDisconnectedCallback)(ControllerPtr);
};

#define BP32_MAX_GAMEPADS 4
extern MockBP32 BP32;

#endif // UNIT_TEST
#endif // MOCK_BLUEPAD32_H