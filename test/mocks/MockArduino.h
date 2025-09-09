#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

// Mock Arduino.h for unit testing
#ifdef UNIT_TEST

#include <stdint.h>
#include <cmath>

// Pin modes
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// Digital values
#define LOW 0
#define HIGH 1

// Mock Arduino functions
inline void pinMode(int pin, int mode) {}
inline void digitalWrite(int pin, int value) {}
inline int digitalRead(int pin) { return LOW; }
inline void delay(unsigned long ms) {}
inline unsigned long millis() { return 0; }

// PWM/LED control functions (ESP32 specific)
inline void ledcSetup(int channel, double freq, int resolution) {}
inline void ledcAttachPin(int pin, int channel) {}
inline void ledcWrite(int channel, int value) {}

// Tone function
inline void tone(int pin, unsigned int frequency, unsigned long duration = 0) {}

// Math functions
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline long constrain(long x, long a, long b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

// Serial class mock
class SerialClass {
public:
    void begin(unsigned long baud) {}
    void println(const char* str) {}
    void printf(const char* format, ...) {}
};

extern SerialClass Serial;

#endif // UNIT_TEST
#endif // MOCK_ARDUINO_H