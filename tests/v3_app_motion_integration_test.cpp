// Actual sketch dispatch + actual IMU/UI/Gesture/LVGL. All bus/input synthetic.
#include "../trailguide_v3/ui.h"
#include "../trailguide_v3/motion_diagnostics.h"
#include <array>
#include <cassert>
#include <string>
#include <cstdio>
#include <algorithm>
struct Bus {
 std::array<uint8_t,256> r{};bool shortRead=false;unsigned address=0,reg=0,cursor=0,left=0,phase=0,writes=0;
 void beginTransmission(uint8_t a){address=a;phase=0;}
 size_t write(uint8_t v){if(phase++==0)reg=v;else{r[reg]=v;++writes;}return 1;}
 int endTransmission(bool=true){return address==0x6b?0:2;}
 size_t requestFrom(uint8_t,size_t n){cursor=reg;left=shortRead?0:n;return left;}
 int available(){return left;}int read(){--left;return r[cursor++];}
} bus;
struct Transport {
 std::string rx,tx;size_t pos=0;int capacity=256;bool connected=true;
 explicit operator bool()const{return connected;}
 int available(){return int(rx.size()-pos);}int read(){return pos<rx.size()?rx[pos++]:-1;}
 int availableForWrite(){return capacity;}
 size_t write(const uint8_t *p,size_t n){assert(n<=256 && n<=size_t(capacity));tx.append(reinterpret_cast<const char*>(p),n);return n;}
 void setTxTimeoutMs(uint32_t n){assert(n==1);}
} Serial;
static uint32_t seed=1;static unsigned rngCalls=0;
static uint32_t rng(void*){++rngCalls;seed=1664525u*seed+1013904223u;return seed;}
static v3app::Ui ui(rng,nullptr,malloc,free);
static v3::Imu<Bus> imu(bus);
static v3app::MotionDiagnostics motionDiagnostics;
static bool ready=false;
static void dispatch(uint32_t now){
#include "startup_motion_generated.h"
}
static lv_color_t buf[360*10];
static void flush(lv_disp_drv_t *d,const lv_area_t *,lv_color_t *){lv_disp_flush_ready(d);}
static void step(uint32_t t){lv_tick_inc(5);lv_timer_handler();dispatch(t);}
int main(){
 // Before LVGL initialization: parser is safe even when UI startup failed.
 Serial.rx="MOTION\n";dispatch(0);assert(motionDiagnostics.active());
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buf,nullptr,360*10);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);
 ui.render();ready=true;bus.r[0]=5;bus.r[1]=0x7c;bus.r[0x2e]=3;bus.r[0x3a]=32;imu.begin(0);
 for(uint32_t t=5;t<=800;t+=5)step(t);
 assert(ui.diagnostic(imu.sample(),800).gesture.phase==1 && rngCalls==0);
 // Driver STATUS-not-ready on otherwise healthy configured bus, real reset.
 const auto lastFresh=imu.sample().at_ms;
 bus.r[0x2e]=0;step(805);step(810);step(815);step(820);
 auto g=ui.diagnostic(imu.sample(),820);assert(!imu.sample().valid && g.gesture.phase==4 && g.eligible && !g.healthy && bus.writes==7);
 bus.r[0x2e]=3;for(uint32_t t=825;t<=1700;t+=5)step(t);
 assert(ui.diagnostic(imu.sample(),1700).gesture.phase==1 && rngCalls==0);
 assert(Serial.tx.find("status=\"IMU: waiting\"")!=std::string::npos);
 assert(Serial.tx.find("phase=1>4")!=std::string::npos);
 assert(Serial.tx.find("last_bad="+std::to_string(lastFresh+20)+":1>4")!=std::string::npos);
 // Pointer and inertial scroll gate motion; Options is globally eligible.
 lv_event_send(ui.root,LV_EVENT_PRESSED,nullptr);assert(ui.diagnostic(imu.sample(),1700).pointer && !ui.eligible());
 lv_event_send(ui.root,LV_EVENT_RELEASED,nullptr);assert(!ui.diagnostic(imu.sample(),1700).pointer);
 lv_event_send(ui.root,LV_EVENT_SCROLL_BEGIN,nullptr);assert(ui.diagnostic(imu.sample(),1700).scrolling && !ui.eligible());
 lv_event_send(ui.root,LV_EVENT_SCROLL_END,nullptr);assert(!ui.diagnostic(imu.sample(),1700).scrolling);
 ui.perform(v3app::Ui::Action::Options);assert(ui.diagnostic(imu.sample(),1700).options && ui.eligible());ui.perform(v3app::Ui::Action::Back);
 // Successful flip still draws once through the instrumented actual loop.
 for(uint32_t t=1705;t<=2600;t+=5)step(t);
 // SYNTHETIC 600dps on both legs via actual raw conversion/dispatch/UI.
 // These invented peaks are NOT reconstructed from the clipped human trace.
 bus.r[0x3b]=0x80;bus.r[0x3c]=0x25; // +9600 raw = +600dps
 bus.r[0x3a]=224;for(uint32_t t=2605;t<=2700;t+=5)step(t);
 assert(imu.sample().gyr_dps[0]==600.f && imu.last_poll().raw[3]==9600);
 assert(!(ui.diagnostic(imu.sample(),2700).gesture.reject&16));
 bus.r[0x3b]=bus.r[0x3c]=0;for(uint32_t t=2705;t<=3100;t+=5)step(t);
 bus.r[0x3b]=0x80;bus.r[0x3c]=0xda; // -9600 raw = -600dps
 bus.r[0x3a]=32;for(uint32_t t=3105;t<=3200;t+=5)step(t);
 assert(imu.sample().gyr_dps[0]==-600.f && imu.last_poll().raw[3]==-9600);
 assert(!(ui.diagnostic(imu.sample(),3200).gesture.reject&16));
 bus.r[0x3b]=bus.r[0x3c]=0;for(uint32_t t=3205;t<=3700;t+=5)step(t);
 assert(ui.model.view==v3app::View::Card && ui.busy() && rngCalls>0);
 assert(Serial.tx.find("trigger=1")!=std::string::npos);
 for(uint32_t t=3705;t<=45100;t+=5)step(t);
 assert(!motionDiagnostics.active() && !motionDiagnostics.draining());
 assert(Serial.tx.find("end=timeout")!=std::string::npos && Serial.tx.find("fires=1")!=std::string::npos);
 FILE *f=std::fopen("build/v3-0.1.21/motion-integration.synthetic.log","wb");assert(f);std::fwrite(Serial.tx.data(),1,Serial.tx.size(),f);std::fclose(f);
 // No off-state telemetry, no extra consume/reset/RNG from helpers.
 const auto n=Serial.tx.size();const auto calls=rngCalls;for(uint32_t t=45105;t<=46000;t+=5)step(t);assert(n==Serial.tx.size() && calls==rngCalls);
 // Recorder is a second session on the same SINGLE parser, never two readers.
#ifdef V3_FLIGHT_RECORDER
 motionDiagnostics.recorder.allocator(malloc,free);
 const auto beforeRecordIndex=ui.model.index;
 Serial.rx+="RECORD\n";step(46005);assert(motionDiagnostics.recorder.active());
 Serial.connected=false;
 const auto basePoll=imu.last_poll().sequence;
 for(uint32_t t=46010;t<166005;t+=5){
  if(t==47000)lv_event_send(ui.root,LV_EVENT_PRESSED,nullptr);
  if(t==47005)lv_event_send(ui.root,LV_EVENT_RELEASED,nullptr);
  if(t==48000)bus.r[0x2e]=0;
  if(t==48020)bus.r[0x2e]=3;
  if(t==48500)bus.shortRead=true;
  if(t==48520)bus.shortRead=false;
  if(t==49000){bus.r[0x3f]=255;bus.r[0x40]=127;}
  if(t==49020){bus.r[0x3f]=0;bus.r[0x40]=0;}
  if(t==49200){bus.r[0x3f]=254;bus.r[0x40]=127;}
  if(t==49220){bus.r[0x3f]=0;bus.r[0x40]=0;}
  if(t==50000)bus.r[0x3a]=224;
  if(t==50500)bus.r[0x3a]=32;
  step(t);
 }
 step(166005);auto &ram=motionDiagnostics.recorder.ram;
 assert(ram.state==2 && ram.reason==1 && ram.count==imu.last_poll().sequence-basePoll && ram.count>=5998);
 bool wait=false,clip=false,contact=false,readError=false,reveal=false,axisReject=false;unsigned triggers=0;
 for(unsigned i=0;i<ram.count;++i){auto &r=ram.records[i];wait|=r.flags&16;clip|=r.clip==32;contact|=(r.resets&20)==20;readError|=(r.flags&8) && r.reset_reason==1 && (r.resets&256);triggers+=bool((r.flags&128) && r.reset_reason==9);axisReject|=r.reset_reason==6 && r.clip==0 && (r.flags&1);reveal|=r.gates_after&8;}
 assert(wait && clip && contact && readError && axisReject && triggers==1 && reveal && !motionDiagnostics.draining());
 assert(ram.crc32==v3app::FlightRecorder::crc(ram.records,ram.count*40));
 assert(ui.model.index!=beforeRecordIndex);
 const auto afterRecordRng=rngCalls;
 Serial.connected=true;Serial.rx+="STATUS\n";for(unsigned t=166010;t<166500;t+=5)step(t);
 assert(Serial.tx.find("TGFR1 STATUS")!=std::string::npos && !motionDiagnostics.draining());
 Serial.capacity=16;Serial.rx+="DUMP 1 0 32 4242\n";
 for(uint32_t t=166500;t<168500;t+=5)step(t);
 assert(Serial.tx.find("TGFR1 END session=1 token=4242")!=std::string::npos && !motionDiagnostics.draining());
 assert(rngCalls==afterRecordRng && ui.model.view==v3app::View::Card && ui.eligible());
#else
 assert(!"actual-loop flight recorder dispatch missing");
#endif
 lv_obj_del(ui.root); // UI globals destroyed after LVGL; no leaked root timers.
 puts("PASS actual sketch motion dispatch: pre-LVGL safety, real STATUS-not-ready invalid/reset/recovery, UI gates, flip trigger, timeout/default OFF; synthetic bus/transport only");
}
