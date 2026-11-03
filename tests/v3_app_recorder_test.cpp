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
static unsigned allocations=0;
static void *allocate(size_t n){++allocations;return malloc(n);}
static void *failAllocate(size_t){++allocations;return nullptr;}
int main(){
 v3app::FlightRecorder f(allocate,free);
 assert(!f.active() && allocations==0 && f.ram.records==nullptr);
 assert(f.start(0));assert(allocations==1 && f.active());
 v3app::FlightRecord r{};r.poll_ms=20;r.raw[5]=32767;r.flags=64;r.clip=32;
 f.append(r);assert(f.ram.count==1 && f.ram.records[0].raw[5]==32767);
 f.tick(45000);assert(!f.active() && f.ram.state==2 && f.ram.count==1);
 for(int cap:{16,32,64}){
  Transport t;t.capacity=cap;
  assert(f.command("STATUS 12345",100000));
  for(uint32_t n=100000;n<101000;++n)f.service(t,n);
  assert(!f.draining() && t.tx.find("token=12345")!=std::string::npos);
  t.tx.clear();assert(f.command("DUMP 1 0 1 9876",102000));
  for(uint32_t n=102000;n<104000;++n)f.service(t,n);
  assert(!f.draining() && t.tx.find("TGFR1 END session=1 token=9876")!=std::string::npos);
  if(cap==16){FILE *wire=std::fopen("build/v3-0.1.8/recorder.synthetic.wire","wb");assert(wire);std::fwrite(t.tx.data(),1,t.tx.size(),wire);std::fclose(wire);}
 }
 Transport stalled;stalled.capacity=0;
 assert(f.command("DUMP 1 0 1 42",200000));f.service(stalled,200000);f.service(stalled,205000);
 assert(!f.draining() && f.ram.count==1);
 assert(f.command("DUMP 1 0 1 43",206000));stalled.connected=false;f.service(stalled,206000);
 assert(!f.draining() && f.ram.crc32==v3app::FlightRecorder::crc(f.ram.records,40));
 assert(f.start(0xfffffff0u));assert(allocations==1);f.tick(44983);assert(f.active());f.tick(44984);assert(!f.active());
 f.clear();assert(f.ram.state==0 && f.ram.count==0);assert(f.start(0));
 for(unsigned i=0;i<v3app::FlightRecorder::Capacity;++i){r.poll_ms=i;f.append(r);}
 assert(f.ram.state==2 && f.ram.reason==2 && f.ram.count==2304);
 f.append(r);assert(f.ram.count==2304);
 v3app::FlightRecorder missing;assert(!missing.start(0) && missing.ram.error==2 && !missing.active());
 const auto allocationsBefore=allocations;v3app::FlightRecorder failed(failAllocate,free);
 assert(!failed.start(0) && failed.ram.error==2 && failed.ram.records==nullptr && allocations==allocationsBefore+1);
 assert(f.command("DUMP 4 0 33 123",5000) && f.ram.error==3 && f.ram.count==2304);
 f.clear();assert(f.command("RECORD 123456",6000) && f.ram.session==123456 && f.active());
 const auto saved=f.ram.session;assert(!f.start(6001) && f.ram.session==saved && f.ram.error==1);
 f.clear();assert(f.active() && f.ram.error==1);f.tick(51000);f.clear();assert(f.ram.state==0);
 // Malformed numeric commands must never clear/replace frozen evidence.
 v3app::FlightRecorder strict(allocate,free);assert(strict.start(0,123));strict.append(r);strict.tick(45000);
 const auto frozen=strict.ram;const auto frozenRow=strict.ram.records[0];const auto oldAllocations=allocations;
 for(const char *bad:{"RECORD -1","REC +1","RECORD 4294967296","RECORD 18446744073709551616","RECORD 1junk","REC junk","REC 1 2","CLEAR -1","CLEAR 4294967296","STATUS +4","DUMP 4294967419 0 1 7","DUMP 123 -0 1 7","DUMP 123 0 4294967297 7","DUMP 123 0 1 4294967296","DUMP 123 0 1 7junk"}){
  assert(strict.command(bad,46000));
  assert(strict.ram.state==frozen.state && strict.ram.session==frozen.session && strict.ram.count==frozen.count && strict.ram.started==frozen.started && strict.ram.ended==frozen.ended && strict.ram.crc32==frozen.crc32 && strict.ram.records==frozen.records);
  assert(std::memcmp(strict.ram.records,&frozenRow,sizeof(frozenRow))==0 && allocations==oldAllocations);
  assert(strict.ram.error==3);
 }
 assert(strict.command("STATUS 4294967295",47000));Transport bounds;for(uint32_t n=47000;n<49000;++n)strict.service(bounds,n);assert(bounds.tx.find("token=4294967295")!=std::string::npos);
 assert(strict.command("REC 4294967295",50000) && strict.active() && strict.ram.session==0xffffffffu);
 puts("PASS synthetic recorder opt-in PSRAM allocation, lossless raw RAM, timeout freeze; strict uint32 commands preserve frozen evidence");
}
#else
int main(){assert(!"bounded flight recorder missing");}
#endif
