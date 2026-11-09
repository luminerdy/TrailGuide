// Actual pinned LVGL renderer + native pointer; synthetic real ImuSample.
#include <lvgl.h>
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
static lv_color_t buffer[360*20],pixels[360*360];
static unsigned events=0,lastEventAt=0;
static uint32_t rng(void *p){auto &s=*static_cast<unsigned*>(p);return s=s*1664525u+1013904223u;}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;++y)for(int x=a->x1;x<=a->x2;++x)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static lv_point_t point;static bool pressed=false;
static void touch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
template<class U> static void frame(U &u,float x=0,float z=1,unsigned dt=20,float rate=0){advance(dt);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[0]=x;s.acc_g[2]=z;s.gyr_dps[1]=rate;bool hit=false;u.sample(s,s.at_ms,true,&hit);events+=hit;if(hit)lastEventAt=s.at_ms;}
template<class U> static void quiet(U &u,unsigned n=26,float z=1){while(n--)frame(u,0,z);}
template<class U> static void burst(U &u,unsigned n=55,float z=1){for(unsigned i=0;i<n;++i)frame(u,i%2? -1.1f:1.1f,z);}

static bool black(){lv_refr_now(nullptr);for(auto p:pixels)if(p.full)return false;return true;}
template<class U> static void flip(U &u){quiet(u,45);quiet(u,11,-1);assert(black());quiet(u,11);}
static void globalContexts(){
 for(bool instant:{false,true})for(bool useFlip:{false,true}){
  for(auto screen:{v3app::Screen::Opening,v3app::Screen::Question,v3app::Screen::Experiment,v3app::Screen::Guide,v3app::Screen::Options,v3app::Screen::Closing,v3app::Screen::Picker,v3app::Screen::Reminders}){
   unsigned actualSeed=7,expectedSeed=7;v3app::CoachUi c(rng,&actualSeed,malloc,free);trailguide_coach::CoachModel expected(rng,&expectedSeed);
   c.instant=instant;c.model.enter();expected.enter();c.model.path(0);c.model.guide();c.model.current.screen=screen;c.render();expected.draw();auto n=c.model.draws;events=0;
   assert(c.eligible()&&"Every live Coaching screen must allow motion start-fresh");
   if(useFlip)flip(c);else {quiet(c);burst(c);}
   assert(events==1&&c.model.draws==n+1&&c.busy()==!instant);assert(c.model.current.screen==v3app::Screen::Opening&&c.model.current.path==-1&&c.model.current.scroll==0);
   assert(c.model.current.index==expected.current.index&&actualSeed==expectedSeed&&!c.compassHome);
   auto fresh=c.model.current;c.model.back();assert(c.model.current.index==fresh.index&&c.model.current.screen==fresh.screen);
  }
  for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try})for(unsigned context=0;context<4;++context){
   unsigned actualSeed=7,expectedSeed=7;v3app::Ui u(rng,&actualSeed,malloc,free);v3app::Model expected(rng,&expectedSeed);
   u.instant=instant;u.model.draw(mode);expected.draw(mode);
   if(context==0){u.model.home();}if(context==2){u.model.guide();}u.render();if(context==3){u.perform(v3app::Ui::Action::Home);u.perform(v3app::Ui::Action::Options);}
   expected.draw(mode);events=0;assert(u.eligible()&&"Classic Home/Card/Guide/Options must allow motion");
   if(useFlip)flip(u);else {quiet(u);burst(u);}
   assert(events==1&&u.model.view==v3app::View::Card&&u.model.mode==mode&&u.body&&u.scroll&&u.busy()==!instant);
   assert(u.model.index==expected.index&&actualSeed==expectedSeed&&!u.diagnostic({},lv_tick_get()).options);
  }
 }
 puts("PASS shake+flip global Classic/Coaching contexts, mode, opener bag, history reset");
}

template<class U> static void deadlineFlip(U &u){
 events=0;quiet(u,45);quiet(u,241);quiet(u,11,-1);assert(u.busy()&&black());
 // Cross the old reveal deadline while inverted. It must stay black and armed.
 quiet(u,2,-1);advance(40);assert(!u.busy()&&black()&&"Automatic settlement cannot unblank a qualified inversion");
 quiet(u,11);assert(events==1&&u.busy()&&!black());
 advance(4980);assert(u.busy()&&lv_obj_has_flag(u.backButton,LV_OBJ_FLAG_HIDDEN));
 advance(1020);assert(!u.busy());advance(2000);assert(!lv_obj_has_flag(u.backButton,LV_OBJ_FLAG_HIDDEN));
}
template<class U> static void deadlineShake(U &u){
 events=0;quiet(u,45);advance(4500);quiet(u,21);burst(u,46);
 assert(events==1&&u.busy()&&"Shake burst straddling old reveal completion must retain evidence");
 burst(u,600);assert(events==1&&!u.busy());
}
static void deadlineLifecycle(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Try);deadlineFlip(u);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);deadlineFlip(c);}
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Try);deadlineShake(u);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);deadlineShake(c);}
 puts("PASS old reveal deadline preserves inverted blackout and partial shake; no stale hold or queued draw");
}


template<class U> static void blackSettlementCue(U &u,v3app::SoundCue &sound,bool shakeInstead){
 sound.enabled=true;events=0;quiet(u,286);quiet(u,13,-1);advance(40);
 assert(black()&&!u.busy());assert(sound.sequence==0&&"A hidden old card has not become readable and must not cue");
 if(shakeInstead)burst(u,46,-1);else quiet(u,11);
 assert(events==1&&u.busy()&&sound.sequence==0&&"Aborting hidden old settlement must not leak its pending cue");
 advance(6000);assert(!u.busy()&&sound.sequence==1);advance(6000);assert(sound.sequence==1);
}
static void soundLifecycle(){
 for(bool shakeInstead:{false,true}){
  {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Ask);blackSettlementCue(u,u.sound,shakeInstead);}
  {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);blackSettlementCue(c,*c.sound,shakeInstead);}
 }
 puts("PASS interrupted hidden settlement cue canceled; new readable card cues exactly once");
}


static void manualCancellation(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Ask);events=0;quiet(u);burst(u,30);u.perform(v3app::Ui::Action::Another);burst(u,30);assert(events==0&&"Manual same-card finish cancels partial shake");}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);events=0;quiet(c);burst(c,30);c.perform(v3app::CoachUi::Action::Another);burst(c,30);assert(events==0);}
 puts("PASS manual reveal finish cancels partial shake, unlike autonomous finish");
}
static void deletedHiddenCue(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.sound.enabled=true;u.perform(v3app::Ui::Action::Ask);quiet(u,286);quiet(u,13,-1);advance(40);assert(black()&&u.sound.sequence==0);lv_obj_del(u.body);assert(u.sound.sequence==0&&"Deleting hidden old content must not emit its deferred cue");advance(12000);assert(!u.busy()&&u.sound.sequence==0);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.sound->enabled=true;c.enterCompass();c.perform(v3app::CoachUi::Action::Another);quiet(c,286);quiet(c,13,-1);advance(40);assert(black()&&c.sound->sequence==0);lv_obj_del(c.body);assert(c.sound->sequence==0);advance(12000);assert(!c.busy()&&c.sound->sequence==0);}
 puts("PASS deletion of hidden content cancels deferred cue and all owned deadlines");
}


// Exercise completed inputs at distinct points, not just settled/Home entry.
template<class U> static void interruptionAt(U &u,v3app::SoundCue &sound,unsigned waitFrames,bool useFlip){
 sound.enabled=true;events=0;quiet(u,waitFrames);
 if(useFlip){quiet(u,45);quiet(u,11,-1);assert(black());quiet(u,11);}else{quiet(u);burst(u,46);}
 assert(events==1&&u.busy()&&!black());auto cues=sound.sequence;
 assert(cues==(waitFrames>=300?1u:0u));
 advance(5990-lv_tick_elaps(lastEventAt));assert(u.busy()&&lv_obj_has_flag(u.backButton,LV_OBJ_FLAG_HIDDEN)&&sound.sequence==cues);
 advance(10);assert(!u.busy()&&sound.sequence==cues+1);advance(1990);assert(lv_obj_has_flag(u.backButton,LV_OBJ_FLAG_HIDDEN));advance(10);assert(!lv_obj_has_flag(u.backButton,LV_OBJ_FLAG_HIDDEN));
}
static void interruptionMatrix(){
 for(bool useFlip:{false,true})for(unsigned waitFrames:{0u,125u,215u,300u,325u,330u}){
  {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Try);interruptionAt(u,u.sound,waitFrames,useFlip);assert(u.model.mode==v3app::Mode::Try);}
  {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();auto n=c.model.draws;c.perform(v3app::CoachUi::Action::Another);interruptionAt(c,*c.sound,waitFrames,useFlip);assert(c.model.draws==n+2);}
 }
 puts("PASS both gestures early/mid/near-settle, hold and focus expiry: fresh exact 6s+2s, no stale cue/chrome");
}
template<class U> static void focusCrossing(U &u){
 quiet(u,386);quiet(u,11,-1);assert(black());advance(100);assert(black()&&!u.busy());
 quiet(u,11);assert(events==1&&u.busy());
}
static void focusDeadline(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);events=0;u.perform(v3app::Ui::Action::Ask);focusCrossing(u);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);events=0;c.enterCompass();c.perform(v3app::CoachUi::Action::Another);focusCrossing(c);}
 puts("PASS inverted blackout survives old focus deadline without consuming the flip");
}
template<class U> static void partialCancel(U &u){
 events=0;quiet(u,45);// no RNG supplied by partial input
 quiet(u,11,-1);assert(black());v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=-1;
 bool hit=false;u.sample(s,s.at_ms,false,&hit);assert(!hit&&events==0&&!black());
 quiet(u,11);assert(events==0&&"Canceled return must never consume a draw");
}
static void partialRestoration(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.model.draw(v3app::Mode::Try);u.render();auto index=u.model.index;auto before=s;partialCancel(u);assert(u.model.index==index&&s==before&&u.model.mode==v3app::Mode::Try&&!lv_obj_has_flag(u.body,LV_OBJ_FLAG_HIDDEN));}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.model.enter();c.model.path(0);c.model.guide();c.render();auto before=c.model.current;auto n=c.model.draws;auto r=s;partialCancel(c);assert(c.model.draws==n&&s==r&&c.model.current.index==before.index&&c.model.current.screen==before.screen&&!lv_obj_has_flag(c.body,LV_OBJ_FLAG_HIDDEN));}
 puts("PASS canceled inversion restores content, no RNG/draw/context mutation");
}


static void manualBlankFinish(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Ask);quiet(u,45);quiet(u,11,-1);assert(black());u.perform(v3app::Ui::Action::Another);assert(!black()&&!u.busy()&&"Manual reveal finish restores the same card immediately");}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);quiet(c,45);quiet(c,11,-1);assert(black());c.perform(v3app::CoachUi::Action::Another);assert(!black()&&!c.busy());}
 puts("PASS explicit finish cancels blackout immediately without another IMU frame");
}


static unsigned allocations=0,releases=0;
static void *tracked(size_t n){auto *p=malloc(n);if(p)++allocations;return p;}
static void releaseTracked(void *p){if(p)++releases;free(p);}
static void resourceLifecycle(){
 allocations=releases=0;
 {unsigned s=7;v3app::Ui u(rng,&s,tracked,releaseTracked);u.perform(v3app::Ui::Action::Ask);events=0;quiet(u);burst(u,46);assert(events==1&&allocations==2&&releases==1);lv_obj_del(u.root);assert(!u.root&&!u.busy()&&allocations==releases);advance(12000);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,tracked,releaseTracked);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);events=0;quiet(c);burst(c,46);assert(events==1&&allocations==4&&releases==3);lv_obj_del(c.root);assert(!c.root&&!c.busy()&&allocations==releases);advance(12000);}
 assert(allocations==releases);
 puts("PASS interrupted snapshot released once; root deletion cancels timers/cue and frees current snapshot");
}
template<class U> static void restoredSettlement(U &u,v3app::SoundCue &sound){
 sound.enabled=true;quiet(u,286);quiet(u,13,-1);advance(40);assert(black()&&sound.sequence==0);
 v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=-1;u.sample(s,s.at_ms,false);assert(!black()&&sound.sequence==1);
 u.sample(s,s.at_ms,false);advance(6000);assert(sound.sequence==1);
}
static void restoreCue(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Ask);restoredSettlement(u,u.sound);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);restoredSettlement(c,*c.sound);}
 puts("PASS canceled inversion restoring a settled original cues only once, without drawing");
}


template<class U> static void realPointerFlip(U &u){
 events=0;quiet(u,45);quiet(u,11,-1);assert(black());
 point={180,180};pressed=true;advance(40);assert(!u.eligible()&&!black());
 // No intervening IMU sample was needed to restore or cancel the inversion.
 pressed=false;advance(80);quiet(u,11);assert(events==0&&!black()&&u.busy());
 quiet(u,45);point={180,180};pressed=true;advance(40);assert(!u.eligible());
 quiet(u,45);quiet(u,11,-1);quiet(u,11);assert(events==0&&!black());
 pressed=false;advance(80);quiet(u,45);quiet(u,11,-1);assert(black());quiet(u,11);assert(events==1);
}
static void pointerFlipPolicy(){
 {unsigned s=7;v3app::Ui u(rng,&s,malloc,free);u.perform(v3app::Ui::Action::Try);realPointerFlip(u);}
 {unsigned s=7;v3app::CoachUi c(rng,&s,malloc,free);c.enterCompass();c.perform(v3app::CoachUi::Action::Another);realPointerFlip(c);}
 puts("PASS real pointer restores/cancels blackout without IMU, held contact blocks complete flips and no queued return fires");
}

int main(){setbuf(stdout,nullptr);lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=touch;lv_indev_drv_register(&input);globalContexts();deadlineLifecycle();soundLifecycle();manualCancellation();deletedHiddenCue();interruptionMatrix();focusDeadline();partialRestoration();manualBlankFinish();resourceLifecycle();restoreCue();pointerFlipPolicy();}