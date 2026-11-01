#pragma once
#if !defined(TRAILGUIDE_DISABLE_WIFI) && !defined(TRAILGUIDE_WIFI_CONFIGURED) && __has_include("secrets.h")
#include "secrets.h"
#define TRAILGUIDE_WIFI_CONFIGURED 1
#endif

bool otaReady = false;
bool updateInProgress = false;
void (*onUpdateState)() = nullptr;

#ifdef TRAILGUIDE_WIFI_CONFIGURED
#include <WiFi.h>
#include <ArduinoOTA.h>
static_assert(sizeof(TRAILGUIDE_WIFI_SSID) > 1, "SSID must not be empty");
static_assert(sizeof(TRAILGUIDE_WIFI_PASSWORD) > 1, "Wi-Fi password must not be empty");
static_assert(sizeof(TRAILGUIDE_OTA_PASSWORD) >= 17, "Use an OTA password of at least sixteen characters");
constexpr bool sameV2Password(const char *a, const char *b) {
  return *a == *b && (*a == '\0' || sameV2Password(a + 1, b + 1));
}
static_assert(!sameV2Password(TRAILGUIDE_WIFI_PASSWORD, TRAILGUIDE_OTA_PASSWORD), "Use a distinct OTA password");
uint32_t wifiAttemptAt = 0;
inline void notifyUpdateState(bool active) {
  updateInProgress = active;
  if (onUpdateState) onUpdateState();
}
void beginUpdates() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("trailguide-v2");
  WiFi.setAutoReconnect(true);
  WiFi.begin(TRAILGUIDE_WIFI_SSID, TRAILGUIDE_WIFI_PASSWORD);
  wifiAttemptAt = millis();
  ArduinoOTA.setHostname("trailguide-v2");
  ArduinoOTA.setPassword(TRAILGUIDE_OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA: update in progress; keep power connected.");
    notifyUpdateState(true);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA: complete; rebooting.");
    notifyUpdateState(false);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA: failed (%u); existing firmware remains available.\n", error);
    notifyUpdateState(false);
  });
  Serial.println("Wi-Fi: connecting in background; advice remains available.");
}
void serviceUpdates() {
  if (WiFi.status() != WL_CONNECTED) {
    if (otaReady) {
      ArduinoOTA.end(); otaReady = false;
      Serial.println("Wi-Fi: disconnected; advice remains available.");
    }
    if (uint32_t(millis() - wifiAttemptAt) >= 30000) {
      wifiAttemptAt = millis(); WiFi.reconnect();
    }
    return;
  }
  if (!otaReady) {
    ArduinoOTA.begin(); otaReady = true;
    Serial.printf("OTA: ready at %s (trailguide-v2.local), port 3232.\n", WiFi.localIP().toString().c_str());
  }
  ArduinoOTA.handle();
}
#else
void beginUpdates() { Serial.println("Wi-Fi disabled: no local V2 secrets.h."); }
void serviceUpdates() {}
#endif
