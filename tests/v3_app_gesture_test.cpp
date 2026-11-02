// Deliberately SYNTHETIC host traces. NOT physical samples or tuned thresholds.
#include <cassert>
#include <cstdio>
#include <cmath>
#include "../trailguide_v3/gesture.h"
static v3::ImuSample sample(uint32_t t,float a=1,float g=2.2f) {v3::ImuSample s;s.valid=true;s.at_ms=t;s.acc_g[2]=a;s.gyr_dps[0]=g;return s;}
static bool feed(v3app::Gesture &d,uint32_t &t,float a,float g=2.2f,bool eligible=true){t+=20;return d.consume(sample(t,a,g),t,eligible);}
static void rest(v3app::Gesture &d,uint32_t &t,int n=40){for(int i=0;i<n;i++)assert(!feed(d,t,1));}
static bool peaks(v3app::Gesture &d,uint32_t &t){for(int i=0;i<3;i++){for(int j=0;j<3;j++)assert(!feed(d,t,2.1f));for(int j=0;j<5;j++)assert(!feed(d,t,1));}bool fired=false;for(int i=0;i<30;i++){if(feed(d,t,1)){assert(!fired);fired=true;}}return fired;}
int main(){
 uint32_t t=0;v3app::Gesture d;rest(d,t);assert(peaks(d,t));assert(!peaks(d,t));rest(d,t,140);assert(peaks(d,t));
 d.reset();rest(d,t);for(int i=0;i<5;i++)assert(!feed(d,t,2.1f));rest(d,t,60); // single bump
 for(int i=0;i<100;i++){assert(!feed(d,t,1,90));}rest(d,t); // rotation
 for(int i=0;i<100;i++)assert(!feed(d,t,1+.08f*std::sin(i))); // handling/noise
 auto stale=sample(t-100);assert(!d.consume(stale,t,true));
 auto invalid=sample(t+20);invalid.valid=false;assert(!d.consume(invalid,t+20,true));
 auto clipped=sample(t+40,4);assert(!d.consume(clipped,t+40,true));
 rest(d,t);auto same=sample(t);for(int i=0;i<50;i++)assert(!d.consume(same,t,true));
 for(int i=0;i<100;i++){assert(!feed(d,t,2.1,2.2,false));}rest(d,t);assert(peaks(d,t));
 d.reset();rest(d,t);for(int i=0;i<3;i++){feed(d,t,2.1);feed(d,t,1);}d.reset();rest(d,t); // context cancellation
 t=0xfffffc00u;d.reset();rest(d,t);assert(peaks(d,t)); // rollover
 puts("PASS gesture: synthetic multi-peak/rest, bias, noise/bump/rotation, bad/stale/repeat/clipped, cooldown/context, rollover");
}
