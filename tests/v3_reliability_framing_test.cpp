// Synthetic transport; exercises the actual bounded serial-facing parser.
#include "../trailguide_v3/motion_diagnostics.h"
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstdio>
struct SerialInput {
 std::string bytes,tx; size_t cursor=0;unsigned reads=0,writes=0;
 int available(){return int(bytes.size()-cursor);}
 int read(){++reads;return uint8_t(bytes[cursor++]);}
 int availableForWrite(){return 256;}
 size_t write(const uint8_t*p,size_t n){assert(n<=256);++writes;tx.append(reinterpret_cast<const char*>(p),n);return n;}
 void setTxTimeoutMs(uint32_t n){assert(n==1);}
 explicit operator bool()const{return true;}
};
static unsigned allocations=0;
static void* allocate(size_t n){++allocations;return std::malloc(n);}
static void feed(v3app::MotionDiagnostics&d,SerialInput&s){while(s.available()){const auto r=s.reads;d.service(s,120020);assert(s.reads-r<=16);}}
int main(){
 unsigned cases=0;
 for(size_t padding:{size_t(0),size_t(15),size_t(16),size_t(31)})
 for(const std::string command:{"CLEAR","REC 88","FREEZE 77 2","DUMP 77 0 1 4","MOTION"})
 for(unsigned control=0;control<256;++control){
  if(control>=32 && control<=126)continue;
  if(control==10)continue; // newline is the framing delimiter
  for(size_t offset=0;offset<=command.size();++offset){
   v3app::MotionDiagnostics d;d.recorder.allocator(allocate,std::free);
   assert(d.recorder.start(0,77));v3app::FlightRecord row{};row.poll_ms=20;d.recorder.append(row);d.recorder.tick(120000);
   const auto header=d.recorder.ram;const auto oldAllocations=allocations;
   SerialInput s; // prefix spaces deliberately put the bad byte across budgets
   s.bytes=std::string(padding,' ')+command.substr(0,offset)+char(control)+command.substr(offset)+"junk\n";
   feed(d,s);
   assert(std::memcmp(&header,&d.recorder.ram,sizeof(header))==0);
   assert(std::memcmp(&row,d.recorder.ram.records,sizeof(row))==0);
   assert(allocations==oldAllocations && !d.active() && !d.draining() && s.tx.empty());++cases;
   // A malformed line must not poison a following valid CRLF command.
   s.bytes+="STATUS 42\r\n";feed(d,s);for(unsigned i=0;i<50;++i)d.service(s,120020);
   assert(s.tx.find("token=42")!=std::string::npos);
  }
 }
 v3app::MotionDiagnostics active;active.recorder.allocator(allocate,std::free);assert(active.recorder.start(120000,77));
 v3app::FlightRecord activeRow{};active.recorder.append(activeRow);const auto activeHeader=active.recorder.ram;
 SerialInput flood;
 for(unsigned i=0;i<200;++i){const char bad[]="FREEZE 77 2\0suffix\n";flood.bytes+=std::string(bad,sizeof(bad)-1);}
 feed(active,flood);assert(std::memcmp(&activeHeader,&active.recorder.ram,sizeof(activeHeader))==0 && flood.tx.empty());
 v3app::MotionDiagnostics valid;valid.recorder.allocator(allocate,std::free);SerialInput s;s.bytes="REC 99\r\n";feed(valid,s);assert(valid.recorder.active() && valid.recorder.ram.session==99);
 printf("PASS actual service framing: %u malformed lines, bounded reads, unchanged frozen headers/rows/allocation, CRLF recovery\n",cases);
}
