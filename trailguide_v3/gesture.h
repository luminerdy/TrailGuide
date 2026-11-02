#pragma once
#include "imu.h"
#include <cmath>
namespace v3app {
// EXPERIMENTAL hypotheses, NOT hardware tuned/accepted. Sensor-native axes.
// >=3 separated |magnitude-1g| peaks >0.75g in <=1200ms; rotation <45dps.
// Quiet <=0.16g error and gyro <8dps for 500ms (allows observed ~2.2 bias).
// Arm only after 600ms rest; 2500ms cooldown AND rest after firing.
class Gesture {
 bool seen=false, quiet=false, armed=false, high=false, candidate=false;
 uint32_t last=0, quietStart=0, firstPeak=0, firedAt=0;unsigned peaks=0;bool cooling=false;
 void drop(){armed=false;high=false;candidate=false;peaks=0;quiet=false;}
 public:
 void reset(){drop();seen=false;} // preserve cooldown across context changes
 bool consume(const v3::ImuSample &s,uint32_t now,bool eligible) {
  if(!eligible){drop();seen=true;last=s.at_ms;return false;}
  if(!s.valid || uint32_t(now-s.at_ms)>60){drop();return false;}
  if(seen && s.at_ms==last)return false;
  if(seen && uint32_t(s.at_ms-last)>80)drop();
  seen=true;last=s.at_ms;
  float a2=0,g2=0;for(unsigned i=0;i<3;i++){
   if(!std::isfinite(s.acc_g[i])||!std::isfinite(s.gyr_dps[i])||std::fabs(s.acc_g[i])>=3.99f||std::fabs(s.gyr_dps[i])>=511.f){drop();return false;}
   a2+=s.acc_g[i]*s.acc_g[i];g2+=s.gyr_dps[i]*s.gyr_dps[i];
  }
  const float delta=std::fabs(std::sqrt(a2)-1.f), gyro=std::sqrt(g2);
  if(gyro>45.f){drop();return false;}
  const bool rest=delta<.16f && gyro<8.f;
  if(rest){if(!quiet){quiet=true;quietStart=now;}}
  else quiet=false;
  if(cooling){if(uint32_t(now-firedAt)<2500)return false;cooling=false;drop();return false;}
  if(candidate && uint32_t(now-firstPeak)>1800){drop();return false;}
  if(!armed){if(quiet && uint32_t(now-quietStart)>=600)armed=true;return false;}
  if(delta>.75f && !high){
   if(!candidate){candidate=true;firstPeak=now;peaks=0;}
   if(uint32_t(now-firstPeak)<=1200)++peaks;
   high=true;
  }else if(delta<.3f)high=false;
  if(candidate && peaks>=3 && quiet && uint32_t(now-quietStart)>=500){firedAt=now;cooling=true;drop();return true;}
  return false;
 }
};
}
