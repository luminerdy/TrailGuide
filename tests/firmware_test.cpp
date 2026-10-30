#include <cassert>
#include <iostream>
#include <set>
#include <sstream>
#include <type_traits>
#include <Adafruit_SH110X.h>
#define TRAILGUIDE_DISABLE_WIFI 1
#include "../oled_oracle/oled_oracle.ino"
static_assert(std::is_same<decltype(display), Adafruit_SH1106G>::value,
              "Actual Hosyond OLED requires SH1106 page addressing, not SSD1306");

void runFor(unsigned ms) { for (unsigned i = 0; i < ms; ++i) loop(); }
void reset() {
 displayReady = false; display.frames = 0; display.allocationOK = true;
 Wire.respondingAddress = 0; Serial.output.clear();
 now = 0; button = HIGH; rawButton = stableButton = HIGH; changedAt = 0;
 nextCard = CARD_COUNT; lastCard = -1;
}
int main() {
 reset(); setup();
 assert(!displayReady);
 assert(Serial.output.find("Serial-only mode") != std::string::npos);
 button = LOW; runFor(10); button = HIGH; runFor(10);
 assert(lastCard == -1); // bounce must not draw
 button = LOW; runFor(36);
 assert(nextCard == 1);
 runFor(2000); assert(nextCard == 1); // held button draws once
 button = HIGH; runFor(36); button = LOW; runFor(36);
 assert(nextCard == 2); assert(display.frames == 0);
 reset(); setup();
 for (int cycle = 0; cycle < 1000; ++cycle) {
  std::set<int> seen;
  for (size_t i = 0; i < CARD_COUNT; ++i) {
   int previous = lastCard; drawCard();
   assert(lastCard != previous); assert(seen.insert(lastCard).second);
  }
  assert(seen.size() == CARD_COUNT);
 }
 for (const auto& card : cards) {
  assert(std::string(card.title).size() <= 21);
  std::istringstream body(card.body); std::string line; int lines = 0;
  while (std::getline(body, line)) {
   assert(line.size() <= 21); ++lines;
   for (unsigned char c : line) assert(c >= 32 && c <= 126);
  }
  assert(lines <= 4);
 }
 for (int address : {0x3C, 0x3D}) {
  reset(); Wire.respondingAddress = address; setup();
  assert(displayReady); assert(display.frames == 1);
  drawCard(); assert(display.frames == 2);
 }
 reset(); Wire.respondingAddress = 0x3C; display.allocationOK = false;
 setup(); assert(!displayReady); drawCard(); assert(display.frames == 0);
 // A press spanning the 32-bit millis rollover still debounces.
 reset(); now = UINT32_MAX - 10; button = LOW; runFor(36);
 assert(nextCard == 1);
 std::cout << "PASS: serial fallback, OLED setup paths, button debounce/hold/rollover, "
              "1,000 deck cycles, and card layout limits\n";
}
