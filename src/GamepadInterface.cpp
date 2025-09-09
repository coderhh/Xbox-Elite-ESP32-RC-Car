#include "GamepadInterface.h"

#ifdef USE_ESP32_BLE_GAMEPAD
GamepadManager BP32;
#elif !defined(UNIT_TEST)
SimpleGamepadManager BP32;
#endif