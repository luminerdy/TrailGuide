#include <Arduino.h>
#include <cassert>
#include <iostream>
#define TRAILGUIDE_DISABLE_WIFI 1
#include "../trailguide_v2/ota_support.h"
int main() {
  beginUpdates(); serviceUpdates();
  assert(!otaReady && !updateInProgress);
  assert(Serial.output.find("Wi-Fi disabled") != std::string::npos);
  std::cout << "PASS: V2 configuration-free build leaves updates disabled\n";
}
