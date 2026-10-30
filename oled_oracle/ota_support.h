#pragma once

#if !defined(TRAILGUIDE_DISABLE_WIFI) && !defined(TRAILGUIDE_WIFI_CONFIGURED) && __has_include("secrets.h")
#include "secrets.h"
#define TRAILGUIDE_WIFI_CONFIGURED 1
#endif

#ifdef TRAILGUIDE_WIFI_CONFIGURED
#include <WiFi.h>
#include <ArduinoOTA.h>

static_assert(sizeof(TRAILGUIDE_WIFI_SSID) > 1, "SSID must not be empty");
static_assert(sizeof(TRAILGUIDE_OTA_PASSWORD) > 1, "OTA password must not be empty");
static_assert(sizeof(TRAILGUIDE_WIFI_PASSWORD) > 1, "Wi-Fi password must not be empty");

constexpr bool samePassword(const char *a, const char *b) {
  return *a == *b && (*a == '\0' || samePassword(a + 1, b + 1));
}
static_assert(!samePassword(TRAILGUIDE_WIFI_PASSWORD, TRAILGUIDE_OTA_PASSWORD),
              "Use a distinct OTA password");

bool otaReady = false;
uint32_t wifiAttemptAt = 0;

void beginUpdates() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("trailguide-v1");
  WiFi.setAutoReconnect(true);
  WiFi.begin(TRAILGUIDE_WIFI_SSID, TRAILGUIDE_WIFI_PASSWORD);
  wifiAttemptAt = millis();
  ArduinoOTA.setHostname("trailguide-v1");
  ArduinoOTA.setPassword(TRAILGUIDE_OTA_PASSWORD);
  ArduinoOTA.onStart([]() { Serial.println("OTA: update in progress; keep power connected."); });
  ArduinoOTA.onEnd([]() { Serial.println("OTA: complete; rebooting."); });
  ArduinoOTA.onError([](ota_error_t error) { Serial.printf("OTA: failed (%u).\n", error); });
  Serial.println("Wi-Fi: connecting in background.");
}

void serviceUpdates() {
  if (WiFi.status() != WL_CONNECTED) {
    if (otaReady) {
      ArduinoOTA.end();
      otaReady = false;
      Serial.println("Wi-Fi: disconnected; advice remains available.");
    }
    if (uint32_t(millis() - wifiAttemptAt) >= 30000) {
      wifiAttemptAt = millis();
      WiFi.reconnect();
    }
    return;
  }
  if (!otaReady) {
    ArduinoOTA.begin();
    otaReady = true;
    Serial.printf("OTA: ready at %s (trailguide-v1.local), port 3232.\n",
                  WiFi.localIP().toString().c_str());
  }
  ArduinoOTA.handle();
}
#else
void beginUpdates() { Serial.println("Wi-Fi disabled: no local secrets.h."); }
void serviceUpdates() {}
#endif
