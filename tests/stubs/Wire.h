#pragma once
#include "Arduino.h"
struct WireStub {
 int address = 0, respondingAddress = 0;
 void begin(int, int) {}
 void beginTransmission(int a) { address = a; }
 int endTransmission() { return address == respondingAddress ? 0 : 2; }
};
inline WireStub Wire;
