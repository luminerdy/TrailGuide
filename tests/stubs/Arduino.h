#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdarg>
#include <cstdio>
constexpr bool HIGH = true, LOW = false;
constexpr int INPUT_PULLUP = 2;
inline uint32_t now = 0;
inline bool button = HIGH;
inline void pinMode(int, int) {}
inline bool digitalRead(int) { return button; }
inline uint32_t millis() { return now; }
inline void delay(int ms) { now += ms; }
inline void randomSeed(unsigned seed) { std::srand(seed); }
inline long random(long limit) { return std::rand() % limit; }
struct SerialStub {
 std::string output;
 void begin(int) {}
 void println(const char* s) { output += s; output += '\n'; }
 void printf(const char* format, ...) {
  char buffer[128]; va_list args; va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args); va_end(args); output += buffer;
 }
};
inline SerialStub Serial;
