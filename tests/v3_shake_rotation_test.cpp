// Generic synthetic motion only: no captured participant samples or timing.
#include "../trailguide_v3/shake.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <initializer_list>
struct Motion {
 v3app::Shake detector;
 uint32_t now=0;
 unsigned events=0;
 void frame(float x=0,float y=0,float z=1,float gx=0,float gy=0,float gz=0){
  now+=20;v3::ImuSample s;s.valid=true;s.at_ms=now;
  s.acc_g[0]=x;s.acc_g[1]=y;s.acc_g[2]=z;
  s.gyr_dps[0]=gx;s.gyr_dps[1]=gy;s.gyr_dps[2]=gz;
  events+=detector.consume(s,now,true);
 }
 void rest(unsigned n=30){for(unsigned i=0;i<n;++i)frame();}
 void mixedShake(unsigned n=80){
  for(unsigned i=0;i<n;++i){
   const float sign=(i/5)%2?-1.f:1.f;
   const float rate=200.f+100.f*float(i%6);
   frame(sign*1.4f,0,1,sign*rate*.6f,rate*.8f,0);
  }
 }
};
static void check(bool ok,const char *name){
 if(!ok){std::fprintf(stderr,"FAIL: %s\n",name);std::exit(1);}
 std::printf("PASS: %s\n",name);
}
int main(){
 Motion mixed;mixed.rest();mixed.mixedShake();
 check(mixed.events==1,"sustained translation with synthetic mixed rotation 200-700 dps fires exactly once");
 mixed.mixedShake(10000);mixed.detector.reset();mixed.mixedShake(500);
 check(mixed.events==1,"mixed rotation continuous rattle and reset never rearm");
 mixed.rest(220);mixed.mixedShake();
 check(mixed.events==2,"mixed rotation permits a second burst only after cooldown and fresh quiet");
 Motion gravity;gravity.rest();gravity.frame(1.4f);
 for(unsigned i=0;i<400;++i){
  const float angle=float(i)*.2f;
  gravity.frame(std::sin(angle),0,std::cos(angle),0,572.9578f,0);
 }
 check(gravity.events==0,"high-rate pure gravity rotation cannot draw even after a translation impulse");
 Motion shortBurst;shortBurst.rest();shortBurst.mixedShake(20);
 check(shortBurst.events==0,"short mixed-rotation jiggle cannot draw");
 Motion impulse;impulse.rest();impulse.frame(2,0,1,300,400,0);impulse.rest(300);
 check(impulse.events==0,"single mixed-rotation impulse cannot draw");
 Motion norm;norm.rest();norm.mixedShake(35);
 // Each component remains below its rail, but the vector exceeds the safety norm.
 norm.frame(1.4f,0,1,1500,1500,0);norm.mixedShake();
 check(norm.events==0,"over-bound vector norm cancels intent despite individually safe axes");
 Motion edge;edge.rest();
 for(unsigned i=0;i<80;++i)edge.frame((i/5)%2?-1.4f:1.4f,0,1,v3::GyroSafetyDps-1.f);
 check(edge.events==1,"translation just below supported rotation bound remains admissible");
 for(unsigned axis=0;axis<3;++axis){
  for(float sign:{-1.f,1.f}){
   Motion rail;rail.rest();rail.mixedShake(35);
   v3::ImuSample s;s.valid=true;s.at_ms=rail.now+=20;s.acc_g[2]=1;
   s.gyr_dps[axis]=sign*v3::GyroSafetyDps;
   rail.events+=rail.detector.consume(s,rail.now,true);rail.mixedShake();
   check(rail.events==0,"either gyro rail on every axis cancels mixed-rotation intent");
  }
  for(unsigned sensor=0;sensor<2;++sensor){
   for(float bad:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
    Motion nonfinite;nonfinite.rest();nonfinite.mixedShake(35);
    v3::ImuSample s;s.valid=true;s.at_ms=nonfinite.now+=20;s.acc_g[2]=1;
    (sensor?s.gyr_dps:s.acc_g)[axis]=bad;
    nonfinite.events+=nonfinite.detector.consume(s,nonfinite.now,true);nonfinite.mixedShake();
    check(nonfinite.events==0,"nonfinite acceleration or gyro on every axis cancels mixed-rotation intent");
   }
  }
 }
}
