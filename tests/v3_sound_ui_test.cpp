// Actual pinned native LVGL; no hardware/audio writes.
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
static lv_color_t buffer[360*20],pixels[360*360];
static uint32_t seed=7;
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static bool failCapture=false,failRevealTimer=false;
static void *allocate(size_t n){return failCapture?nullptr:malloc(n);}
extern "C" lv_timer_t *__real_lv_timer_create(lv_timer_cb_t,uint32_t,void*);
extern "C" lv_timer_t *__wrap_lv_timer_create(lv_timer_cb_t cb,uint32_t period,void *user){if(period==40&&failRevealTimer)return nullptr;return __real_lv_timer_create(cb,period,user);}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static lv_point_t point;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static void tap(int x,int y){point={lv_coord_t(x),lv_coord_t(y)};pressed=true;advance(60);pressed=false;advance(60);}
static lv_obj_t *find(lv_obj_t *o,const char *text){if(lv_obj_check_type(o,&lv_label_class)&&!strcmp(lv_label_get_text(o),text))return o;for(unsigned i=0;i<lv_obj_get_child_cnt(o);++i)if(auto *p=find(lv_obj_get_child(o,i),text))return p;return nullptr;}
static lv_obj_t *control(lv_obj_t *root,const char *text,int x,int y,int w,int h){lv_obj_update_layout(root);auto *t=find(root,text);assert(t);auto *o=lv_obj_get_parent(t);lv_area_t a,b;lv_obj_get_coords(o,&a);lv_obj_get_coords(t,&b);assert(a.x1==x&&a.y1==y&&lv_obj_get_width(o)==w&&lv_obj_get_height(o)==h);assert(b.x1>=a.x1&&b.x2<=a.x2&&b.y1>=a.y1&&b.y2<=a.y2);for(int px:{int(a.x1)-2,int(a.x2)+2})for(int py:{int(a.y1)-2,int(a.y2)+2})assert((px-180)*(px-180)+(py-180)*(py-180)<=168*168);return o;}
static void save(const char *name){lv_refr_now(nullptr);std::ofstream f(std::string("build/v3-0.1.28/evidence/ui/")+name+".ppm",std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";for(auto p:pixels){lv_color32_t c;c.full=lv_color_to32(p);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}}
int main(){lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);
 using U=v3app::Ui;using C=v3app::CoachUi;
 {U u(rng,&seed,allocate,free);assert(!u.sound.enabled&&u.sound.sequence==0&&u.coach.sound==&u.sound);u.render();tap(180,180);
 control(u.root,"Instant OFF",76,252,100,48);control(u.root,"Sound OFF",184,252,100,48);save("after-classic-menu");auto s=seed;
 tap(234,276);assert(u.sound.enabled&&u.sound.sequence==0&&seed==s);control(u.root,"Sound ON",184,252,100,48);
 tap(126,276);assert(u.instant&&u.sound.enabled&&u.sound.sequence==0&&seed==s);
 tap(180,92);assert(u.coachingMode);assert(u.sound.sequence==0);s=seed;u.coach.perform(C::Action::Options);
 control(u.coach.root,"Reminders",88,268,88,48);control(u.coach.root,"Sound ON",184,268,88,48);save("after-coach-menu");
 tap(228,292);assert(!u.sound.enabled&&u.sound.sequence==0&&seed==s);control(u.coach.root,"Sound OFF",184,268,88,48);
 u.coach.perform(C::Action::Classic);u.perform(U::Action::Options);assert(find(u.root,"Sound OFF"));}
 {C c(rng,&seed,allocate,free);assert(c.sound&&!c.sound->enabled&&c.sound->sequence==0);}
 for(bool coaching:{false,true}){U u(rng,&seed,allocate,free);if(coaching){u.perform(U::Action::Coaching);u.coach.perform(C::Action::Another);}else u.perform(U::Action::Ask);advance(6100);assert(u.sound.sequence==0);}
 static_assert(unsigned(C::Action::Progress)==0&&unsigned(C::Action::Team)==5,"path numeric IDs unchanged");
 assert(!strcmp(v3app::version,"v3-0.1.28-controls"));
 // Each explicit new draw cues once only when the whole card becomes readable.
 for(bool coaching:{false,true}){U u(rng,&seed,allocate,free);u.sound.enabled=true;if(coaching)u.perform(U::Action::Coaching);else u.render();assert(u.sound.sequence==0);
 auto draw=[&](){if(coaching)u.coach.perform(C::Action::Another);else u.perform(U::Action::Ask);};
 draw();assert(u.busy()&&u.sound.sequence==0);advance(5990);assert(u.sound.sequence==0);advance(20);assert(!u.busy()&&u.sound.sequence==1);advance(12000);assert(u.sound.sequence==1);
 draw();advance(500);tap(180,180);assert(!u.busy()&&u.sound.sequence==2);tap(180,180);advance(12000);assert(u.sound.sequence==2);
 if(coaching)u.coach.instant=true;else u.instant=true;draw();assert(!u.busy()&&u.sound.sequence==3);advance(12000);assert(u.sound.sequence==3);
 if(coaching)u.coach.instant=false;else u.instant=false;failCapture=true;draw();failCapture=false;assert(!u.busy()&&u.sound.sequence==4);advance(12000);assert(u.sound.sequence==4);
 failRevealTimer=true;draw();failRevealTimer=false;assert(!u.busy()&&u.sound.sequence==5);advance(12000);assert(u.sound.sequence==5);
 // Navigation/rerender is cancellation, never deferred settlement.
 draw();if(coaching)u.coach.perform(C::Action::Guide);else u.perform(U::Action::Guide);advance(12000);assert(u.sound.sequence==5);
 if(coaching)u.coach.perform(C::Action::Back);else u.perform(U::Action::Back);advance(12000);assert(u.sound.sequence==5);
 draw();if(coaching)u.coach.render();else u.render();advance(12000);assert(u.sound.sequence==5);
 draw();if(coaching)lv_obj_del(u.coach.body);else lv_obj_del(u.body);advance(12000);assert(u.sound.sequence==5);
 draw();u.sound.enabled=false;advance(6100);assert(u.sound.sequence==5);u.sound.enabled=true;tap(180,180);advance(12000);assert(u.sound.sequence==5);
 u.sound.enabled=false;draw();advance(500);u.sound.enabled=true;advance(5600);assert(u.sound.sequence==6);
 draw();if(coaching)u.coach.perform(C::Action::Sound);else u.perform(U::Action::Sound);advance(12000);assert(!u.sound.enabled&&u.sound.sequence==6);
 u.sound.enabled=true;draw();if(coaching)lv_obj_del(u.coach.root);else lv_obj_del(u.root);advance(12000);assert(u.sound.sequence==6);
 }
 // Existing opener restoration, paths, help, menus and closing are silent.
 {U u(rng,&seed,allocate,free);u.sound.enabled=true;u.perform(U::Action::Coaching);auto &c=u.coach;
 for(auto a:{C::Action::Options,C::Action::Back,C::Action::Guide,C::Action::Back,C::Action::Stuck,C::Action::Next,C::Action::Experiment,C::Action::Question,C::Action::Picker,C::Action::Opening,C::Action::Reminders,C::Action::Release,C::Action::Back}){c.perform(a);advance(6100);assert(u.sound.sequence==0);}
 c.perform(C::Action::Classic);u.perform(U::Action::Ask);u.perform(U::Action::More);advance(12000);assert(u.sound.sequence==0);u.perform(U::Action::Guide);u.perform(U::Action::Back);u.perform(U::Action::More);assert(u.sound.sequence==0);}
 // Standalone ownership can bind an externally observed cue for destructor checks.
 for(bool rootDelete:{false,true}){v3app::SoundCue cue;cue.enabled=true;{C c(rng,&seed,allocate,free);c.sound=&cue;c.model.enter();c.render();c.perform(C::Action::Another);assert(c.busy());if(rootDelete)lv_obj_del(c.root);}advance(12000);assert(cue.sequence==0);}
 for(bool coaching:{false,true}){{U u(rng,&seed,allocate,free);u.sound.enabled=true;if(coaching){u.perform(U::Action::Coaching);u.coach.perform(C::Action::Another);}else u.perform(U::Action::Ask);assert(u.busy());}advance(12000);}
 {U u(rng,&seed,allocate,free);assert(!u.sound.enabled&&u.sound.sequence==0);}
 puts("PASS actual LVGL8.3.10 ASan/UBSan: shared default-muted RAM preference; both real-pointer menu toggles/caption fit/focus-circle geometry; Classic+Coaching natural 6s/early tap/Instant/capture-failure/timer-failure exactly-once settlement; muted-at-settlement and enable-during-reveal; cancellation/rerender/body/root/destructor/stale-timer safety; silent entry/help/menu/path/closing/opener restoration; candidate18. HOST ONLY, NO AUDIO/HARDWARE");
}
