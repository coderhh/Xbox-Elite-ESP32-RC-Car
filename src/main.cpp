#include "RCCarSystem.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#endif

RCCarSystem* rcCar;

void setup() {
    rcCar = new RCCarSystem();
    rcCar->initialize();
}

void loop() {
    if (rcCar) {
        rcCar->update();
    }
}