// Actual pinned SDK write implementation; synthetic ringbuffer/RTOS/USB only.
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "../trailguide_v3/motion_diagnostics.h"
#include "../trailguide_v3/gesture.h"
struct Snapshot {
 bool pointer=false,scrolling=false,options=false,reveal=false,eligible=true,root=true,healthy=true;unsigned view=0;
 v3app::Gesture::Diagnostic gesture;
 uint32_t key()const{return unsigned(eligible)|(unsigned(gesture.phase)<<8);}
};
static unsigned delays=0,mutexTicks=0,writes=0,flushes=0;
static bool reading=true;
static size_t used=0;
constexpr size_t ringSize=128;
constexpr unsigned portTICK_PERIOD_MS=1,pdPASS=1,pdTRUE=1,USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY=1;
static void *tx_ring_buf=reinterpret_cast<void*>(1),*tx_lock=reinterpret_cast<void*>(1);
static unsigned xSemaphoreTake(void*,unsigned ticks){mutexTicks+=ticks;return pdPASS;}
static void xSemaphoreGive(void*){}
static size_t xRingbufferGetCurFreeSize(void*){return ringSize-used;}
static unsigned xRingbufferSend(void*,void *,size_t n,unsigned){assert(n<=ringSize-used);used+=n;return pdTRUE;}
static void usb_serial_jtag_ll_txfifo_flush(){if(reading)used=0;}
static void usb_serial_jtag_ll_ena_intr_mask(unsigned){}
static bool usb_serial_jtag_ll_txfifo_writable(){return false;}
static void delay(unsigned ms){assert(ms==1);if(++delays>16)throw std::runtime_error("iteration guard: zero budget underflowed before reaching zero");}
#define log_w(...) ((void)0)
struct Transport {
 uint32_t tx_timeout_ms=100;bool connected=true;unsigned timeoutCalls=0;
 std::string rx,last;size_t cursor=0;
 void begin(unsigned baud){assert(baud==115200);}
 void setTxTimeoutMs(uint32_t n){tx_timeout_ms=n;++timeoutCalls;}
 bool isCDC_Connected()const{return connected;}
 explicit operator bool()const{return connected;}
 void flushTXBuffer(const uint8_t *,size_t n){++flushes;used=std::min(n,ringSize);}
 int available(){return int(rx.size()-cursor);}int read(){return available()?rx[cursor++]:-1;}
 int availableForWrite(){return int(ringSize-used);}
 size_t write(const uint8_t *,size_t);
 int printf(const char *format,...){assert(tx_timeout_ms==1);char b[1024];va_list a;va_start(a,format);int n=vsnprintf(b,sizeof(b),format,a);va_end(a);assert(n>0 && size_t(n)<sizeof(b));last=b;++writes;write(reinterpret_cast<uint8_t*>(b),size_t(n));return n;}
} Serial;
#include "hwcdc_write_generated.h"
static v3app::MotionDiagnostics motionDiagnostics;
namespace v3app {constexpr const char *version="HOST-BUDGET";struct WindowReveal {static constexpr unsigned bytes=143360;};}
struct ImuDouble {struct Sample {bool valid=true;} s;const Sample &sample()const{return s;} const char *status()const{return "IMU: sample";}unsigned read_errors()const{return 0;}} imu;
struct UiDouble {bool fallback=false;bool busy()const{return false;}} ui;
struct lv_mem_monitor_t {unsigned free_size=0,free_biggest_size=0;};
static void lv_mem_monitor(lv_mem_monitor_t*){}
constexpr unsigned MALLOC_CAP_SPIRAM=1;
static unsigned heap_caps_get_free_size(unsigned){return 0;}
static bool ready=true;static uint32_t last_heartbeat=0;
static void startup(){
#include "budget_setup_generated.h"
}
static void heartbeat(uint32_t now){
#include "startup_heartbeat_generated.h"
}
static void fullUnread(){reading=false;used=ringSize;Serial.connected=true;delays=mutexTicks=0;}
int main(int argc,char **argv){
 if(argc>1 && std::strcmp(argv[1],"--zero")==0){
  fullUnread();Serial.setTxTimeoutMs(0);const uint8_t b=1;
  try{Serial.write(&b,1);}catch(const std::runtime_error &e){std::printf("EXPECTED RED: %s; delays=%u (uint32_t tries begins 0, decrement wraps UINT32_MAX)\n",e.what(),delays);return 1;}
  puts("ERROR zero did not reproduce");return 2;
 }
 // Driver-level finite budget independent of the application/spy.
 fullUnread();Serial.setTxTimeoutMs(1);const uint8_t b=1;
 assert(Serial.write(&b,1)==0 && delays==1 && mutexTicks==1 && !Serial.connected);
 puts("PASS verbatim HWCDC::write connected+full+no-reader: budget=1, one delay, one mutex tick, returns partial=0");
 reading=true;used=0;Serial.connected=true;Serial.tx_timeout_ms=100;Serial.timeoutCalls=0;
 startup();
 if(Serial.tx_timeout_ms!=1 || Serial.timeoutCalls!=1){puts("RED setup: Serial.begin must immediately set finite positive 1ms budget before startup printf");return 1;}
 assert(Serial.last.find("V3_START")==0);
 // The actual activation path must reassert a positive budget, not restore at end.
 Serial.tx_timeout_ms=100;Serial.rx="MOTION\n";motionDiagnostics.service(Serial,0);
 if(Serial.tx_timeout_ms!=1 || Serial.timeoutCalls!=2){puts("RED activation: MOTION must set finite positive 1ms budget");return 1;}
 assert(motionDiagnostics.active());
 // Stay connected but stop host reads during active telemetry and timeout.
 // Capacity checks leave a pending line, yet the caller keeps stepping.
 v3::ImuSample sample;sample.valid=true;sample.acc_g[2]=1;Snapshot snapshot;
 reading=false;used=0;unsigned activeProgress=0;
 for(uint32_t now=5;now<=45100;now+=5){
  sample.at_ms=now;motionDiagnostics.observe(Serial,now,sample,"IMU: sample",snapshot,snapshot,false);
  motionDiagnostics.service(Serial,now);++activeProgress;
 }
 assert(activeProgress==9020 && !motionDiagnostics.active() && motionDiagnostics.draining());
 assert(Serial.connected && motionDiagnostics.dropped()>0);
 // An actual reader drain (synthetic here), not flush/end/reset, finishes lines.
 reading=true;used=0;
 for(uint32_t now=45105;now<=45300;now+=5)motionDiagnostics.service(Serial,now);
 assert(!motionDiagnostics.active() && !motionDiagnostics.draining());
 assert(Serial.tx_timeout_ms==1); // end must not leave future regular writes at zero
 fullUnread();unsigned progress=0;
 for(uint32_t now=50000;now<=70000;now+=5){heartbeat(now);++progress;}
 assert(progress==4001 && last_heartbeat==70000 && delays==1 && mutexTicks==5);
 assert(Serial.last.find("V3_HEARTBEAT")==0 && Serial.last.size()>ringSize);
 assert(flushes>0); // driver disconnected itself, not a test workaround
 // Reopen without resetting the app: fresh connected/full block remains bounded.
 reading=true;used=0;Serial.connected=true;heartbeat(75000);assert(last_heartbeat==75000);
 fullUnread();heartbeat(80000);assert(last_heartbeat==80000 && delays==1);
 puts("PASS actual sketch setup+MOTION activation budget=1; connected/unread active+timeout: 9020 caller steps; reader drain/default OFF; oversized regular heartbeat+unread transport: 4001 app steps; reopen/close without reset returns; synthetic USB only");
}
