// Synthetic transport only; no device trace claims.
#include <cassert>
#include <string>
#include <algorithm>
#include <cstdio>
#include "../trailguide_v3/motion_diagnostics.h"
#include "../trailguide_v3/gesture.h"
struct Snapshot {
 bool pointer=false,scrolling=false,options=false,reveal=false,eligible=true,root=true,healthy=true;unsigned view=0;
 v3app::Gesture::Diagnostic gesture;
 uint32_t key()const{return unsigned(eligible)|(unsigned(gesture.phase)<<8);}
};
struct Transport {
 std::string rx,tx; size_t pos=0; int capacity=32; size_t partial=7; bool connected=true;
 explicit operator bool()const{return connected;}
 int available(){return int(rx.size()-pos);} int read(){return pos<rx.size()?rx[pos++]:-1;}
 int availableForWrite(){return capacity;}
 size_t write(const uint8_t *p,size_t n){assert(n<=size_t(capacity));n=std::min(n,partial);tx.append(reinterpret_cast<const char*>(p),n);return n;}
 void setTxTimeoutMs(uint32_t n){assert(n==0);}
};
int main(){
 v3app::MotionDiagnostics d; Transport t;
 assert(!d.active());t.rx="MOT";d.service(t,0);assert(!d.active());t.rx+="ION\n";d.service(t,1);assert(d.active());
 d.service(t,45001);assert(!d.active());
 for(int i=0;i<200;++i){d.service(t,45002+i);}assert(!d.draining());assert(t.tx.find("end=timeout")!=std::string::npos);
 // Input overflow cannot activate a suffix, and work per loop is bounded.
 {v3app::MotionDiagnostics x;Transport s;s.rx=std::string(100,'x')+"MOTION\n";x.service(s,0);assert(s.pos==16 && !x.active());
  while(s.available()){x.service(s,1);}assert(!x.active());s.rx+="MOTION\n";x.service(s,2);assert(x.active());x.service(s,44000);assert(x.active());x.service(s,45002);assert(!x.active());}
 {v3app::MotionDiagnostics x;Transport s;s.rx="ASK\nMOTIONx\n";x.service(s,0);assert(!x.active() && s.tx.empty());}
 // OFF observes nothing. Repeated 5ms callers do not duplicate fresh frames.
 {v3app::MotionDiagnostics x;Transport s;v3::ImuSample sample;sample.valid=true;sample.at_ms=20;Snapshot a;
  x.observe(s,20,sample,"IMU: sample",a,a,false);assert(x.observations()==0 && s.tx.empty());
  s.rx="MOTION\n";x.service(s,0);x.observe(s,20,sample,"IMU: sample",a,a,false);
  for(unsigned t=21;t<60;++t){x.observe(s,t,sample,"IMU: sample",a,a,false);}assert(x.observations()==1);
  x.observe(s,60,sample,"IMU: sample",a,a,false);assert(x.observations()==2);
  s.rx+="MOTION\n";x.service(s,44000);x.service(s,45000);assert(!x.active());}
 // Timeout wraps safely and zero write capacity never blocks or breaks a line.
 {v3app::MotionDiagnostics x;Transport s;s.capacity=0;s.rx="MOTION\n";x.service(s,0xfffffff0u);
  v3::ImuSample sample;sample.valid=true;Snapshot a;x.observe(s,0xfffffff0u,sample,"IMU: sample",a,a,false);
  for(unsigned t=0;t<100;++t){sample.at_ms=t*20;x.observe(s,t*20,sample,"IMU: sample",a,a,false);x.service(s,t*20);}
  assert(x.dropped()>0 && s.tx.empty());x.service(s,44984);assert(!x.active());s.capacity=32;
  for(unsigned i=0;i<500;++i){x.service(s,44985+i);}assert(!x.draining() && s.tx.back()=='\n');}
 // A lost not-ready transition still survives in cumulative session evidence.
 {v3app::MotionDiagnostics x;Transport s;s.capacity=0;s.rx="MOTION\n";x.service(s,0);
  v3::ImuSample sample;sample.valid=true;sample.at_ms=20;Snapshot a,b;a.gesture.phase=1;b.gesture.phase=4;
  x.observe(s,20,sample,"IMU: sample",a,a,false);sample.valid=false;x.observe(s,40,sample,"IMU: waiting",a,b,false);assert(x.dropped()==1);
  sample.valid=true;sample.at_ms=60;x.observe(s,60,sample,"IMU: sample",b,b,false);
  x.service(s,45000);s.capacity=64;for(unsigned i=0;i<500;++i){x.service(s,45001+i);}
  assert(s.tx.find("bad=1 wait=1 last_bad=40:1>4")!=std::string::npos && !x.draining());}
 for(int capacity:{16,32,64}){
  v3app::MotionDiagnostics x;Transport s;s.capacity=capacity;s.partial=5;s.rx="MOTION\n";x.service(s,0);
  v3::ImuSample sample;sample.valid=true;sample.at_ms=20;sample.acc_g[2]=1;
  v3app::Gesture detector;Snapshot a;a.gesture=detector.diagnostic(sample,20);
  x.observe(s,20,sample,"IMU: sample",a,a,false);assert(x.observations()==1);
  for(unsigned i=0;i<10;++i){sample.at_ms+=20;x.observe(s,sample.at_ms,sample,"IMU: sample",a,a,false);x.service(s,sample.at_ms);}
  assert(x.dropped()==10);
  for(unsigned i=0;i<300;++i)x.service(s,1000+i);
  assert(!x.draining() && s.tx.back()=='\n');assert(std::count(s.tx.begin(),s.tx.end(),'\n')==1);
  assert(s.tx.find("TG_MOTION1 t=20 at=20 age=0 gap=0 fresh=1 valid=1") == 0);
  // A validity change without a fresh timestamp must be reported immediately.
  sample.valid=false;x.observe(s,1300,sample,"IMU: waiting",a,a,false);
  for(unsigned i=0;i<300;++i)x.service(s,1300+i);
  assert(s.tx.find("valid=0 status=\"IMU: waiting\"")!=std::string::npos);
  s.connected=false;sample.at_ms+=20;x.observe(s,2000,sample,"IMU: waiting",a,a,false);assert(x.dropped()==11);
  s.connected=true;x.service(s,45000);for(unsigned i=0;i<300;++i)x.service(s,45001+i);
  assert(!x.active() && !x.draining());assert(s.tx.find("obs=13 drop=11")!=std::string::npos);
 }
 puts("PASS motion: partial-line/overflow/16-byte RX bound/timeout; 16/32/64-byte TX + partial writes/newline integrity/drops/disconnect/invalid transition");
}
