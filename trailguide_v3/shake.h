#pragma once
#include "imu.h"
#include <cmath>

namespace v3app {
// PROVISIONAL first-prototype thresholds: synthetic host evidence only, NOT
// physically calibrated/accepted. Recognizes sustained alternating translation,
// not motivation or the cause of any earlier display/flip failure. No hardware,
// allocation, UI dispatch, or dependency on Gesture's reference/state.
class Shake {
 // Central hypotheses, intentionally not public settings.
 static constexpr float PeakG=.65f, MagnitudeDepartureG=.25f;
 static constexpr float OppositeCos=-.5f, QuietToleranceG=.16f;
 static constexpr float QuietDps=12.f, MaxRotationDps=v3::GyroSafetyDps, AccSafetyG=3.99f;
 static constexpr uint32_t BurstMs=900, QuietMs=400, CooldownMs=2200;
 static constexpr uint32_t MaxAgeMs=60, MaxGapMs=120, MaxPeakGapMs=240;
 static constexpr uint32_t MaxCreditMs=20; // Existing driver's poll interval.
 static constexpr unsigned RequiredPeaks=10;

 float previous_[3]={}, gravity_[3]={0,0,1};
 unsigned peaks_=0;
 uint32_t activeMs_=0,lastPeak_=0,firedAt_=0,quietMs_=0,last_=0;
 bool fired_=false,armed_=false,seen_=false;
 static float dot(const float *a,const float *b){
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
 }
 public:
 // Cancel partial intent and require fresh eligible quiet acquisition. Preserves
 // freshness AND fired latch/cooldown: navigation/repaint must not unlock rattle.
 // Keep the same object alive across screens. This is NOT a factory reset.
 void reset(){peaks_=0;armed_=false;quietMs_=0;}

 // Call before legacy !sample.valid early returns. eligible is UI-owned;
 // healthy means sensor available/no KNOWN hard fault, NOT sample.valid.
 // Healthy valid=false (NotReady/noData) pauses without advancing any evidence.
 // Its retained timestamp/values are not a fresh frame. A subsequent fresh gap
 // over MaxGapMs disarms. Without separate fault information, an invalid sample
 // alone cannot distinguish NotReady from a single bus/clipping rejection.
 bool consume(const v3::ImuSample &s,uint32_t now,bool eligible,bool healthy=true){
  if(!eligible || !healthy){reset();return false;}
  if(s.valid && uint32_t(now-s.at_ms)>MaxAgeMs){reset();return false;}
  if(!s.valid)return false;
  for(unsigned i=0;i<3;++i){
   if(!std::isfinite(s.acc_g[i]) || !std::isfinite(s.gyr_dps[i]) ||
      std::fabs(s.acc_g[i])>=AccSafetyG ||
      std::fabs(s.gyr_dps[i])>=v3::GyroSafetyDps){reset();return false;}
  }
  const float rate2=dot(s.gyr_dps,s.gyr_dps);
  if(rate2>MaxRotationDps*MaxRotationDps){reset();return false;}
  if(seen_ && s.at_ms==last_)return false;
  if(seen_ && uint32_t(s.at_ms-last_)>MaxGapMs){
   last_=s.at_ms;reset();return false;
  }
  const uint32_t dt=seen_ ? uint32_t(s.at_ms-last_) : 0;
  const uint32_t credit=dt<MaxCreditMs ? dt : MaxCreditMs;
  seen_=true;last_=s.at_ms;
  const float magnitudeDeparture=std::fabs(std::sqrt(dot(s.acc_g,s.acc_g))-1.f);
  const bool quiet=magnitudeDeparture<QuietToleranceG && rate2<QuietDps*QuietDps;
  // Quiet during cooldown does not prepay rearming. Continuous motion never
  // unlocks the fired latch, even after cooldown expires or reset() is called.
  if(fired_ && uint32_t(now-firedAt_)<CooldownMs){quietMs_=0;return false;}
  if(!armed_){
   if(!quiet){quietMs_=0;return false;}
   quietMs_+=credit;
   for(unsigned i=0;i<3;++i)gravity_[i]=s.acc_g[i];
   if(quietMs_>=QuietMs){armed_=true;fired_=false;}
   return false;
  }
  if(peaks_==0 && quiet){
   for(unsigned i=0;i<3;++i)gravity_[i]=s.acc_g[i];
   return false;
  }
  float p[3];for(unsigned i=0;i<3;++i)p[i]=s.acc_g[i]-gravity_[i];
  const float size=std::sqrt(dot(p,p));
  if(peaks_ && uint32_t(s.at_ms-lastPeak_)>MaxPeakGapMs)peaks_=0;
  // Fresh frames only, with capped time credit: dropped/NotReady intervals do
  // not turn one late frame into a completed burst. Smooth troughs are allowed.
  if(peaks_ && activeMs_<BurstMs)activeMs_+=credit;
  // Magnitude departure rejects pure changes in the gravity direction. Vector
  // opposition is independent of the resting orientation or a stuck flip origin.
  if(size<PeakG || magnitudeDeparture<MagnitudeDepartureG)return false;
  if(peaks_==0)activeMs_=0;
  if(peaks_==0 || dot(p,previous_)<=OppositeCos*size*std::sqrt(dot(previous_,previous_))){
   for(unsigned i=0;i<3;++i)previous_[i]=p[i];
   if(peaks_<RequiredPeaks)++peaks_; // Saturating, even during indefinite rattle.
   lastPeak_=s.at_ms;
   // Fire only on a genuinely fresh opposing peak, never held acceleration.
   if(!fired_ && peaks_>=RequiredPeaks && activeMs_>=BurstMs){
    reset();fired_=true;firedAt_=now;return true;
   }
  }
  return false;
 }
};
} // namespace v3app
