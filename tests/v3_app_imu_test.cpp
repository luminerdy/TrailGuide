// Host-only fake Wire; synthetic register values, never target samples.
#include "../trailguide_v3/imu.h"
#include <array>
#include <cassert>
#include <cstdio>
struct Bus {
 std::array<uint8_t,256> r{};bool present=true,shortRead=false,shortRaw=false;unsigned address=0,reg=0,cursor=0,left=0,writes=0,reads=0,phase=0;
 void beginTransmission(uint8_t a){address=a;phase=0;}
 size_t write(uint8_t v){if(phase++==0)reg=v;else {r[reg]=v;++writes;}return 1;}
 int endTransmission(bool=true){return present&&address==0x6b?0:2;}
 size_t requestFrom(uint8_t,size_t n){++reads;cursor=reg;left=(shortRead || (shortRaw && reg==0x35))?0:n;return left;}
 int available(){return left;}
 int read(){--left;return r[cursor++];}
};
int main(){
 Bus bus;bus.r[0]=5;bus.r[1]=0x7c;v3::Imu<Bus> imu(bus);imu.begin(0);for(unsigned n=0;n<10;n++)imu.tick(n);assert(imu.state()==v3::ImuState::Running && bus.writes==7);assert(!imu.sample().valid);
 bus.r[0x2e]=3;bus.r[0x3a]=0x20; // native Z 1g, SYNTHETIC
 unsigned before=bus.reads;imu.tick(19);assert(bus.reads==before);imu.tick(20);assert(imu.sample().valid && imu.sample().at_ms==20 && imu.sample().acc_g[2]==1);
 before=bus.reads;imu.tick(39);assert(bus.reads==before);imu.tick(40);assert(imu.sample().at_ms==40);
 bus.shortRead=true;imu.tick(60);assert(!imu.sample().valid && imu.read_errors()==1);bus.shortRead=false;imu.tick(80);assert(imu.sample().valid);
 bus.r[0x2e]=0;imu.tick(100);assert(!imu.sample().valid);bus.r[0x2e]=3;bus.r[0x39]=255;bus.r[0x3a]=127;imu.tick(120);assert(!imu.sample().valid);
 Bus missing;missing.present=false;v3::Imu<Bus> off(missing);off.begin(0);for(unsigned i=0;i<100;i++)off.tick(i);assert(off.state()==v3::ImuState::Missing && !missing.writes);
 Bus wrong;wrong.r[0]=1;v3::Imu<Bus> bad(wrong);bad.begin(0);for(unsigned i=0;i<100;i++)bad.tick(i);assert(bad.state()==v3::ImuState::Identity && !wrong.writes);
 bus.r[0x39]=0;bus.r[0x3a]=32;imu.begin(0xfffffff0u);for(unsigned i=0;i<10;i++)imu.tick(0xfffffff0u+i);imu.tick(3);assert(!imu.sample().valid);imu.tick(4);assert(imu.sample().valid);
 // Metadata must retain all six raw axes, including BOTH clip domains.
#ifdef V3_LAST_POLL_METADATA
 bus.r[0x35]=255;bus.r[0x36]=127;bus.r[0x3f]=0;bus.r[0x40]=128;
 imu.tick(24);const auto &m=imu.last_poll();
 assert(!imu.sample().valid && m.clip==33 && m.raw[0]==32767 && m.raw[5]==-32768 && m.status0==3 && m.flags==70);
 auto sequence=m.sequence;imu.tick(25);assert(imu.last_poll().sequence==sequence);
 bus.r[0x2e]=0;imu.tick(44);assert(imu.last_poll().flags==18 && imu.last_poll().clip==0);
 bus.shortRead=true;imu.tick(64);assert(imu.last_poll().flags==8 && imu.last_poll().errors==1);bus.shortRead=false;
 bus.r[0x2e]=3;for(unsigned i=0;i<12;++i)bus.r[0x35+i]=255;imu.tick(84);
 assert(imu.last_poll().flags==38 && imu.last_poll().raw[5]==-1 && !imu.sample().valid);
 bus.shortRaw=true;imu.tick(104);assert(imu.last_poll().flags==10 && imu.last_poll().status0==3 && imu.last_poll().errors==2 && !imu.sample().valid);bus.shortRaw=false;
 for(unsigned axis=0;axis<6;++axis){
  for(unsigned i=0;i<12;++i)bus.r[0x35+i]=0;
  bus.r[0x3a]=32;bus.r[0x35+2*axis]=0;bus.r[0x36+2*axis]=128;
  imu.tick(124+20*axis);assert(imu.last_poll().clip==(1u<<axis) && imu.last_poll().raw[axis]==-32768 && !imu.sample().valid);
 }
#else
 assert(!"last-poll diagnostics missing");
#endif
 puts("PASS owned IMU fake bus: NEW 20ms cadence, exact config/WHO gating, missing/wrong identity, fresh/status/short/clipped, rollover; NOT HARDWARE");
}
