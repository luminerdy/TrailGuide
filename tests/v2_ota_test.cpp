#include <Arduino.h>
#include <cassert>
#include <iostream>
#define TRAILGUIDE_WIFI_CONFIGURED 1
#define TRAILGUIDE_WIFI_SSID "unit-test-network"
#define TRAILGUIDE_WIFI_PASSWORD "unit-test-wifi-password"
#define TRAILGUIDE_OTA_PASSWORD "unit-test-v2-update-password"
#include "../trailguide_v2/ota_support.h"
static int notifications = 0;
static void changed() { ++notifications; }
int main() {
  onUpdateState = changed;
  beginUpdates();
  assert(!otaReady && !updateInProgress && WiFi.begins == 1);
  serviceUpdates(); assert(ArduinoOTA.starts == 0);
  now = 30000; serviceUpdates(); assert(WiFi.reconnects == 1);
  WiFi.connection = WL_CONNECTED; serviceUpdates();
  assert(otaReady && ArduinoOTA.starts == 1 && ArduinoOTA.handles == 1);
  serviceUpdates(); assert(ArduinoOTA.starts == 1 && ArduinoOTA.handles == 2);
  ArduinoOTA.startCallback(); assert(updateInProgress && notifications == 1);
  ArduinoOTA.error(1); assert(!updateInProgress && notifications == 2);
  ArduinoOTA.startCallback(); ArduinoOTA.endCallback();
  assert(!updateInProgress && notifications == 4);
  WiFi.connection = 0; serviceUpdates();
  assert(!otaReady && ArduinoOTA.stops == 1);
  WiFi.connection = WL_CONNECTED; serviceUpdates();
  assert(otaReady && ArduinoOTA.starts == 2);
  WiFi.connection = 0; now = UINT32_MAX - 10; wifiAttemptAt = now;
  const int before = WiFi.reconnects;
  now = 29990; serviceUpdates(); assert(WiFi.reconnects == before + 1);
  std::cout << "PASS: V2 nonblocking retry, reconnect, OTA lifecycle/error state, rollover\n";
}
