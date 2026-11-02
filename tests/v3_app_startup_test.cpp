// Exercise the actual sketch heartbeat with real LVGL before/after init.
// Only Arduino/IMU/heap-capability transport surfaces are host test doubles.
#include <lvgl.h>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <string>
struct SerialDouble {
  std::string last;
  int printf(const char *format, ...) {
    char out[1024]; va_list args; va_start(args,format);
    int n=vsnprintf(out,sizeof(out),format,args); va_end(args); last=out; return n;
  }
} Serial;
struct ImuDouble {
  struct Sample { bool valid=false; } value;
  const char *status() const { return "IMU: off"; }
  const Sample &sample() const { return value; }
  unsigned read_errors() const { return 0; }
} imu;
struct UiDouble { bool fallback=false; bool busy() const { return false; } } ui;
namespace v3app {
constexpr const char *version="HOST-HEARTBEAT-TEST";
struct WindowReveal { static constexpr unsigned bytes=143360; };
}
constexpr unsigned MALLOC_CAP_SPIRAM=1;
unsigned heap_caps_get_free_size(unsigned) { return 0; }
static bool ready=false;
static uint32_t last_heartbeat=0;
static void heartbeat(uint32_t now) {
#include "startup_heartbeat_generated.h"
}
int main() {
  // Every pre-LVGL setup failure leaves ready=false. Real TLSF is not initialized.
  heartbeat(5000);
  assert(Serial.last.find("ui_ready=0")!=std::string::npos);
  assert(Serial.last.find("lvgl_heap_available=0")!=std::string::npos);
  lv_init(); ready=true;
  heartbeat(10000);
  assert(Serial.last.find("ui_ready=1")!=std::string::npos);
  assert(Serial.last.find("lvgl_heap_available=1")!=std::string::npos);
  puts("PASS actual heartbeat: pre-LVGL failure path avoids real uninitialized TLSF; initialized path reports available heap (host hardware doubles only)");
}
