#pragma once
#include <functional>
using ota_error_t = unsigned;
struct OTAStub {
 int starts = 0, stops = 0, handles = 0;
 std::function<void()> startCallback, endCallback;
 std::function<void(ota_error_t)> error;
 void setHostname(const char*) {}
 void setPassword(const char*) {}
 void onStart(std::function<void()> f) { startCallback = f; }
 void onEnd(std::function<void()> f) { endCallback = f; }
 void onError(std::function<void(ota_error_t)> f) { error = f; }
 void begin() { ++starts; }
 void end() { ++stops; }
 void handle() { ++handles; }
};
inline OTAStub ArduinoOTA;
