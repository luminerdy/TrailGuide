// Lightweight actual driver + unchanged sketch slice + real Shake.
// Ui is a detector adapter here, not LVGL/UI composition evidence.
#include "../trailguide_v3/imu.h"
#include "../trailguide_v3/shake.h"
#include "../trailguide_v3/gesture.h"
#include <new>
#include "../trailguide_v3/motion_diagnostics.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
struct Bus {
 std::array<uint8_t,256> r{};unsigned address=0,reg=0,cursor=0,left=0,phase=0;int shortReg=-1;
 void beginTransmission(uint8_t a){address=a;phase=0;}
 size_t write(uint8_t v){if(phase++==0)reg=v;else r[reg]=v;return 1;}
 int endTransmission(bool=true){return address==0x6b?0:2;}
 size_t requestFrom(uint8_t,size_t n){cursor=reg;left=int(reg)==shortReg?0:n;return left;}
 int available(){return int(left);}int read(){--left;return r[cursor++];}
} bus;
struct Transport {
 int available(){return 0;}int read(){return -1;}int availableForWrite(){return 0;}
 size_t write(const uint8_t*,size_t n){return n;}void setTxTimeoutMs(uint32_t){}
 explicit operator bool()const{return false;}
} Serial;
struct DetectorAdapter {
 v3app::Shake shake;unsigned events=0;bool healthy=true;
 void sample(const v3::ImuSample&s,uint32_t now,bool available,bool*hit=nullptr){healthy=available;bool fired=shake.consume(s,now,true,available);events+=fired;if(hit)*hit=fired;}
 struct G {unsigned phase=0;};
 // Diagnostics are OFF for this small actual-dispatch proof. Template still
 // compiles via real Gesture's diagnostic snapshot supplied by the recorder fixture.
 v3app::Gesture gesture;
 struct Snapshot {
  bool pointer=false,scrolling=false,options=false,reveal=false,eligible=true,root=true,healthy=true;
  unsigned view=0;uint32_t resetCounts[8]={};v3app::Gesture::Diagnostic gesture;
  uint32_t key()const{return 0;}
 };
 Snapshot diagnostic(const v3::ImuSample&s,uint32_t now){Snapshot d;d.gesture=gesture.diagnostic(s,now);return d;}
} ui;
static v3::Imu<Bus> imu(bus);
static v3app::MotionDiagnostics motionDiagnostics;
static bool ready=true;
static void dispatch(uint32_t now){
#include "startup_motion_generated.h"
}
static void raw(unsigned axis,int value){auto b=uint16_t(value);bus.r[0x35+axis*2]=uint8_t(b);bus.r[0x36+axis*2]=uint8_t(b>>8);}
int main(){
 unsigned cases=0;
 for(bool recording:{false,true})for(unsigned fault=0;fault<10;++fault){
  motionDiagnostics.~MotionDiagnostics();new (&motionDiagnostics) v3app::MotionDiagnostics;
  if(recording){motionDiagnostics.recorder.allocator(std::malloc,std::free);assert(motionDiagnostics.recorder.start(0,77));}
  bus=Bus{};ui=DetectorAdapter{};bus.r[0]=5;bus.r[1]=124;bus.r[0x2e]=3;imu.~Imu();new (&imu) v3::Imu<Bus>(bus);imu.begin(0);
  for(unsigned t=0;t<10;++t)imu.tick(t);
  uint32_t now=20;
  auto poll=[&](int x){now+=20;raw(0,x);raw(1,0);raw(2,2048);dispatch(now);};
  for(unsigned i=0;i<30;++i)poll(0);
  for(unsigned i=0;i<35;++i)poll(i%2?2458:-2458);
  const auto retained=imu.sample().at_ms;
  if(fault<2)bus.shortReg=fault==0?0x2e:0x35;
  else if(fault<4)for(unsigned i=0x35;i<=0x40;++i)bus.r[i]=fault==2?0:255;
  else raw(fault-4,32767);
  now+=20;dispatch(now);
  assert((imu.last_poll().flags&(8|32|64))!=0);
  assert(!imu.sample().valid && imu.sample().at_ms==retained);
  assert(!ui.healthy && "Known hard poll fault must reach actual sketch dispatch");
  dispatch(now+5);assert(!ui.healthy); // retained fault/duplicate loop also fails closed
  bus.shortReg=-1;for(unsigned a=0;a<6;++a)raw(a,0);
  for(unsigned i=0;i<100;++i)poll(i%2?2458:-2458);
  assert(ui.events==0 && "Fault must destroy partial burst until quiet rearm");
  for(unsigned i=0;i<30;++i)poll(0);
  for(unsigned i=0;i<60;++i)poll(i%2?2458:-2458);
  assert(ui.events==1);++cases;
  ui=DetectorAdapter{};
  for(unsigned i=0;i<30;++i)poll(0);
  for(unsigned i=0;i<70;++i){
   bus.r[0x2e]=(i==15 || i==35)?0:3;
   poll(i%2?2458:-2458);
   if(!bus.r[0x2e])assert(!imu.sample().valid && ui.healthy && (imu.last_poll().flags&16)!=0);
  }
  assert(ui.events==1 && "Bounded healthy NotReady must retain partial intent");
 }
 printf("PASS %u raw fault domains through actual sketch dispatch; retained timestamps/duplicates reset partial intent; fresh quiet + full burst draws once\n",cases);
}
