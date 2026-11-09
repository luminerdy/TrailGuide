#include "../trailguide_v3/shake.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <type_traits>
static_assert(std::is_trivially_copyable<v3app::Shake>::value,"state-only detector");
static_assert(sizeof(v3app::Shake)<=128,"bounded detector state");
static void check(bool ok,const char *name){if(!ok){std::fprintf(stderr,"FAIL: %s\n",name);std::exit(1);}std::printf("PASS: %s\n",name);}
struct Trace {
 v3app::Shake d; uint32_t t=0; int events=0;
 void sample(float x,float y=0,float z=1,bool valid=true,bool eligible=true,bool healthy=true,float gyro=0,uint32_t dt=20){
  t+=dt;v3::ImuSample s;s.valid=valid;s.at_ms=t;s.acc_g[0]=x;s.acc_g[1]=y;s.acc_g[2]=z;s.gyr_dps[0]=gyro;
  events+=d.consume(s,t,eligible,healthy);
 }
 void rest(unsigned n=30){for(unsigned i=0;i<n;++i)sample(0);}
 void shake(unsigned n=70){for(unsigned i=0;i<n;++i)sample((i/5)%2? -1.4f:1.4f);}
};
int main(){
 std::printf("STATE_BYTES: %zu\n",sizeof(v3app::Shake));
 Trace midFault;midFault.rest();midFault.shake(35);midFault.sample(0,0,1,false,true,false);midFault.shake(100);check(midFault.events==0,"known hard fault discards old partial intent");
 Trace context;context.rest();context.shake(35);context.sample(0,0,1,true,false);context.shake(100);check(context.events==0,"context return cannot resume cancelled partial intent");
 Trace recovered;recovered.rest();recovered.shake();recovered.d.reset();recovered.rest(220);recovered.shake();check(recovered.events==2,"reset preserves cooldown but allows later fresh quiet rearm");
 Trace backwards;backwards.rest();backwards.shake(35);backwards.t-=300;backwards.shake(100);check(backwards.events==0,"backward nonrollover sample time disarms");
 Trace duration;duration.rest();duration.shake(44);check(duration.events==0,"880ms burst is below sustained threshold");duration.shake(15);check(duration.events==1,"roughly one second deliberate burst can trigger");
 Trace negative;for(unsigned i=0;i<30;++i){negative.sample(0,0,-1);}
 for(unsigned i=0;i<70;++i){negative.sample(0,(i/5)%2?-1.4f:1.4f,-1);}
 check(negative.events==1,"inverted resting orientation also accepts shake alone");
 Trace sameAfter; sameAfter.rest();sameAfter.shake(35);v3::ImuSample saved;saved.valid=true;saved.at_ms=sameAfter.t;saved.acc_g[0]=1.4f;saved.acc_g[2]=1;
 for(unsigned i=0;i<500;++i){sameAfter.events+=sameAfter.d.consume(saved,sameAfter.t,true);}
 check(sameAfter.events==0,"repeated same-snapshot polling cannot pay duration");
 Trace actualPoll;actualPoll.rest();for(unsigned i=0;i<100;++i){actualPoll.t+=20;v3::ImuSample s;s.valid=i%4!=1;s.at_ms=s.valid?actualPoll.t:actualPoll.t-20;s.acc_g[0]=(i/5)%2?-1.4f:1.4f;s.acc_g[2]=1;actualPoll.events+=actualPoll.d.consume(s,actualPoll.t,true);}
 check(actualPoll.events==1,"actual 20ms polling with intermittent retained-timestamp NotReady passes");
 Trace high;high.rest();for(unsigned i=0;i<300;++i){high.sample(1.8f);}
 check(high.events==0,"held high acceleration is not shake");
 Trace impulse;impulse.rest();impulse.sample(2);impulse.rest(300);check(impulse.events==0,"single impulse cannot qualify");
 Trace flip;flip.rest();for(unsigned i=0;i<50;++i){float r=float(i)*3.14159265f/49;flip.sample(std::sin(r),0,std::cos(r),true,true,true,90);}
 for(unsigned i=0;i<50;++i){float r=3.14159265f-float(i)*3.14159265f/49;flip.sample(std::sin(r),0,std::cos(r),true,true,true,90);}
 flip.rest();check(flip.events==0,"ordinary turnover and return cannot qualify");
 Trace gyroClip;gyroClip.rest();gyroClip.shake(35);gyroClip.sample(0,0,1,true,true,true,v3::GyroSafetyDps);gyroClip.shake(80);check(gyroClip.events==0,"gyro clipping requires quiet recovery");
 Trace inf;inf.rest();inf.shake(35);inf.sample(0,0,1,true,true,true,std::numeric_limits<float>::infinity());inf.shake(80);check(inf.events==0,"infinite gyro requires quiet recovery");
 Trace noData;noData.rest();noData.shake(30);for(unsigned i=0;i<20;++i){noData.sample(0,0,1,false);}
 noData.shake(80);check(noData.events==0,"extended noData requires quiet recovery");
 Trace wrapQuiet;wrapQuiet.t=UINT32_MAX-300;wrapQuiet.rest(80);check(wrapQuiet.events==0,"quiet timestamp rollover cannot synthesize event");
 Trace unquiet;unquiet.shake(100);check(unquiet.events==0,"power-on motion cannot replace quiet acquisition");
 Trace longBurst;longBurst.rest();longBurst.shake(10000);check(longBurst.events==1,"long continuous rattle stays one event");
 Trace shallow;shallow.rest();for(unsigned i=0;i<70;++i)shallow.sample((i/5)%2?-1.1f:1.1f,1.f);
 check(shallow.events==0,"weak obtuse zigzags are not deliberate alternating translation");
 Trace heldFinish;heldFinish.rest();for(unsigned i=0;i<36;++i)heldFinish.sample((i/2)%2?-1.4f:1.4f);
 for(unsigned i=0;i<11;++i){heldFinish.sample(-1.4f);}
 check(heldFinish.events==0,"held acceleration cannot complete a short shake");
 Trace interleave;interleave.rest();for(unsigned i=0;i<90;++i){interleave.sample((i/5)%2?-1.4f:1.4f,0,1,true,true,true,0,(i%3==0)?17:20);
 v3::ImuSample old;old.valid=false;old.at_ms=interleave.t;interleave.events+=interleave.d.consume(old,interleave.t+4,true);}
 check(interleave.events==1,"60Hz style valid samples interleaved NotReady retain intent");
 Trace wrap;wrap.t=UINT32_MAX-1000;wrap.rest();wrap.shake();check(wrap.events==1,"timestamp rollover accepts coherent fresh sequence without false event");
 Trace continuous;continuous.rest();continuous.shake();for(unsigned i=0;i<30;++i){continuous.d.reset();continuous.shake(20);}
 check(continuous.events==1,"continuous shaking and resets cannot unlock fired latch");
 Trace early;early.rest();early.shake();early.rest(25);early.shake(160);check(early.events==1,"quiet only during cooldown cannot rearm");
 Trace rearm;rearm.rest();rearm.shake();rearm.rest(200);rearm.shake();check(rearm.events==2,"quiet after cooldown permits second deliberate burst");
 Trace cancelled;cancelled.rest();cancelled.shake(35);cancelled.d.reset();cancelled.shake(90);
 check(cancelled.events==0,"cancellation requires quiet before fresh intent");
 Trace fastRotation;fastRotation.rest();for(unsigned i=0;i<70;++i)fastRotation.sample((i/5)%2?-1.4f:1.4f,0,1,true,true,true,v3::GyroSafetyDps+1.f);
 check(fastRotation.events==0,"rotation beyond supported physical bound cannot build intent");
 Trace rotation;rotation.rest();rotation.sample(1.4f);for(unsigned i=0;i<100;++i){float angle=.8f*std::sin(float(i)*.62831853f);rotation.sample(std::sin(angle),0,std::cos(angle));}
 check(rotation.events==0,"gravity direction changes alone are not translation");
 Trace sine;sine.rest();for(unsigned i=0;i<75;++i)sine.sample(1.4f*std::sin(float(i)*.62831853f));
 check(sine.events==1,"smooth shake troughs do not cancel coherent peaks");
 Trace interrupted;interrupted.rest();for(unsigned i=0;i<10;++i){interrupted.shake(10);interrupted.rest(30);}
 check(interrupted.events==0,"separate short jiggles do not aggregate across quiet");
 Trace pause;pause.rest();for(unsigned i=0;i<35;++i)pause.sample((i/2)%2?-1.4f:1.4f);
 pause.sample(0,0,1,false,true,true,0,100);for(unsigned i=0;i<8;++i)pause.sample((i/2)%2?-1.4f:1.4f);
 check(pause.events==0,"NotReady elapsed time does not pay burst duration");
 Trace stale;stale.rest();for(unsigned i=0;i<70;++i){v3::ImuSample s;s.valid=true;s.at_ms=stale.t+20;stale.t+=20;s.acc_g[0]=(i/5)%2?-1.4f:1.4f;s.acc_g[2]=1;stale.events+=stale.d.consume(s,stale.t+61,true);}
 check(stale.events==0,"aged valid samples cannot qualify");
 Trace nan;nan.rest();nan.shake(35);nan.sample(std::numeric_limits<float>::quiet_NaN());nan.shake(35);check(nan.events==0,"nonfinite axis cancels partial intent");
 Trace clipped;clipped.rest();clipped.shake(35);clipped.sample(15.96f);clipped.shake(35);check(clipped.events==0,"clipped acceleration cancels partial intent");
 Trace unhealthy;unhealthy.rest();for(unsigned i=0;i<70;++i)unhealthy.sample((i/5)%2?-1.4f:1.4f,0,1,true,true,false);
 check(unhealthy.events==0,"hard unhealthy input cannot trigger");
 Trace suppressed;suppressed.rest();for(unsigned i=0;i<70;++i)suppressed.sample((i/5)%2?-1.4f:1.4f,0,1,true,false);
 check(suppressed.events==0,"ineligible context cannot trigger");
 Trace gap;gap.rest();gap.shake(35);gap.t+=500;gap.shake(35);check(gap.events==0,"large fresh sample gap cancels partial burst");
 Trace dup;dup.rest();for(unsigned i=0;i<70;++i){dup.t=600;dup.sample((i/5)%2?-1.4f:1.4f);}
 dup.t=1600;dup.sample(-1.4f);check(dup.events==0,"duplicate timestamps cannot build pulses");
 Trace invalid;invalid.rest();for(unsigned i=0;i<70;++i)invalid.sample((i/5)%2?-1.4f:1.4f,0,1,false);
 check(invalid.events==0,"invalid samples cannot build intent");
 Trace side;for(unsigned i=0;i<30;++i)side.sample(1,0,0);
 for(unsigned i=0;i<70;++i)side.sample(1,(i/5)%2?-1.4f:1.4f,0);
 check(side.events==1,"sideways resting orientation needs no flip reference");
 Trace a;a.rest();a.shake();check(a.events==1,"sustained alternating translation fires once");
 Trace shortBurst;shortBurst.rest();for(unsigned i=0;i<20;++i)shortBurst.sample(i%2?1.4f:-1.4f);
 check(shortBurst.events==0,"short high frequency jiggle cannot qualify");
}
