// SYNTHETIC unit-gravity vectors, never measured traces or physical acceptance.
// Compile this actual production header with ASan/UBSan; no detector stand-in.
#include <cassert>
#include <cstdio>
#include <cmath>
#include <limits>
#include <initializer_list>
#include "../trailguide_v3/gesture.h"
struct Vec { float x,y,z; };
static Vec scaled(Vec v,float k){return {v.x*k,v.y*k,v.z*k};}
struct Trace {
 v3app::Gesture d;
 uint32_t t=0, step=20;
 unsigned fires=0;
 Vec up={0,0,1}, side={1,0,0};
 v3::ImuSample sample(Vec a,float gyro=2.2f) const {
  v3::ImuSample s;s.valid=true;s.at_ms=t;
  s.acc_g[0]=a.x;s.acc_g[1]=a.y;s.acc_g[2]=a.z;s.gyr_dps[0]=gyro;return s;
 }
 bool send(v3::ImuSample s,uint32_t age=0,bool eligible=true){
  const bool fired=d.consume(s,t+age,eligible);fires+=fired;return fired;
 }
 bool tick(Vec a,float gyro=2.2f,bool eligible=true){t+=step;return send(sample(a,gyro),0,eligible);}
 void pose(Vec a,unsigned ms,float gyro=2.2f,bool eligible=true){
  for(unsigned elapsed=0;elapsed<ms;elapsed+=step)tick(a,gyro,eligible);
 }
 void rest(unsigned ms=900){pose(up,ms);}
 void down(unsigned ms=500){pose(scaled(up,-1),ms);}
 void arc(bool back,unsigned ms=600,float gyro=90.f){
  const unsigned n=ms/step;
  for(unsigned i=1;i<=n;++i){
   const float angle=3.14159265359f*(back ? 1.f-float(i)/n : float(i)/n);
   const float c=std::cos(angle),s=std::sin(angle);
   tick({up.x*c+side.x*s,up.y*c+side.y*s,up.z*c+side.z*s},gyro);
  }
 }
 void cycle(unsigned turnMs=600){arc(false,turnMs);down();arc(true,turnMs);rest(500);}
 void expect(unsigned n)const{if(fires!=n)std::fprintf(stderr,"fires=%u expected=%u at=%u cadence=%u\n",fires,n,t,step);assert(fires==n);}
};
static void heldCycle(){
 Trace f;f.rest();f.down();f.expect(0);f.rest(500);f.expect(1);
 f.down(5000);f.expect(1); // No draw from holding down, including cooldown and later recovery.
 f.rest(900);f.expect(1);f.cycle();f.expect(2);
}
static void axesAndCadences(){
 const Vec up[]={{0,0,1},{0,0,-1},{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{.6f,0,.8f}};
 const Vec side[]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},{.8f,0,-.6f}};
 for(unsigned i=0;i<7;++i)for(uint32_t cadence:{20u,40u,100u}){
  Trace f;f.up=up[i];f.side=side[i];f.step=cadence;f.rest();f.cycle();f.expect(1);
 }
 Trace jitter;for(unsigned i=0;i<150;++i){jitter.step=(i%3==0?100: (i%3==1?40:20));jitter.tick(jitter.up);}
 jitter.cycle();jitter.expect(1);
 Trace slow;slow.rest();slow.cycle(3000);slow.expect(1);
}
static void rejectsOrdinaryHandling(){
 Trace f;f.rest();f.down(15000);f.expect(0);f.rest();f.expect(0); // Never-return timeout.
 f.rest(1500);f.cycle();f.expect(1);
 Trace tilt;tilt.rest();tilt.pose({.8f,0,.6f},2000);tilt.rest();tilt.expect(0);
 // Heading-only gyro is not gravity inversion, even for a slow steady rotation.
 tilt.pose(tilt.up,4000,5);tilt.pose(tilt.up,1000,150);tilt.rest();tilt.expect(0);
 for(unsigned i=0;i<20;++i){tilt.pose(scaled(tilt.up,2.1f),60);tilt.rest(100);}
 tilt.rest();tilt.expect(0); // Old multi-peak shake must no longer draw.
 Trace brief;brief.rest();brief.down(100);brief.rest();brief.expect(0);
 Trace partial;partial.rest();partial.pose({.7071068f,0,-.7071068f},700);partial.rest();partial.expect(0);
 Trace shortReturn;shortReturn.rest();shortReturn.down();shortReturn.rest(100);shortReturn.down(700);shortReturn.expect(0);
 shortReturn.rest(500);shortReturn.expect(1);
 Trace noRest;noRest.pose(noRest.up,1000,30);noRest.cycle();noRest.expect(0);
 Trace bias;bias.pose(bias.up,900,10);bias.down();bias.rest(500);bias.expect(1);
 Trace dynamic;dynamic.rest();dynamic.pose({2,0,0},300,300);dynamic.down();
 dynamic.pose({-2,0,0},300,300);dynamic.rest(500);dynamic.expect(1);
 Trace movingHold;movingHold.rest();movingHold.arc(false);movingHold.pose(scaled(movingHold.up,-1),900,90);
 movingHold.arc(true);movingHold.rest();movingHold.expect(0);
 // Fresh steady direction, not magnitude alone, is required to acquire origin.
 Trace rotating;for(unsigned i=0;i<40;++i)rotating.tick(i%2?rotating.up:rotating.side);
 rotating.down();rotating.rest(500);rotating.expect(0);
}
static void cancelAndRearm(){
 for(unsigned phase=0;phase<4;++phase)for(bool reset:{false,true}){
  Trace f;f.rest();
  if(phase>=1)f.arc(false);
  if(phase>=2)f.down();
  if(phase>=3){f.arc(true);f.rest(60);}
  if(reset)f.d.reset();else f.tick(scaled(f.up,-1),2.2f,false);
  f.down(1800);f.rest();f.expect(0); // Recovery may rebase; returning once is not a new complete cycle.
  f.cycle();f.expect(1);
 }
 Trace f;f.rest();f.cycle();f.expect(1);f.d.reset();f.rest();f.cycle();f.expect(1);
 f.rest(3000);f.cycle();f.expect(2); // Cooldown survives reset; requires fresh rest.
 Trace rebased;rebased.rest();rebased.pose(rebased.side,15000);rebased.expect(0);
 rebased.up={1,0,0};rebased.side={0,0,1};rebased.rest(1500);rebased.cycle();rebased.expect(1);
}
static void badFrames(){
 for(unsigned kind=0;kind<12;++kind){
  Trace f;f.rest();f.arc(false);f.down();f.t+=f.step;
  auto s=f.sample(scaled(f.up,-1));uint32_t age=0;
  switch(kind){
   case 0:s.valid=false;break;
   case 1:s.acc_g[0]=std::numeric_limits<float>::quiet_NaN();break;
   case 2:s.gyr_dps[2]=std::numeric_limits<float>::infinity();break;
   case 3:s.acc_g[1]=15.96f;break;
   case 4:s.gyr_dps[1]=-2044.f;break;
   case 5:age=61;break;
   case 6:s.at_ms-=f.step;break; // duplicate is cancellation, not a free extra hold
   case 7:s.at_ms-=1000;break; // backwards/out of order
   case 8:f.t+=400;s.at_ms=f.t;break; // missing-frame gap
   case 9:s.acc_g[2]=std::numeric_limits<float>::infinity();break;
   case 10:s.gyr_dps[0]=std::numeric_limits<float>::quiet_NaN();break;
   case 11:s.at_ms=f.t+1;break; // future frame
  }
  assert(!f.send(s,age));f.rest();f.expect(0);f.cycle();f.expect(1);
 }
 Trace jump;jump.rest();jump.down(20);jump.t+=200;jump.tick(scaled(jump.up,-1));
 jump.rest(500);jump.expect(0); // Elapsed time alone is not enough: >=4 fresh samples.
 Trace duplicate;duplicate.rest();duplicate.down(20);
 auto same=duplicate.sample(scaled(duplicate.up,-1));
 for(unsigned i=0;i<100;++i)assert(!duplicate.send(same));
 duplicate.rest();duplicate.expect(0);
 Trace gap;gap.rest();gap.arc(false);gap.down();gap.t+=400;gap.rest();gap.expect(0);
 Trace wrap;wrap.t=0xfffffc00u;wrap.rest();wrap.cycle();wrap.expect(1);
 wrap.d.reset();wrap.rest(4000);wrap.cycle();wrap.expect(2);
}
// SYNTHETIC regression for the observed Recover lock, NOT recorded user data.
// Rounded near-opposed gravity with bounded bias; raw capture remains private.
static void opposedRecover(){
 Trace f;f.rest();f.d.reset();const Vec old=f.up;
 f.up={.025f,.05f,-.99f};
 f.rest(1200);f.expect(0);
 auto d=f.d.diagnostic(f.sample(f.up),f.t);
 assert(d.phase==4 && d.origin[2]==old.z); // Reference retained until qualified.
 f.tick(f.up);f.expect(0);d=f.d.diagnostic(f.sample(f.up),f.t);
 assert(d.phase==1 && !d.dwelling && d.origin[2]<-.98f);
 f.cycle();f.expect(1);
}
static void recoveryGuards(){
 // Rebase alone, old-origin return alone and partial return never draw.
 Trace f;f.rest();f.down();f.d.reset();f.down(1400);f.expect(0);
 assert(f.d.diagnostic(f.sample(scaled(f.up,-1)),f.t).phase==1);
 f.rest(500);f.expect(0);f.pose({.8f,0,-.6f},700);f.expect(0);
 f.down(500);f.expect(1); // Only the whole opposition-return from NEW origin.
 // Single-frame/tap-style cancellation cannot preserve active-cycle dwell.
 Trace tap;tap.rest();tap.down();tap.d.reset();tap.tick(scaled(tap.up,-1));
 tap.d.reset();tap.rest();tap.expect(0);tap.cycle();tap.expect(1);
 // Neither long inversion in an active cycle nor timed-out held cancellation draws.
 Trace held;held.rest();held.down(18000);held.expect(0);
 held.rest(500);held.expect(0);held.down(500);held.expect(1);
 // Existing cooldown remains 2500ms, and no baseline accrues within it.
 Trace cool;cool.rest();cool.down(500);
 bool coolFired=false;unsigned coolSample=0;
 for(;coolSample<300 && !coolFired;++coolSample)coolFired=cool.tick(cool.up);
 if(!coolFired){
  const auto exhausted=cool.d.diagnostic(cool.sample(cool.up),cool.t);
  std::fprintf(stderr,"expected cooldown trigger exhausted seed=deterministic sample=%u t=%u phase=%u cooling=%u dwelling=%u\n",coolSample,unsigned(cool.t),unsigned(exhausted.phase),unsigned(exhausted.cooling),unsigned(exhausted.dwelling));
  assert(coolFired);
 }
 const auto fired=cool.t;cool.d.reset();
 while(cool.t-fired<2480)cool.tick(scaled(cool.up,-1));
 auto d=cool.d.diagnostic(cool.sample(scaled(cool.up,-1)),cool.t);
 assert(d.cooling && !d.dwelling && d.origin[2]==1);
 cool.tick(scaled(cool.up,-1)); // Exactly +2500: START fresh 1200ms recovery.
 cool.down(1180);cool.expect(1);
 assert(cool.d.diagnostic(cool.sample(scaled(cool.up,-1)),cool.t).phase==4);
 cool.tick(scaled(cool.up,-1));cool.expect(1);
 assert(cool.d.diagnostic(cool.sample(scaled(cool.up,-1)),cool.t).phase==1);
 // Interrupted baselines cannot accumulate. Each bad-frame kind splits a hold.
 for(unsigned kind=0;kind<9;++kind){
  Trace b;b.rest();b.d.reset();
  for(unsigned n=0;n<8;++n){
   b.down(700);b.t+=b.step;auto s=b.sample(scaled(b.up,-1));unsigned age=0;bool eligible=true;
   switch(kind){
    case 0:s.valid=false;break;
    case 1:age=61;break;
    case 2:s.at_ms-=b.step;break;
    case 3:eligible=false;break;
    case 4:s.acc_g[2]=-1.2f;break;
    case 5:s.gyr_dps[0]=12;break;
    case 6:s.acc_g[0]=std::numeric_limits<float>::quiet_NaN();break;
    case 7:b.t+=251;s.at_ms=b.t;break;
    case 8:s.acc_g[0]=.8f;s.acc_g[2]=-.6f;break;
   }
   assert(!b.send(s,age,eligible));
   assert(b.d.diagnostic(s,b.t).origin[2]==1);
  }
  b.down(700);b.expect(0);assert(b.d.diagnostic(b.sample(scaled(b.up,-1)),b.t).phase==4);
  b.down(540);b.expect(0);assert(b.d.diagnostic(b.sample(scaled(b.up,-1)),b.t).phase==1);
  b.up=scaled(b.up,-1);b.cycle();b.expect(1);
 }
 // Repeated pose jitter with valid 1g/low gyro cannot acquire a reference.
 Trace jitter;jitter.rest();jitter.d.reset();
 for(unsigned n=0;n<200;++n)jitter.tick(n%2?Vec{.3f,0,-.9539392f}:Vec{-.3f,0,-.9539392f});
 jitter.expect(0);auto j=jitter.d.diagnostic(jitter.sample(jitter.up),jitter.t);
 assert(j.phase==4 && j.origin[2]==1 && j.dwellSamples==1);
}
static void rangeSafetyAllAxes(){
 static_assert(v3::GyroFullScaleDps==2048.f && v3::GyroSafetyDps==2044.f);
 for(unsigned axis=0;axis<3;++axis)for(float value:{-2048.f,-2044.f,-2043.9375f,-600.f,600.f,2043.9375f,2044.f,2048.f,
   std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){
  Trace f;f.rest();f.t+=20;auto s=f.sample(f.up,0);s.gyr_dps[axis]=value;
  const bool reject=!std::isfinite(value) || std::fabs(value)>=2044.f;
  assert(bool(f.d.diagnostic(s,f.t).reject&16)==reject);
  assert(!f.send(s));const auto d=f.d.diagnostic(s,f.t);
  assert(reject ? d.phase==4 && d.resetReason==6 : d.phase==1);
 }
 for(unsigned axis=0;axis<3;++axis)for(float value:{-15.96f,15.96f,std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){
  Trace f;f.rest();f.t+=20;auto s=f.sample(f.up);s.acc_g[axis]=value;
  assert(f.d.diagnostic(s,f.t).reject&16);assert(!f.send(s));
  assert(f.d.diagnostic(s,f.t).resetReason==6);
 }
}
int main(){
 rangeSafetyAllAxes();
 // Explicitly SYNTHETIC 600dps whole cycle, not recovered clipped human peaks.
 {Trace wide;wide.rest();wide.arc(false,300,600);wide.down();
  wide.arc(true,300,-600);wide.rest(500);wide.expect(1);wide.rest(500);wide.expect(1);}
 recoveryGuards();
 opposedRecover();
 heldCycle();axesAndCadences();rejectsOrdinaryHandling();cancelAndRearm();badFrames();
#ifdef V3_GESTURE_RESET_DIAGNOSTICS
 {v3app::Gesture d;v3::ImuSample s;s.valid=true;s.acc_g[2]=1;s.at_ms=20;
  d.consume(s,20,false);assert(d.diagnostic(s,20).resetReason==1);
  s.valid=false;d.consume(s,20,true);assert(d.diagnostic(s,20).resetReason==2);
  s.valid=true;d.consume(s,100,true);assert(d.diagnostic(s,100).resetReason==3);
  s.at_ms=40;s.gyr_dps[0]=2048;d.consume(s,40,true);assert(d.diagnostic(s,40).resetReason==6);
 }
#else
 assert(!"actual detector reset cause metadata missing");
#endif
 puts("PASS gesture: SYNTHETIC held flip/return, axes/sign/cadence/slow turns, handling, cancellation/rearm/cooldown, invalid/freshness/gaps/rollover; NOT PHYSICAL ACCEPTANCE");
}
