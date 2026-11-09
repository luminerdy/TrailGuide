// SYNTHETIC seeded differential against immutable installed 1.9; never human replay.
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
 int available(){return left;}
 int read(){--left;const auto at=cursor++;calls.push_back(0x50000u|(at<<8)|r[at]);return r[at];}
};
static uint32_t seed=0x73594ab1;
static uint32_t word(){seed=1664525u*seed+1013904223u;return seed;}
static void raw(Bus &b,unsigned axis,int value){const uint16_t bits=uint16_t(value);b.r[0x35+2*axis]=bits&255;b.r[0x36+2*axis]=bits>>8;}
// Historical CTRL3 whitelist is unchanged; add ONLY exact CTRL2 range write/readback.
static unsigned compareBus(const Bus &a,const Bus &b,bool physical){
 assert(a.calls.size()==b.calls.size());unsigned changes=0;
 for(size_t k=0;k<a.calls.size();++k){
  const auto x=a.calls[k],y=b.calls[k];if(x==y)continue;
  const bool ctrlWrite=x==0x20057 && y==0x20077 && k>0 && a.calls[k-1]==0x20004 && b.calls[k-1]==0x20004;
  const bool ctrlRead=x==0x50457 && y==0x50477;
  const unsigned at=(x>>8)&255;
  const bool encodedGyro=physical && (x&0xf0000)==0x50000 && (y&0xf0000)==0x50000 && at>=0x3b && at<=0x40 && (y&0xff00)==(x&0xff00);
 const bool accCtrlWrite=x==0x20017 && y==0x20037 && k>0 && a.calls[k-1]==0x20003 && b.calls[k-1]==0x20003;
 const bool accCtrlRead=x==0x50317 && y==0x50337;
 const bool encodedAcc=physical && (x&0xf0000)==0x50000 && (y&0xf0000)==0x50000 && at>=0x35 && at<=0x3a && (y&0xff00)==(x&0xff00);
 assert(ctrlWrite || ctrlRead || encodedGyro || accCtrlWrite || accCtrlRead || encodedAcc);++changes;
 }
 for(unsigned k=0;k<256;++k){
  if(k==3){assert((a.r[k]==0 && b.r[k]==0) || (a.r[k]==0x17 && b.r[k]==0x37));continue;}
  if(physical && k>=0x35 && k<=0x3a)continue; // independently encoded same physical acceleration
  if(k==4){assert((a.r[k]==0 && b.r[k]==0) || (a.r[k]==0x57 && b.r[k]==0x77));continue;}
  if(physical && k>=0x3b && k<=0x40)continue; // checked as signed physical values below
  assert(a.r[k]==b.r[k]);
 }
 return changes;
}
int main(){
 unsigned configDifferences=0;
 // Equal raw stream: preserve every bus call/state assertion, allow exact 4x gyro conversion only.
 {Bus a,b;a.r[0]=b.r[0]=5;a.r[1]=b.r[1]=124;
 baseline::Imu<Bus> old(a);v3::Imu<Bus> now(b);old.begin(0xfffff000u);now.begin(0xfffff000u);uint32_t clock=0xfffff000u;
 for(unsigned i=0;i<12000;++i){
  if(i>=12){
   for(unsigned k=0;k<12;++k)a.r[0x35+k]=b.r[0x35+k]=uint8_t(word()>>16);
   a.r[0x2e]=b.r[0x2e]=(i%7==0)?0:3;a.fail=b.fail=(i%13==0)?1:((i%17==0)?2:0);
   if(i%19==0)for(unsigned k=0;k<12;++k)a.r[0x35+k]=b.r[0x35+k]=255;
   if(i%23==0){a.r[0x35]=b.r[0x35]=255;a.r[0x36]=b.r[0x36]=127;}
  }
  clock+=i<12?1:word()%40;old.tick(clock);now.tick(clock);configDifferences+=compareBus(a,b,false);
  const auto &x=old.sample();const auto &y=now.sample();
  assert(x.valid==y.valid && x.at_ms==y.at_ms);
  for(unsigned axis=0;axis<3;++axis)assert(y.acc_g[axis]==4*x.acc_g[axis]);
  for(unsigned axis=0;axis<3;++axis)assert(y.gyr_dps[axis]==4*x.gyr_dps[axis]);
  // Same raw headroom (32704 counts) despite 4x different physical units.
  baselineapp::Gesture oldGuard;v3app::Gesture newGuard;
  assert(oldGuard.diagnostic(x,clock).reject==newGuard.diagnostic(y,clock).reject);
  assert(unsigned(old.state())==unsigned(now.state()) && old.read_errors()==now.read_errors() && std::strcmp(old.status(),now.status())==0);
  const auto &m=old.last_poll();const auto &n=now.last_poll();
  assert(m.sequence==n.sequence && m.at_ms==n.at_ms && m.errors==n.errors && m.status0==n.status0 && m.flags==n.flags && m.clip==n.clip && std::memcmp(m.raw,n.raw,sizeof(m.raw))==0);
  a.calls.clear();b.calls.clear();
 }}
 assert(configDifferences==4); // exactly one CTRL3 write and one readback, no blanket bus exclusions
 unsigned fires=0;
 // Independently encoded equal PHYSICAL motion; stationary bias is not multiplied.
 {Bus a,b;a.r[0]=b.r[0]=5;a.r[1]=b.r[1]=124;
 baseline::Imu<Bus> old(a);v3::Imu<Bus> now(b);old.begin(0);now.begin(0);
 baselineapp::Gesture oldGesture;v3app::Gesture nowGesture;uint32_t clock=0;
 for(unsigned i=0;i<12000;++i){
  if(i>=12){
   const unsigned pose=i%500;const bool turning=(pose>=80 && pose<95)||(pose>=125 && pose<140);const bool down=pose>=95 && pose<125;
   for(unsigned axis=0;axis<3;++axis){
    const int acc=axis==2?(turning?0:(down?-8192:8192)):(axis==0 && turning?8192:0);
    raw(a,axis,acc);raw(b,axis,acc/4);
    const int gyro=axis==0?(turning?5760:140):int(word()%5)*4-8;
    raw(a,axis+3,gyro);raw(b,axis+3,gyro/4);
   }
   a.r[0x2e]=b.r[0x2e]=3;a.fail=b.fail=0;
   // Fault blocks don't eliminate the clean completed cycles in other periods.
   if(i%1000>=500){
    if(i%37==0)a.r[0x2e]=b.r[0x2e]=0;
    if(i%61==0)a.fail=b.fail=1;
    if(i%67==0)a.fail=b.fail=2;
    if(i%71==0)for(unsigned k=0;k<12;++k)a.r[0x35+k]=b.r[0x35+k]=255;
    if(i%73==0){raw(a,0,-32768);raw(b,0,-32768);}
   }
  }
  clock+=i<12?1:20;old.tick(clock);now.tick(clock);compareBus(a,b,true);
  const auto &x=old.sample();const auto &y=now.sample();
  assert(x.valid==y.valid && x.at_ms==y.at_ms && std::memcmp(x.acc_g,y.acc_g,sizeof(x.acc_g))==0 && std::memcmp(x.gyr_dps,y.gyr_dps,sizeof(x.gyr_dps))==0);
  assert(unsigned(old.state())==unsigned(now.state()) && old.read_errors()==now.read_errors() && std::strcmp(old.status(),now.status())==0);
  if(i%1000==700){oldGesture.reset();nowGesture.reset();}
  const bool eligible=i%1000<500 || i%31!=0;
  const bool fire=oldGesture.consume(x,clock,eligible);assert(fire==nowGesture.consume(y,clock,eligible));fires+=fire;
  const auto gx=oldGesture.diagnostic(x,clock);const auto gy=nowGesture.diagnostic(y,clock);
  assert(gx.phase==gy.phase && gx.dwellSamples==gy.dwellSamples && gx.haveOrigin==gy.haveOrigin && gx.dwelling==gy.dwelling && gx.seen==gy.seen && gx.cooling==gy.cooling && gx.rest==gy.rest);
  assert(gx.dwellMs==gy.dwellMs && gx.last==gy.last && gx.cycleAge==gy.cycleAge && gx.reject==gy.reject && gx.resets==gy.resets && gx.resetReason==gy.resetReason);
  assert(std::memcmp(gx.origin,gy.origin,sizeof(gx.origin))==0 && std::memcmp(gx.held,gy.held,sizeof(gx.held))==0);
  assert(gx.alignment==gy.alignment && gx.magnitude==gy.magnitude && gx.rate==gy.rate);
  a.calls.clear();b.calls.clear();
 }}
 assert(fires>0);
 printf("PASS SYNTHETIC 12000 equal-raw steps (ONLY CTRL2+CTRL3 write/readback and 4x accel+gyro conversion) + 12000 independently encoded equal-physical steps (exact gesture/state/bus equivalence, %u fires); NOT HUMAN REPLAY\n",fires);
}
