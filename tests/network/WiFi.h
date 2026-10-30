#pragma once
#include "Arduino.h"
constexpr int WIFI_STA = 1, WL_CONNECTED = 3;
struct IPStub { std::string toString() { return "192.0.2.10"; } };
struct WiFiStub {
 int connection = 0, reconnects = 0, begins = 0;
 void persistent(bool) {}
 void mode(int) {}
 void setHostname(const char*) {}
 void setAutoReconnect(bool) {}
 void begin(const char*, const char*) { ++begins; }
 void reconnect() { ++reconnects; }
 int status() { return connection; }
 IPStub localIP() { return {}; }
};
inline WiFiStub WiFi;
