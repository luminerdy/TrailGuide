// All input is clearly SYNTHETIC; no captured human movement.
#include <cassert>
#include <cstdio>
#include <cstdlib>
#if __has_include("../trailguide_v3/flight_recorder.h")
#include "../trailguide_v3/flight_recorder.h"
#include <string>
#include <algorithm>
struct Transport {
 std::string tx;int capacity=16;size_t partial=3;bool connected=true;
 explicit operator bool()const{return connected;}
 int availableForWrite(){return capacity;}
 size_t write(const uint8_t *p,size_t n){assert(n<=size_t(capacity) && n<=64);n=std::min(n,partial);tx.append(reinterpret_cast<const char*>(p),n);return n;}
};
static unsigned allocations=0;static size_t allocatedBytes=0;
static void *allocate(size_t n){++allocations;allocatedBytes=n;return malloc(n);}
static void *failAllocate(size_t){++allocations;return nullptr;}
int main(){
 v3app::FlightRecorder f(allocate,free);
 assert(!f.active() && allocations==0 && f.ram.records==nullptr);
 assert(f.start(0));assert(allocations==1 && f.active());assert(allocatedBytes==8192*40 && f.ram.capacity==8192);
 v3app::FlightRecord r{};r.poll_ms=20;r.raw[5]=32767;r.flags=64;r.clip=32;
 f.append(r);assert(f.ram.count==1 && f.ram.records[0].raw[5]==32767);
 f.tick(119999);assert(f.active());f.tick(120000);assert(!f.active() && f.ram.state==2 && f.ram.count==1);
 for(int cap:{16,32,64}){
  Transport t;t.capacity=cap;
  assert(f.command("STATUS 12345",100000));
  for(uint32_t n=100000;n<101000;++n)f.service(t,n);
  assert(!f.draining() && t.tx.find("token=12345")!=std::string::npos);
  t.tx.clear();assert(f.command("DUMP 1 0 1 9876",102000));
  for(uint32_t n=102000;n<104000;++n)f.service(t,n);
  assert(!f.draining() && t.tx.find("TGFR1 END session=1 token=9876")!=std::string::npos);
  if(cap==16){FILE *wire=std::fopen("build/v3-0.1.14/recorder.synthetic.wire","wb");assert(wire);std::fwrite(t.tx.data(),1,t.tx.size(),wire);std::fclose(wire);}
 }
 Transport stalled;stalled.capacity=0;
 assert(f.command("DUMP 1 0 1 42",200000));f.service(stalled,200000);f.service(stalled,205000);
 assert(!f.draining() && f.ram.count==1);
 assert(f.command("DUMP 1 0 1 43",206000));stalled.connected=false;f.service(stalled,206000);
 assert(!f.draining() && f.ram.crc32==v3app::FlightRecorder::crc(f.ram.records,40));
 assert(f.start(0xfffffff0u));assert(allocations==1);f.tick(119983);assert(f.active());f.tick(119984);assert(!f.active());
 f.clear();assert(f.ram.state==0 && f.ram.count==0);assert(f.start(0));
 for(unsigned i=0;i<v3app::FlightRecorder::Capacity;++i){r.poll_ms=i;f.append(r);}
 assert(f.ram.state==2 && f.ram.reason==2 && f.ram.count==8192);
 f.append(r);assert(f.ram.count==8192);
 v3app::FlightRecorder missing;assert(!missing.start(0) && missing.ram.error==2 && !missing.active());
 const auto allocationsBefore=allocations;v3app::FlightRecorder failed(failAllocate,free);
 assert(!failed.start(0) && failed.ram.error==2 && failed.ram.records==nullptr && allocations==allocationsBefore+1);
 assert(f.command("DUMP 4 0 33 123",5000) && f.ram.error==3 && f.ram.count==8192);
 f.clear();assert(f.command("RECORD 123456",6000) && f.ram.session==123456 && f.active());
 const auto saved=f.ram.session;assert(!f.start(6001) && f.ram.session==saved && f.ram.error==1);
 f.clear();assert(f.active() && f.ram.error==1);f.tick(126000);f.clear();assert(f.ram.state==0);
 // Malformed numeric commands must never clear/replace frozen evidence.
 v3app::FlightRecorder strict(allocate,free);assert(strict.start(0,123));strict.append(r);strict.tick(120000);
 const auto frozen=strict.ram;const auto frozenRow=strict.ram.records[0];const auto oldAllocations=allocations;
 for(const char *bad:{"RECORD -1","REC +1","RECORD 4294967296","RECORD 18446744073709551616","RECORD 1junk","REC junk","REC 1 2","CLEAR -1","CLEAR 4294967296","STATUS +4","DUMP 4294967419 0 1 7","DUMP 123 -0 1 7","DUMP 123 0 4294967297 7","DUMP 123 0 1 4294967296","DUMP 123 0 1 7junk"}){
  assert(strict.command(bad,46000));
  assert(strict.ram.state==frozen.state && strict.ram.session==frozen.session && strict.ram.count==frozen.count && strict.ram.started==frozen.started && strict.ram.ended==frozen.ended && strict.ram.crc32==frozen.crc32 && strict.ram.records==frozen.records);
  assert(std::memcmp(strict.ram.records,&frozenRow,sizeof(frozenRow))==0 && allocations==oldAllocations);
  assert(strict.ram.error==3);
 }
 assert(strict.command("STATUS 4294967295",47000));Transport bounds;for(uint32_t n=47000;n<49000;++n)strict.service(bounds,n);assert(bounds.tx.find("token=4294967295")!=std::string::npos);
 assert(strict.command("REC 4294967295",50000) && strict.active() && strict.ram.session==0xffffffffu);
 // Manual freeze is strict-session and retry-safe; errors may change only error/transport.
 v3app::FlightRecorder manual(allocate,free);assert(manual.start(50,77));r.poll_ms=51;manual.append(r);
 const auto activeHeader=manual.ram;
 assert(manual.command("FREEZE 0 900",55) && manual.active() && manual.ram.count==activeHeader.count && manual.ram.session==activeHeader.session);
 assert(manual.command("FREEZE 78 900",55) && manual.active() && manual.ram.ended==0 && manual.ram.reason==0);
 assert(manual.command("FREEZE 77 901",60));assert(manual.ram.state==2 && manual.ram.reason==3 && manual.ram.ended==60);
 const auto header=manual.ram;const auto row=manual.ram.records[0];
 Transport manualWire;manualWire.partial=64;manualWire.capacity=64;
 for(uint32_t t=60;t<100;++t)manual.service(manualWire,t);
 assert(manualWire.tx.find("capacity=8192")!=std::string::npos && manualWire.tx.find("reason=3")!=std::string::npos);
 assert(manual.command("DUMP 77 0 1 905",100));for(uint32_t t=100;t<200;++t)manual.service(manualWire,t);
 FILE *manualFile=std::fopen("build/v3-0.1.14/recorder.manual.synthetic.wire","wb");assert(manualFile);std::fwrite(manualWire.tx.data(),1,manualWire.tx.size(),manualFile);std::fclose(manualFile);
 assert(manual.command("FREEZE 77 902",90));assert(manual.ram.error==0);
 for(const char *bad:{"FREEZE","FREEZE 77","FREEZE 77 1 2","FREEZE 0 1","FREEZE 78 1","FREEZE -77 1","FREEZE +77 1","FREEZE 4294967296 1","FREEZE 77 4294967296","FREEZE 77 1junk","FREEZE 77 -1","FREEZE 77 +1","FREEZE 18446744073709551616 1","FREEZE junk 1"}){
  assert(manual.command(bad,100));assert(manual.ram.error==3);
  assert(manual.ram.session==header.session && manual.ram.state==header.state && manual.ram.count==header.count && manual.ram.started==header.started && manual.ram.ended==header.ended && manual.ram.reason==header.reason && manual.ram.crc32==header.crc32 && manual.ram.records==header.records);
  assert(std::memcmp(&row,manual.ram.records,sizeof(row))==0);
 }
 manual.append(r);assert(manual.ram.count==1);assert(manual.command("FREEZE 77 903",120) && manual.ram.error==0);
 v3app::FlightRecorder idle(allocate,free);assert(idle.command("FREEZE 77 1",0) && idle.ram.state==0 && idle.ram.records==nullptr && idle.ram.error==3);
 // If timeout wins the race, FREEZE preserves timeout reason/end/CRC.
 assert(manual.start(0,78));manual.append(r);manual.tick(120000);const auto timed=manual.ram;
 assert(manual.command("FREEZE 78 904",120001));assert(manual.ram.reason==1 && manual.ram.ended==timed.ended && manual.ram.crc32==timed.crc32);
 puts("PASS synthetic recorder opt-in PSRAM allocation, lossless raw RAM, timeout freeze; strict uint32 commands preserve frozen evidence");
}
#else
int main(){assert(!"bounded flight recorder missing");}
#endif
