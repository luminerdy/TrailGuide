// Host-only fake Wire bus. No synthetic values enter the device firmware.
#include "../trailguide_v3_bringup/imu.h"
#include <array>
#include <vector>
#include <cassert>
#include <cstdio>
#include <cstring>
struct FakeWire {
  std::array<std::array<uint8_t, 256>, 2> regs{};
  std::array<bool, 2> present{false, false};
  std::vector<std::array<int,3>> writes;
  std::vector<int> reads;
  int addr=0, reg=0, cursor=0, count=0, short_count=-1, fail_reg=-1;
  bool writing=false, nack_write=false, corrupt=false;
  int transactions=0;
  void beginTransmission(uint8_t a) { ++transactions; addr=a-0x6a; writing=false; }
  size_t write(uint8_t v) { if(!writing) {reg=v;writing=true;} else { writes.push_back({addr+0x6a,reg,v}); if(!corrupt) regs[addr][reg]=v; } return 1; }
  int endTransmission(bool = true) { return !present[addr] || (nack_write && !writes.empty()) ? 2 : 0; }
  size_t requestFrom(uint8_t, size_t n) { reads.push_back(reg); cursor=reg; count= !present[addr] || reg==fail_reg ? 0 : (short_count>=0 && size_t(short_count)<n ? short_count : int(n)); return count; }
  int available() { return count; }
  int read() { --count; return regs[addr][cursor++]; }
};
int main() {
  FakeWire bus;
  v3::Imu<FakeWire> imu(bus);
  imu.begin(0);
  for(unsigned t=0;t<100;++t) imu.tick(t);
  assert(imu.state()==v3::ImuState::Missing);
  assert(bus.writes.empty()); // Missing devices must never be configured/reset.
  assert(bus.transactions==2); // Only WHO_AM_I at the two expected candidates.
  for(unsigned t=100;t<10000;++t) imu.tick(t);
  assert(bus.transactions==2); // No busy retry loop.
  // ACK/wrong ID must not authorize writes. Probe both, preserve actual IDs.
  for(int slot=0;slot<2;++slot) {
    FakeWire wrong; wrong.present[slot]=true; wrong.regs[slot][0]=0x01;
    v3::Imu<FakeWire> d(wrong); d.begin(0);
    for(unsigned t=0;t<100;++t) d.tick(t);
    assert(d.state()==v3::ImuState::Identity && wrong.writes.empty());
    assert(d.candidate(slot).read_ok && d.candidate(slot).id==1);
  }
  FakeWire good; good.present[1]=true; good.regs[1][0]=5; good.regs[1][1]=0x42;
  v3::Imu<FakeWire> d(good); d.begin(0);
  for(unsigned t=0;t<100;++t) d.tick(t);
  assert(d.state()==v3::ImuState::Running && d.address()==0x6b && d.revision()==0x42);
  const std::vector<std::array<int,3>> expected={
    {0x6b,8,0},{0x6b,2,0x40},{0x6b,3,0x17},{0x6b,4,0x57},
    {0x6b,6,0},{0x6b,7,0},{0x6b,8,0x43}};
  assert(good.writes==expected); // No reset/CTRL9/unrelated peripherals.
  for(auto &write:expected) assert(good.regs[1][write[1]]==uint8_t(write[2]) || write[1]==8);
  for(uint8_t id : {uint8_t(0),uint8_t(255)}) {
    FakeWire wrong; wrong.present={true,true}; wrong.regs[0][0]=id; wrong.regs[1][0]=id;
    v3::Imu<FakeWire> w(wrong); w.begin(0);
    for(unsigned i=0;i<20;++i) w.tick(i);
    assert(w.state()==v3::ImuState::Identity && wrong.writes.empty());
  }
  FakeWire dual; dual.present={true,true}; dual.regs[0][0]=5; dual.regs[1][0]=5;
  v3::Imu<FakeWire> both(dual); both.begin(0);
  for(unsigned i=0;i<20;++i) { auto n=dual.transactions; both.tick(i); assert(dual.transactions-n<=2); }
  assert(both.state()==v3::ImuState::Running && both.address()==0x6a);
  assert(both.candidate(0).id==5 && both.candidate(1).id==5);
  assert(std::strcmp(both.status(),"IMU: waiting")==0);
  puts("PASS: identity-gated exact original-family config, observed address/revision (host-only)");
  auto raw=[&](unsigned axis,int16_t value) {
    good.regs[1][0x35+axis*2]=uint16_t(value)&255;
    good.regs[1][0x36+axis*2]=uint16_t(value)>>8;
  };
  good.regs[1][0x2e]=3;
  raw(0,-8192); raw(1,4096); raw(2,-1); raw(3,-64); raw(4,128); raw(5,-16384);
  auto tx=good.transactions;
  d.tick(249); assert(good.transactions==tx && !d.sample().valid);
  d.tick(250); assert(d.sample().valid && d.sample().at_ms==250);
  assert(d.sample().acc_g[0]==-1 && d.sample().acc_g[1]==0.5f && d.sample().acc_g[2]==-4.f/32768);
  assert(d.sample().gyr_dps[0]==-1 && d.sample().gyr_dps[1]==2 && d.sample().gyr_dps[2]==-256);
  tx=good.transactions; d.tick(251); assert(good.transactions==tx);
  good.short_count=11; d.tick(500); assert(!d.sample().valid && d.read_errors()==1);
  good.short_count=-1; d.tick(750); assert(d.sample().valid);
  good.fail_reg=0x2e; d.tick(1000); assert(!d.sample().valid && d.read_errors()==2);
  good.fail_reg=-1; good.regs[1][0x2e]=1; d.tick(1250); assert(!d.sample().valid);
  good.regs[1][0x2e]=3;
  for(int i=0;i<6;++i) raw(i,0);
  d.tick(1500); assert(!d.sample().valid); // all-zero bus frame is not real evidence
  for(int i=0;i<6;++i) raw(i,-1);
  d.tick(1750); assert(!d.sample().valid); // all-ff frame
  raw(0,32767); d.tick(2000); assert(!d.sample().valid); // clipped sample
  raw(0,-32768); d.tick(2250); assert(!d.sample().valid);
  raw(0,8192); d.tick(2500); assert(d.sample().valid);
  // Scheduling wraps millis; begin() clears old sample/config state.
  d.begin(UINT32_MAX-100); for(unsigned i=0;i<20;++i) d.tick(UINT32_MAX-100+i);
  tx=good.transactions; d.tick(148); assert(good.transactions==tx);
  d.tick(149); assert(d.sample().valid);
  for(int mode=0;mode<4;++mode) {
    FakeWire bad; bad.present[0]=true; bad.regs[0][0]=5;
    v3::Imu<FakeWire> b(bad); b.begin(0); b.tick(0); b.tick(1);
    if(mode==0) bad.fail_reg=1; // revision read failure
    if(mode==1) bad.nack_write=true;
    if(mode==2) bad.corrupt=true; // configuration readback mismatch
    if(mode==3) bad.fail_reg=2; // CTRL1 read failure
    for(unsigned i=2;i<100;++i) b.tick(i);
    assert(b.state()==v3::ImuState::ConfigError && !b.sample().valid);
    auto n=bad.transactions; b.tick(100000); assert(bad.transactions==n);
  }
  FakeWire short_id; short_id.present[0]=true; short_id.regs[0][0]=5; short_id.short_count=0;
  v3::Imu<FakeWire> s(short_id); s.begin(0); s.tick(0); s.tick(1);
  assert(s.state()==v3::ImuState::Missing && short_id.writes.empty());
  puts("PASS: signed axes, timing/wrap, fresh-data gate, short reads, invalid frames, config failures (host-only)");
}
