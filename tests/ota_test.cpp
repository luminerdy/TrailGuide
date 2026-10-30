#include <cassert>
#include <iostream>
#define TRAILGUIDE_WIFI_CONFIGURED 1
#define TRAILGUIDE_WIFI_SSID "test-network"
#define TRAILGUIDE_WIFI_PASSWORD "test-wifi-password"
#define TRAILGUIDE_OTA_PASSWORD "test-update-password"
#include "../oled_oracle/oled_oracle.ino"
int main() {
 setup();
 assert(!displayReady && !otaReady && WiFi.begins == 1);
 button = LOW;
 for (int i = 0; i < 40; ++i) loop();
 assert(nextCard == 1 && ArduinoOTA.starts == 0);
 now = 30000; loop(); assert(WiFi.reconnects == 1);
 WiFi.connection = WL_CONNECTED; loop();
 assert(otaReady && ArduinoOTA.starts == 1 && ArduinoOTA.handles == 1);
 loop(); assert(ArduinoOTA.starts == 1 && ArduinoOTA.handles == 2);
 ArduinoOTA.startCallback(); ArduinoOTA.error(1); ArduinoOTA.endCallback();
 assert(Serial.output.find("OTA: failed (1)") != std::string::npos);
 WiFi.connection = 0; loop();
 assert(!otaReady && ArduinoOTA.stops == 1);
 WiFi.connection = WL_CONNECTED; loop();
 assert(otaReady && ArduinoOTA.starts == 2);
 WiFi.connection = 0; now = UINT32_MAX - 10; wifiAttemptAt = now;
 loop(); int previous = WiFi.reconnects;
 now = 29990; loop(); assert(WiFi.reconnects == previous + 1);
 std::cout << "PASS: offline BOOT, background retry, OTA connect/disconnect/reconnect, callbacks, rollover\n";
}
