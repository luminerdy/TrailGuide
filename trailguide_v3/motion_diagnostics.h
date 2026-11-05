#pragma once
#include <stdint.h>
#include <stddef.h>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "flight_recorder.h"
#define V3_FLIGHT_RECORDER 1
namespace v3app {
// One fixed pending line, no queue, allocation, flush, or UI command path.
// Serial mutex/write calls have a finite 1ms budget, not strict wait-freedom.
class MotionDiagnostics {
 char command_[80]={}, pending_[768]={};
 size_t used_=0,length_=0,cursor_=0;
 bool overflow_=false,carriageReturn_=false,active_=false,have_=false,ending_=false;
 uint32_t started_=0,lastReport_=0,lastAt_=0,lastKey_=0,dropped_=0,observed_=0;
 uint32_t invalidEvents_=0,waitingEvents_=0,phaseEvents_=0,triggers_=0,lastInvalid_=0;
 unsigned invalidFrom_=0,invalidTo_=0,lastPhase_=0;bool lastValid_=false;
 const char *lastStatus_="";
 // Integer-only printf avoids libc float-format allocation. Nonfinite/clipped
 // diagnostic conversion uses a sentinel; the original float sample is untouched.
 static long fixed(float value,float scale){
  const double v=double(value)*double(scale);
  if(!std::isfinite(v) || v>2147483646.0 || v< -2147483646.0)return 2147483647L;
  return static_cast<long>(v);
 }
 public:
 FlightRecorder recorder;
 static constexpr uint32_t DurationMs=45000;
 bool active()const{return active_ || recorder.active();}
 bool draining()const{return cursor_<length_ || ending_ || recorder.draining();}
 uint32_t dropped()const{return dropped_;}
 uint32_t observations()const{return observed_;}
 template<class Serial> void service(Serial &serial,uint32_t now){
  recorder.tick(now);
  if(active_ && uint32_t(now-started_)>=DurationMs){active_=false;ending_=true;}
  if(ending_ && cursor_>=length_){
   length_=size_t(std::snprintf(pending_,sizeof(pending_),"TG_MOTION1 end=timeout duration_ms=45000 obs=%lu drop=%lu bad=%lu wait=%lu last_bad=%lu:%u>%u phases=%lu fires=%lu\n",(unsigned long)observed_,(unsigned long)dropped_,(unsigned long)invalidEvents_,(unsigned long)waitingEvents_,(unsigned long)lastInvalid_,invalidFrom_,invalidTo_,(unsigned long)phaseEvents_,(unsigned long)triggers_));
   cursor_=0;ending_=false;
  }
  // Bounded input even under garbage/flood. Overflow discards through newline.
  for(unsigned budget=0;budget<16 && serial.available()>0;++budget){
   const int c=serial.read();if(c<0)break;
   if(c=='\n'){
    command_[used_]=0;
    if(!overflow_ && recorder.command(command_,now)){active_=false;ending_=false;length_=cursor_=0;serial.setTxTimeoutMs(1);}
    else
    if(!overflow_ && used_==6 && std::memcmp(command_,"MOTION",6)==0 && !active() && !draining()){
     active_=true;started_=now;have_=false;dropped_=observed_=0;
     invalidEvents_=waitingEvents_=phaseEvents_=triggers_=lastInvalid_=0;invalidFrom_=invalidTo_=0;lastValid_=false;
     // SDK 3.3.2 HWCDC::write timeout 0 underflows unsigned tries on no
     // progress. Keep a finite positive budget during AND after diagnostics.
     serial.setTxTimeoutMs(1); // 1ms mutex bound, not strictly wait-free.
    }
    used_=0;overflow_=false;carriageReturn_=false;
   }else{
    // CR is valid only as the final byte of CRLF, including across budgets.
    // Reject the whole frame, never let a C-string terminator hide a suffix.
    if(carriageReturn_)overflow_=true;
    if(c=='\r')carriageReturn_=true;
    else if(c<32 || c>126)overflow_=true; // ASCII command protocol only.
    else if(!overflow_){
     if(used_<sizeof(command_)-1)command_[used_++]=char(c);else overflow_=true;
    }
   }
  }
  if(recorder.quiet()){recorder.service(serial,now);return;}
  // Capacity-sized writes avoid HWCDC's remainder/wait loop. A partial write
  // retains the cursor: never splice the next observation into an old line.
  if(draining() && bool(serial)){
   const int capacity=serial.availableForWrite();
   if(capacity>0){size_t n=length_-cursor_;if(n>256)n=256;if(n>size_t(capacity))n=size_t(capacity);
    const size_t sent=serial.write(reinterpret_cast<const uint8_t*>(pending_+cursor_),n);
    cursor_+=sent<n?sent:n;
   }
  }
 }
 // Snapshot is a value read of the actual UI/detector, never consume/reset.
 template<class Serial,class Sample,class Snapshot>
 void observe(Serial &serial,uint32_t now,const Sample &s,const char *status,
              const Snapshot &before,const Snapshot &after,bool triggered){
  if(!active_)return;
  const uint32_t key=after.key() | (uint32_t(s.valid)<<20);
  const bool fresh=!have_ || s.at_ms!=lastAt_;
  const uint32_t gap=have_?uint32_t(s.at_ms-lastAt_):0;
  const bool changed=!have_ || key!=lastKey_ || triggered || std::strcmp(status,lastStatus_)!=0;
  if(!fresh && !changed && uint32_t(now-lastReport_)<40)return;
  if(!s.valid && (!have_ || lastValid_)){
   ++invalidEvents_;if(std::strcmp(status,"IMU: waiting")==0)++waitingEvents_;
   lastInvalid_=now;invalidFrom_=before.gesture.phase;invalidTo_=after.gesture.phase;
  }
  if(have_ && lastPhase_!=after.gesture.phase)++phaseEvents_;
  lastPhase_=after.gesture.phase;lastStatus_=status;
  if(triggered)++triggers_;
  lastValid_=s.valid;
  have_=true;lastAt_=s.at_ms;lastKey_=key;lastReport_=now;++observed_;
  if(draining() || !bool(serial)){++dropped_;return;}
  const auto &g=after.gesture;
  const int n=std::snprintf(pending_,sizeof(pending_),
   "TG_MOTION1 t=%lu at=%lu age=%lu gap=%lu fresh=%u valid=%u status=\"%s\" acc_g_e6=%ld,%ld,%ld gyro_dps_e3=%ld,%ld,%ld gates=%u,%u,%u,%u,%u root=%u view=%u healthy=%u reject=%lu phase=%u>%u origin_e6=%u:%ld,%ld,%ld align_e6=%ld mag_g_e6=%ld rate_dps_e3=%ld rest=%u dwell=%u:%lu:%u held_e6=%ld,%ld,%ld last=%lu seen=%u cooling=%u cycle_age=%lu trigger=%u obs=%lu drop=%lu bad=%lu wait=%lu last_bad=%lu:%u>%u phases=%lu fires=%lu\n",
   (unsigned long)now,(unsigned long)s.at_ms,(unsigned long)uint32_t(now-s.at_ms),(unsigned long)gap,
   unsigned(fresh),unsigned(s.valid),status,fixed(s.acc_g[0],1e6f),fixed(s.acc_g[1],1e6f),fixed(s.acc_g[2],1e6f),fixed(s.gyr_dps[0],1e3f),fixed(s.gyr_dps[1],1e3f),fixed(s.gyr_dps[2],1e3f),
   unsigned(after.pointer),unsigned(after.scrolling),unsigned(after.options),unsigned(after.reveal),unsigned(after.eligible),unsigned(after.root),after.view,unsigned(after.healthy),(unsigned long)before.gesture.reject,
   before.gesture.phase,g.phase,unsigned(g.haveOrigin),fixed(g.origin[0],1e6f),fixed(g.origin[1],1e6f),fixed(g.origin[2],1e6f),fixed(g.alignment,1e6f),fixed(g.magnitude,1e6f),fixed(g.rate,1e3f),unsigned(g.rest),unsigned(g.dwelling),(unsigned long)g.dwellMs,g.dwellSamples,fixed(g.held[0],1e6f),fixed(g.held[1],1e6f),fixed(g.held[2],1e6f),(unsigned long)g.last,unsigned(g.seen),unsigned(g.cooling),(unsigned long)g.cycleAge,unsigned(triggered),(unsigned long)observed_,(unsigned long)dropped_,(unsigned long)invalidEvents_,(unsigned long)waitingEvents_,(unsigned long)lastInvalid_,invalidFrom_,invalidTo_,(unsigned long)phaseEvents_,(unsigned long)triggers_);
  if(n<=0 || size_t(n)>=sizeof(pending_)){++dropped_;length_=cursor_=0;return;}
  length_=size_t(n);cursor_=0;
 }
};
}
