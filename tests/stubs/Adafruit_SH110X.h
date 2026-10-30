#pragma once
#include "Wire.h"
constexpr int SH110X_WHITE = 1;
struct Adafruit_SH1106G {
 bool allocationOK = true;
 int frames = 0;
 int initializedAddress = 0;
 Adafruit_SH1106G(int, int, WireStub*, int) {}
 bool begin(int address, bool) { initializedAddress = address; return allocationOK; }
 void clearDisplay() {}
 void setCursor(int, int) {}
 void print(const char*) {}
 void drawLine(int, int, int, int, int) {}
 void display() { ++frames; }
 void setTextSize(int) {}
 void setTextColor(int) {}
 void setTextWrap(bool) {}
};
