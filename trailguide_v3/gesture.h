#pragma once
#include "imu.h"
#include <cmath>
#define V3_GESTURE_RESET_DIAGNOSTICS 1
namespace v3app {
// EXPERIMENTAL defaults supported by synthetic host tests, NOT physical acceptance.
// Instruction: hold screen up, turn screen down and pause, turn back up and pause.
// Native axes/signs need no mapping: compare unit gravity with a settled origin.
// Heading-only rotation is unobservable; this does NOT recognize every possible
// turn from every starting pose. Touch remains sufficient; eligible is UI-owned.
class Gesture {
 enum class Phase { Acquire, Ready, Turning, Inverted, Recover };
 Phase phase=Phase::Acquire;
 float origin[3]={}, held[3]={};
 bool haveOrigin=false, seen=false, dwelling=false, cooling=false;
 uint32_t last=0, dwellAt=0, cycleAt=0, firedAt=0;
 unsigned dwellSamples=0;uint32_t resetGeneration=0;uint8_t lastResetReason=0;
 // Bounds/dwells are hypotheses to measure on-device, not calibrated constants.
 static constexpr uint32_t RestMs=600, PoseMs=180, RebaseMs=1200;
 static constexpr uint32_t MaxGapMs=250, MaxAgeMs=60, CycleMs=12000, CooldownMs=2500;
 static float dot(const float *a,const float *b){
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
 }
 static void copy(float *to,const float *from){for(unsigned i=0;i<3;++i)to[i]=from[i];}
 void clearDwell(){dwelling=false;dwellSamples=0;}
 bool hold(const float *unit,uint32_t at,uint32_t duration){
  // A stable direction, not just a stable 1g magnitude. Four genuinely fresh
  // frames AND elapsed dwell prevent a timestamp jump from completing a pose.
  if(!dwelling || dot(unit,held)<.985f){
   dwelling=true;dwellAt=at;dwellSamples=0;copy(held,unit);
  }
  if(dwellSamples<4)++dwellSamples;
  return dwellSamples>=4 && uint32_t(at-dwellAt)>=duration;
 }
 public:
 struct Diagnostic {
  unsigned phase=0,dwellSamples=0;uint32_t resets=0;uint8_t resetReason=0;
  bool haveOrigin=false,dwelling=false,seen=false,cooling=false,rest=false;
  float origin[3]={},held[3]={},alignment=0,magnitude=0,rate=0;
  uint32_t dwellMs=0,last=0,cycleAge=0,reject=0;
 };
 Diagnostic diagnostic(const v3::ImuSample &s,uint32_t now)const{
  Diagnostic d;d.resets=resetGeneration;d.resetReason=lastResetReason;d.phase=unsigned(phase);d.dwellSamples=dwellSamples;
  d.haveOrigin=haveOrigin;d.dwelling=dwelling;d.seen=seen;d.cooling=cooling;
  copy(d.origin,origin);copy(d.held,held);d.last=last;
  d.dwellMs=dwelling?uint32_t(s.at_ms-dwellAt):0;
  d.cycleAge=(phase==Phase::Turning || phase==Phase::Inverted)?uint32_t(now-cycleAt):0;
  // Read-only rejection predicates, not an additional detector dispatch.
  // bits: invalid, age, duplicate, gap, bad/clipped axis, cooldown, cycle timeout.
  d.reject=(!s.valid?1u:0u)|(uint32_t(now-s.at_ms)>MaxAgeMs?2u:0u)|
   (seen && s.at_ms==last?4u:0u)|(seen && uint32_t(s.at_ms-last)>MaxGapMs?8u:0u)|
   (cooling && uint32_t(now-firedAt)<CooldownMs?32u:0u)|(d.cycleAge>CycleMs?64u:0u);
  float a2=0,g2=0;for(unsigned i=0;i<3;++i){a2+=s.acc_g[i]*s.acc_g[i];g2+=s.gyr_dps[i]*s.gyr_dps[i];
   if(!std::isfinite(s.acc_g[i]) || !std::isfinite(s.gyr_dps[i]) || std::fabs(s.acc_g[i])>=3.99f || std::fabs(s.gyr_dps[i])>=511.f)d.reject|=16u;
  }
  d.magnitude=std::sqrt(a2);d.rate=std::sqrt(g2);
  d.rest=std::fabs(d.magnitude-1.f)<.16f && d.rate<12.f;
  d.alignment=d.magnitude>0?dot(origin,s.acc_g)/d.magnitude:0;
  return d;
 }
 // Preserve reference across cancellation until a fresh eligible recovery hold
 // qualifies. Active cycles never rebase. Keep freshness/cooldown across resets.
 void reset(uint8_t reason=10){lastResetReason=reason;++resetGeneration;phase=haveOrigin ? Phase::Recover : Phase::Acquire;clearDwell();}
 bool consume(const v3::ImuSample &s,uint32_t now,bool eligible){
  if(!eligible){reset(1);seen=true;last=s.at_ms;return false;}
  if(!s.valid || uint32_t(now-s.at_ms)>MaxAgeMs){reset(!s.valid?2:3);return false;}
  if(seen && (s.at_ms==last || uint32_t(s.at_ms-last)>MaxGapMs)){
   const uint8_t reason=s.at_ms==last?4:5;last=s.at_ms;reset(reason);return false;
  }
  seen=true;last=s.at_ms;
  float a2=0,g2=0;
  for(unsigned i=0;i<3;++i){
   if(!std::isfinite(s.acc_g[i]) || !std::isfinite(s.gyr_dps[i]) ||
      std::fabs(s.acc_g[i])>=3.99f || std::fabs(s.gyr_dps[i])>=511.f){reset(6);return false;}
   a2+=s.acc_g[i]*s.acc_g[i];g2+=s.gyr_dps[i]*s.gyr_dps[i];
  }
  if(cooling){
   if(uint32_t(now-firedAt)<CooldownMs){reset(7);return false;}
   cooling=false; // Rest must be acquired AFTER cooldown, not accumulated in it.
  }
  if((phase==Phase::Turning || phase==Phase::Inverted) &&
     uint32_t(s.at_ms-cycleAt)>CycleMs){reset(8);return false;}
  const float mag=std::sqrt(a2);
  // Rest-only gyro gate: 12dps permits bounded uncalibrated bias (e.g. 2.2dps)
  // plus modest noise. NOT a bias estimate; >12dps while held cannot qualify.
  // Turning samples have NO 45dps blanket veto and need not remain near 1g.
  const bool rest=std::fabs(mag-1.f)<.16f && std::sqrt(g2)<12.f;
  if(!rest){clearDwell();return false;}
  float unit[3];for(unsigned i=0;i<3;++i)unit[i]=s.acc_g[i]/mag;
  if(phase==Phase::Acquire){
   if(hold(unit,s.at_ms,RestMs)){
    copy(origin,held);haveOrigin=true;phase=Phase::Ready;clearDwell();
   }
   return false;
  }
  const float alignment=dot(origin,unit);
  if(phase==Phase::Recover){
   // Cancellation ends the old cycle. A stable eligible held pose may become
   // the new reference even opposite the old one: vetoing inversion here can
   // lock Recover forever. Preserve the old origin until the longer rebase
   // qualifies; reacquisition itself never completes a gesture or draws.
   if(hold(unit,s.at_ms,alignment>=.94f ? RestMs : RebaseMs)){
    copy(origin,held);phase=Phase::Ready;cycleAt=0;clearDwell();
   }
   return false;
  }
  if(phase==Phase::Ready){
   if(alignment>=.8f)return false;
   phase=Phase::Turning;cycleAt=s.at_ms;clearDwell();
  }
  const bool pose=phase==Phase::Turning ? alignment<=-.85f : alignment>=.94f;
  if(!pose){clearDwell();return false;}
  if(!hold(unit,s.at_ms,PoseMs))return false;
  clearDwell();
  if(phase==Phase::Turning){phase=Phase::Inverted;return false;}
  firedAt=now;cooling=true;reset(9);return true;
 }
};
} // namespace v3app
