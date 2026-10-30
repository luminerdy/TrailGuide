#pragma once
#include "Wire.h"
constexpr int SSD1306_WHITE = 1, SSD1306_SWITCHCAPVCC = 2;
struct Adafruit_SSD1306 {
 bool allocationOK = true;
 int frames = 0;
 Adafruit_SSD1306(int, int, WireStub*, int) {}
 bool begin(int, int, bool, bool) { return allocationOK; }
 void clearDisplay() {}
 void setCursor(int, int) {}
 void print(const char*) {}
 void drawLine(int, int, int, int, int) {}
 void display() { ++frames; }
 void setTextSize(int) {}
 void setTextColor(int) {}
 void setTextWrap(bool) {}
};
