#include "MockBluepad32.h"

#ifdef UNIT_TEST

MockController MockBP32::mockController;
void (*MockBP32::onConnectedCallback)(ControllerPtr) = nullptr;
void (*MockBP32::onDisconnectedCallback)(ControllerPtr) = nullptr;

MockBP32 BP32;

#endif // UNIT_TEST