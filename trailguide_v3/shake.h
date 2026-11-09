#pragma once
#define S_MANUAL_API 1
#define S_MCU_CLOCK_API 1
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
 static constexpr float QuietDps=12.f, MaxRotationDps=v3::GyroSafetyDps, AccSafetyG=v3::AccSafetyG;
 static constexpr uint32_t BurstMs=900, QuietMs=400, CooldownMs=2200;
 static constexpr uint32_t MaxAgeMs=60, MaxGapMs=120, MaxPeakGapMs=240;
 static constexpr uint32_t MaxCreditMs=20; // Existing driver's poll interval.
 static constexpr unsigned RequiredPeaks=10;

 float previous_[3]={}, gravity_[3]={0,0,1};
 unsigned peaks_=0;
 uint32_t activeMs_=0,lastPeak_=0,firedAt_=0,quietMs_=0,last_=0;
 bool fired_=false,armed_=false,seen_=false;
 bool manual_=false,manualSeed_=false,manualPair_=false;
 float manualCandidate_[3]={};uint32_t manualAt_=0,manualCandidateAt_=0;
 float manualStrong_[3]={};uint32_t manualStrongAt_=0;
 static float dot(const float *a,const float *b){
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
 }
 public:
 // Cancel partial intent and require fresh eligible quiet acquisition. Preserves
 // freshness AND fired latch/cooldown: navigation/repaint must not unlock rattle.
 // Keep the same object alive across screens. This is NOT a factory reset.
 void reset(){peaks_=0;armed_=false;quietMs_=0;manual_=false;manualSeed_=false;manualPair_=false;}
 // Explicit one-shot intent, requested ONLY by a qualified released S tap.
 // Provisional eight-second wall-time bound; neither request nor expiry draws.
 void requestManual(uint32_t now){reset();manual_=true;manualSeed_=true;manualAt_=now;}
 bool manualArmed()const{return manual_;}

 // Call before legacy !sample.valid early returns. eligible is UI-owned;
 // healthy means sensor available/no KNOWN hard fault, NOT sample.valid.
 // Healthy valid=false (NotReady/noData) pauses without advancing any evidence.
 // Its retained timestamp/values are not a fresh frame. A subsequent fresh gap
 // over MaxGapMs disarms. Without separate fault information, an invalid sample
 // alone cannot distinguish NotReady from a single bus/clipping rejection.
 bool consume(const v3::ImuSample &s,uint32_t now,bool eligible,bool healthy=true){
  if(manual_ && uint32_t(now-manualAt_)>=8000)reset();
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
  // A real first moving frame is a candidate, not normalized "gravity".
  // A neutral-looking fresh frame may retain the direct seed path, but remains
  // provisional while a bounded opposed pair can still resolve ambiguity.
  if(manual_ && manualSeed_){
   if(int32_t(s.at_ms-manualAt_)<=0)return false;
   const float norm=std::sqrt(dot(s.acc_g,s.acc_g));
   for(unsigned i=0;i<3;++i)manualCandidate_[i]=s.acc_g[i];
   manualCandidateAt_=s.at_ms;manualPair_=true;manualSeed_=false;
   if(std::fabs(norm-1.f)<QuietToleranceG && rate2<QuietDps*QuietDps){
    for(unsigned i=0;i<3;++i)gravity_[i]=s.acc_g[i]/norm;
    armed_=true;fired_=false;
   }
   seen_=true;last_=s.at_ms;return false;
  }
  if(seen_ && uint32_t(s.at_ms-last_)>MaxGapMs){
   last_=s.at_ms;reset();return false;
  }
  const uint32_t dt=seen_ ? uint32_t(s.at_ms-last_) : 0;
  const uint32_t credit=dt<MaxCreditMs ? dt : MaxCreditMs;
  seen_=true;last_=s.at_ms;
  if(manual_ && manualPair_){
   if(uint32_t(s.at_ms-manualCandidateAt_)>=MaxPeakGapMs){
    // A candidate timeout expires evidence, not the explicit eight-second
    // intent. Start again from this fresh frame; no old peaks/time survive.
    peaks_=0;activeMs_=0;
    const float norm=std::sqrt(dot(s.acc_g,s.acc_g));
    for(unsigned i=0;i<3;++i)manualCandidate_[i]=s.acc_g[i];
    manualCandidateAt_=s.at_ms;
    armed_=std::fabs(norm-1.f)<QuietToleranceG && rate2<QuietDps*QuietDps;
    if(armed_){
     for(unsigned i=0;i<3;++i)gravity_[i]=s.acc_g[i]/norm;
     fired_=false;
    }
    return false;
   }else{
    float midpoint[3],span[3];
    for(unsigned i=0;i<3;++i){
     midpoint[i]=(manualCandidate_[i]+s.acc_g[i])*.5f;
     span[i]=s.acc_g[i]-manualCandidate_[i];
    }
    const float midpointNorm=std::sqrt(dot(midpoint,midpoint));
    const float departure=std::fabs(std::sqrt(dot(s.acc_g,s.acc_g))-1.f);
    const float candidateDeparture=std::fabs(std::sqrt(dot(manualCandidate_,manualCandidate_))-1.f);
    // Half the span must meet the existing peak threshold; centered residuals
    // are exactly opposed. Unit-magnitude direction changes alone cannot seed.
    if(dot(span,span)>=4.f*PeakG*PeakG &&
       std::fabs(midpointNorm-1.f)<QuietToleranceG &&
       (departure>=MagnitudeDepartureG || candidateDeparture>=MagnitudeDepartureG)){
     bool changed=false;
     for(unsigned i=0;i<3;++i){
      changed=changed || gravity_[i]!=midpoint[i];
      gravity_[i]=midpoint[i];
     }
     manualPair_=false;armed_=true;fired_=false;
     // Provisional neutral may have counted biased residuals: never combine
     // evidence from two different gravity estimates or back-credit the pair.
     if(changed){peaks_=0;activeMs_=0;}
     // Candidate is seed-only: never back-credit time or count it as a peak.
    }else if(departure>candidateDeparture){
     // The first moving sample need not be an extremum. Follow fresh stronger
     // magnitude evidence, keeping the opposed endpoint strictly <240ms old.
     for(unsigned i=0;i<3;++i)manualCandidate_[i]=s.acc_g[i];
     manualCandidateAt_=s.at_ms;
    }
   }
   if(!armed_)return false;
  }
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
  if(peaks_==0 && quiet && !manual_){
   for(unsigned i=0;i<3;++i)gravity_[i]=s.acc_g[i];
   return false;
  }
  float p[3];for(unsigned i=0;i<3;++i)p[i]=s.acc_g[i]-gravity_[i];
  const float size=std::sqrt(dot(p,p));
  // Manual weak peaks can bridge only a live accepted strong opposed peak.
  // Neither weak turns nor held strong acceleration refresh that proof. Expiry
  // discards the entire partial burst BEFORE this frame can earn time credit.
  if(manual_ && peaks_ && uint32_t(s.at_ms-manualStrongAt_)>=MaxPeakGapMs){
   peaks_=0;activeMs_=0;
  }
  if(peaks_ && uint32_t(s.at_ms-lastPeak_)>MaxPeakGapMs){
   peaks_=0;if(manual_)activeMs_=0;
  }
  // Fresh frames only, with capped time credit: dropped/NotReady intervals do
  // not turn one late frame into a completed burst. Smooth troughs are allowed.
  if(peaks_ && activeMs_<BurstMs)activeMs_+=credit;
  // Automatic policy is unchanged. Manual-only weak magnitude needs bounded
  // opposing evidence against the accepted strong vector, not an episode latch.
  const bool strong=magnitudeDeparture>=MagnitudeDepartureG;
  if(size<PeakG)return false;
  if(!strong && (!manual_ || !peaks_ ||
     dot(p,manualStrong_)>OppositeCos*size*std::sqrt(dot(manualStrong_,manualStrong_))))return false;
  if(peaks_==0)activeMs_=0;
  if(peaks_==0 || dot(p,previous_)<=OppositeCos*size*std::sqrt(dot(previous_,previous_))){
   if(manual_){
    if(strong){
     manualStrongAt_=s.at_ms;
     for(unsigned i=0;i<3;++i)manualStrong_[i]=p[i];
    }
   }
   for(unsigned i=0;i<3;++i)previous_[i]=p[i];
   if(peaks_<RequiredPeaks)++peaks_; // Saturating, even during indefinite rattle.
   lastPeak_=s.at_ms;
   // Manual completion additionally requires a fresh accepted strong peak.
   if(!fired_ && (!manual_ || strong) && peaks_>=RequiredPeaks && activeMs_>=BurstMs){
    reset();fired_=true;firedAt_=now;return true;
   }
  }
  return false;
 }
};
} // namespace v3app
