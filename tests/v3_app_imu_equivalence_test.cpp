// SYNTHETIC seeded differential test against immutable installed 1.6 driver.
#include "imu_baseline_generated.h"
#include "../trailguide_v3/imu.h"
#include "gesture_baseline_generated.h"
#include "../trailguide_v3/gesture.h"
#include <array>
#include <cassert>
#include <cstring>
#include <vector>
#include <cstdio>
struct Bus {
 std::array<uint8_t,256> r{};std::vector<uint32_t> calls;
 unsigned address=0,reg=0,cursor=0,left=0,phase=0,fail=0;
 void beginTransmission(uint8_t a){address=a;phase=0;calls.push_back(0x10000u|a);}
 size_t write(uint8_t v){calls.push_back(0x20000u|v);if(phase++==0)reg=v;else r[reg]=v;return 1;}
 int endTransmission(bool stop=true){calls.push_back(0x30000u|stop);return address==0x6b && fail!=1?0:2;}
 size_t requestFrom(uint8_t,size_t n){calls.push_back(0x40000u|reg|(uint32_t(n)<<8));cursor=reg;left=fail==2?0:n;return left;}
 int available(){return left;}int read(){--left;return r[cursor++];}
};
static uint32_t seed=0x73594ab1;
static uint32_t word(){seed=1664525u*seed+1013904223u;return seed;}
int main(){
 Bus a,b;a.r[0]=b.r[0]=5;a.r[1]=b.r[1]=124;
 baseline::Imu<Bus> old(a);v3::Imu<Bus> now(b);old.begin(0xfffff000u);now.begin(0xfffff000u);
 baselineapp::Gesture oldGesture;v3app::Gesture nowGesture;
 uint32_t clock=0xfffff000u;
 for(unsigned i=0;i<12000;++i){
  if(i>=12){
   for(unsigned k=0;k<12;++k)a.r[0x35+k]=b.r[0x35+k]=uint8_t(word()>>16);
   a.r[0x2e]=b.r[0x2e]=(i%7==0)?0:3;a.fail=b.fail=(i%13==0)?1:((i%17==0)?2:0);
   if(i%19==0)for(unsigned k=0;k<12;++k)a.r[0x35+k]=b.r[0x35+k]=255;
   if(i%23==0){a.r[0x35]=b.r[0x35]=255;a.r[0x36]=b.r[0x36]=127;}
  }
  clock+=i<12?1:word()%40;
  old.tick(clock);now.tick(clock);
  assert(a.calls==b.calls && a.r==b.r);
  const auto &x=old.sample();const auto &y=now.sample();
  assert(x.valid==y.valid && x.at_ms==y.at_ms && std::memcmp(x.acc_g,y.acc_g,sizeof(x.acc_g))==0 && std::memcmp(x.gyr_dps,y.gyr_dps,sizeof(x.gyr_dps))==0);
  assert(unsigned(old.state())==unsigned(now.state()) && old.read_errors()==now.read_errors() && std::strcmp(old.status(),now.status())==0);
  if(i%211==0){oldGesture.reset();nowGesture.reset();}
  assert(oldGesture.consume(x,clock,i%31!=0)==nowGesture.consume(y,clock,i%31!=0));
  const auto gx=oldGesture.diagnostic(x,clock);const auto gy=nowGesture.diagnostic(y,clock);
  assert(gx.phase==gy.phase && gx.dwellSamples==gy.dwellSamples && gx.haveOrigin==gy.haveOrigin && gx.dwelling==gy.dwelling && gx.seen==gy.seen && gx.cooling==gy.cooling && gx.rest==gy.rest);
  assert(gx.dwellMs==gy.dwellMs && gx.last==gy.last && gx.cycleAge==gy.cycleAge && gx.reject==gy.reject);
  assert(std::memcmp(gx.origin,gy.origin,sizeof(gx.origin))==0 && std::memcmp(gx.held,gy.held,sizeof(gx.held))==0);
  assert(gx.alignment==gy.alignment && gx.magnitude==gy.magnitude && gx.rate==gy.rate);
  a.calls.clear();b.calls.clear();
 }
 puts("PASS SYNTHETIC 12000 seeded steps incl rollover, not-ready, errors, all-FF, clipping: immutable 1.6 vs 1.7 exact bus operations/registers/published float bits/validity/timestamps/state/errors/status and detector triggers/state/predicates");
}
