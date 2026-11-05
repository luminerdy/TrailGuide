// Actual native production compass pixels, pointer input and lifecycle regressions.
#include <lvgl.h>
#include <cassert>
static bool failIdleTimer=false,lowMonitor=false,exhaustAtMonitor=false;
static void *pressure[256];static unsigned pressureCount=0;
static lv_timer_t *testCreate(lv_timer_cb_t cb,uint32_t ms,void *ctx){return failIdleTimer&&ms==160?nullptr:lv_timer_create(cb,ms,ctx);}
static void testMonitor(lv_mem_monitor_t *m){lv_mem_monitor(m);if(exhaustAtMonitor){exhaustAtMonitor=false;while(m->free_biggest_size>=sizeof(lv_timer_t)+64 && pressureCount<256){pressure[pressureCount++]=lv_mem_alloc(m->free_biggest_size>2048?m->free_biggest_size-1024:16);assert(pressure[pressureCount-1]);lv_mem_monitor(m);}assert(m->free_biggest_size<sizeof(lv_timer_t)+64);}if(lowMonitor)m->free_biggest_size=0;}
#define lv_timer_create testCreate
#define lv_mem_monitor testMonitor
#include "../trailguide_v3/ui.h"
#undef lv_timer_create
#undef lv_mem_monitor
#include <cassert>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <cmath>
static constexpr auto OUT="build/v3-0.1.22/evidence/compass/";
static lv_color_t buffer[360*20],pixels[360*360];
static uint32_t seed=7;
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;++y)for(int x=a->x1;x<=a->x2;++x)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static lv_point_t point;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static void tap(int x,int y){point={lv_coord_t(x),lv_coord_t(y)};pressed=true;advance(60);pressed=false;advance(60);}
static lv_obj_t *find(lv_obj_t *o,const char *text){if(lv_obj_check_type(o,&lv_label_class)&&!strcmp(lv_label_get_text(o),text))return o;for(unsigned i=0;i<lv_obj_get_child_cnt(o);++i)if(auto *p=find(lv_obj_get_child(o,i),text))return p;return nullptr;}
static void clickText(v3app::CoachUi &u,const char *text){auto *o=find(u.root,text);assert(o);lv_area_t a;lv_obj_get_coords(lv_obj_get_parent(o),&a);tap((a.x1+a.x2)/2,(a.y1+a.y2)/2);}
static std::string raster(){lv_refr_now(nullptr);return std::string(reinterpret_cast<char*>(pixels),sizeof pixels);}
static void save(const std::string &name){lv_refr_now(nullptr);std::ofstream f(std::string(OUT)+name+".ppm",std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";for(auto p:pixels){lv_color32_t c;c.full=lv_color_to32(p);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}}

static unsigned timers(){unsigned n=0;for(auto *t=lv_timer_get_next(nullptr);t;t=lv_timer_get_next(t))++n;return n;}
static v3::ImuSample feed(v3app::Ui &u,float z){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=z;s.gyr_dps[1]=2.2f;u.sample(s,s.at_ms,true);u.sample(s,s.at_ms+1,true);return s;}
static void pose(v3app::Ui &u,float z,unsigned n){while(n--)feed(u,z);}
static bool black(){lv_refr_now(nullptr);for(auto p:pixels)if(p.full)return false;return true;}
static const char *names[]={"Progress","Stuck","Unclear","Explore","AI","Team"};
using Geometry=std::array<std::array<int,4>,6>;
static Geometry geometry(v3app::CoachUi &c){lv_obj_update_layout(c.root);Geometry g{};for(int i=0;i<6;++i){auto *label=find(c.root,names[i]);assert(label);auto *o=lv_obj_get_parent(label);lv_area_t a;lv_obj_get_coords(o,&a);g[i]={a.x1,a.y1,a.x2,a.y2};}return g;}
int main(){
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);
 const auto baseTimers=timers();
 {seed=7;v3app::Ui u(rng,&seed,malloc,free);assert(!u.coachingMode&&!u.coach.compassHome);u.render();save("classic-initial");tap(180,180);tap(180,90);assert(u.coachingMode&&u.coach.compassHome);assert(lv_obj_has_flag(u.coach.scroll,LV_OBJ_FLAG_HIDDEN));assert(!find(u.coach.root,"Options"));save("home");auto g=geometry(u.coach);auto first=raster();tap(180,180);assert(u.coach.model.current.screen==trailguide_coach::Screen::Options);save("options");clickText(u.coach,"Back");assert(first==raster());assert(geometry(u.coach)==g);}
 puts("PASS native Classic boot/two-tap Coaching; approved wordless home; center Options/Back");
 for(int attempt=0;attempt<2;++attempt)for(int p=0;p<6;++p){seed=7;v3app::Ui u(rng,&seed,malloc,free);u.perform(v3app::Ui::Action::Coaching);auto g=geometry(u.coach);auto calls=seed;auto opener=u.coach.model.current.index;advance(attempt?2160:60);tap((g[p][0]+g[p][2])/2,(g[p][1]+g[p][3])/2);assert(u.coach.model.current.screen==trailguide_coach::Screen::Question&&u.coach.model.current.path==p);save(std::string("selected-")+names[p]);auto q=u.coach.model.current;clickText(u.coach,"Guide me");assert(u.coach.model.current.screen==trailguide_coach::Screen::Guide);save(std::string("guide-")+names[p]);clickText(u.coach,"Back");assert(u.coach.model.current.index==q.index);clickText(u.coach,"Release");assert(u.coach.model.current.screen==trailguide_coach::Screen::Closing);clickText(u.coach,"Back");assert(u.coach.model.current.index==q.index&&u.coach.model.current.path==p);clickText(u.coach,"Paths");clickText(u.coach,"Opening");assert(u.coach.compassHome&&u.coach.model.current.index==opener&&u.coach.model.draws==1&&seed==calls);tap(180,180);clickText(u.coach,"Back");assert(u.coach.compassHome);printf("PASS actual pointer %s question/Guide/Back/Release/Paths/Opening context RNG unchanged\n",names[p]);}
 {seed=7;v3app::Ui u(rng,&seed,malloc,free);u.perform(v3app::Ui::Action::Coaching);advance(60);auto oldSeed=seed;auto draws=u.coach.model.draws;auto index=u.coach.model.current.index;auto first=raster();lv_mem_monitor_t before;lv_mem_monitor(&before);for(int i=0;i<120;++i){advance(50);char name[40];snprintf(name,sizeof name,"frames/frame-%03d",i);save(name);assert(seed==oldSeed&&u.coach.model.draws==draws&&u.coach.model.current.index==index&&u.coach.model.current.path==-1&&!u.busy());}assert(first!=raster());lv_mem_monitor_t after;lv_mem_monitor(&after);assert(before.free_size==after.free_size);tap(180,180);assert(u.coach.model.current.screen==trailguide_coach::Screen::Options);clickText(u.coach,"Classic");assert(!u.coachingMode&&u.root&&!u.coach.root);advance(300);tap(180,180);tap(180,90);assert(u.coach.compassHome&&u.coach.model.current.index==index&&u.coach.model.draws==draws&&seed==oldSeed);}
 puts("PASS 120 production idle frames/6 seconds, stable heap/card RNG/model/path; center after idle and Classic reentry");
 {v3app::Ui u(rng,&seed,malloc,free);u.perform(v3app::Ui::Action::Coaching);pose(u,1,40);auto oldSeed=seed;auto draws=u.coach.model.draws;pose(u,-1,10);assert(black()&&seed==oldSeed);save("compass-black");advance(1000);assert(black());auto s=feed(u,-1);s.valid=false;u.sample(s,s.at_ms,true);assert(!black()&&u.coach.compassHome&&seed==oldSeed);save("compass-cancel");pose(u,1,50);pose(u,-1,10);assert(black());tap(180,180);assert(!black()&&u.coach.model.current.screen==trailguide_coach::Screen::Opening&&u.coach.compassHome);pose(u,1,50);pose(u,-1,10);assert(black());pose(u,1,10);assert(u.busy()&&!u.coach.compassHome&&u.coach.model.draws==draws+1&&!u.coach.fallback);save("reveal-start");advance(3000);save("reveal-mid");advance(3000);assert(!u.busy());save("reveal-settled");auto *options=find(u.coach.root,"Options");assert(options&&lv_obj_has_flag(lv_obj_get_parent(options),LV_OBJ_FLAG_HIDDEN));advance(4900);assert(lv_obj_has_flag(lv_obj_get_parent(options),LV_OBJ_FLAG_HIDDEN));advance(200);assert(!lv_obj_has_flag(lv_obj_get_parent(options),LV_OBJ_FLAG_HIDDEN));clickText(u.coach,"Options");clickText(u.coach,"Back");assert(!u.coach.compassHome&&!lv_obj_has_flag(u.coach.scroll,LV_OBJ_FLAG_HIDDEN));}
 puts("PASS synthetic real Gesture compass blackout/cancel/swallowed center/one draw; card 6000ms reveal + 5000ms hold; card Options Back variant");
 for(int mode=0;mode<3;++mode){failIdleTimer=mode==0;lowMonitor=mode==1;exhaustAtMonitor=mode==2;v3app::Ui u(rng,&seed,malloc,free);u.perform(v3app::Ui::Action::Coaching);if(mode==2){assert(pressureCount);lv_mem_monitor_t m;lv_mem_monitor(&m);assert(m.free_biggest_size<sizeof(lv_timer_t)+64);while(pressureCount)lv_mem_free(pressure[--pressureCount]);}advance(60);auto first=raster();auto n=timers();advance(2000);assert(first==raster()&&!u.busy()&&timers()==n);tap(180,180);assert(u.coach.model.current.screen==trailguide_coach::Screen::Options);failIdleTimer=lowMonitor=false;}
 puts("PASS null timer + injected and genuinely exhausted LVGL largest-block preflight leave static, fully interactive compass");
 for(int i=0;i<40;++i){v3app::Ui u(rng,&seed,malloc,free);u.perform(v3app::Ui::Action::Coaching);advance(170);if(i%4==0)lv_obj_del(u.coach.root);if(i%4==1)lv_obj_del(u.coach.backButton);if(i%4==2){u.coach.perform(v3app::CoachUi::Action::Options);u.coach.perform(v3app::CoachUi::Action::Another);}advance(12000);}assert(timers()==baseTimers);
 puts("PASS root/child deletion, navigation/reveal, destruction and stale callback stress; all owned timers gone; ASAN+UBSAN host only, not installed");
}
