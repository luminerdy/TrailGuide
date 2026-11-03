#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
static lv_color_t buf[360*20], screen[360*360];
static unsigned allocations=0,outstanding=0;static bool fail=false,starveFocus=false;static void *focusMemoryBlock=nullptr;
static void *focusFragments[64]{};static unsigned focusFragmentCount=0;
static void *allocate(size_t n){if(starveFocus){lv_mem_monitor_t m;lv_mem_monitor(&m);focusMemoryBlock=lv_mem_alloc(m.free_biggest_size-1024);assert(focusMemoryBlock);while(focusFragmentCount<64){void *p=lv_mem_alloc(64);if(!p)break;focusFragments[focusFragmentCount++]=p;}return nullptr;}if(fail)return nullptr;++allocations;++outstanding;return malloc(n);}
static void release(void *p){if(p){assert(outstanding);--outstanding;free(p);}}
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);s=s*1664525u+1013904223u;return s;}
static bool failFocusTimer=false;
extern "C" lv_timer_t *__real_lv_timer_create(lv_timer_cb_t,uint32_t,void*);
extern "C" lv_timer_t *__wrap_lv_timer_create(lv_timer_cb_t cb,uint32_t period,void *user){if(period==5000){lv_mem_monitor_t m;lv_mem_monitor(&m);assert(m.free_biggest_size>=sizeof(lv_timer_t)+64 && "do not enter asserting LVGL timer allocator on exhausted heap");if(failFocusTimer)return nullptr;}return __real_lv_timer_create(cb,period,user);}
static unsigned flushedPixels=0;
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){flushedPixels+=(a->x2-a->x1+1)*(a->y2-a->y1+1);for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++)screen[y*360+x]=*c++;lv_disp_flush_ready(d);}
static unsigned heapPeak=0;
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();lv_mem_monitor_t m;lv_mem_monitor(&m);heapPeak=std::max(heapPeak,unsigned(m.total_size-m.free_size));}lv_obj_update_layout(lv_scr_act());}
static void save(unsigned n){lv_refr_now(nullptr);char path[128];snprintf(path,sizeof(path),"build/v3-0.1.10/frames/frame-%03u.ppm",n);std::ofstream f(path,std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";for(auto p:screen){lv_color32_t c;c.full=lv_color_to32(p);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}}
static std::string face(){lv_refr_now(nullptr);std::string b;for(int y=112;y<252;y++)b.append(reinterpret_cast<char*>(screen+y*360+52),256*2);return b;}
static void bounds(lv_obj_t *o,bool control=false){assert(o);lv_area_t a;lv_obj_get_coords(o,&a);for(int x:{a.x1-2,a.x2+2})for(int y:{a.y1-2,a.y2+2})assert((x-180)*(x-180)+(y-180)*(y-180)<=168*168);if(control)assert(lv_obj_get_height(o)>=48);}
static unsigned timers(){unsigned n=0;for(auto *t=lv_timer_get_next(nullptr);t;t=lv_timer_get_next(t))++n;return n;}
static lv_point_t pt;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=pt;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static void press(int x,int y){pt={lv_coord_t(x),lv_coord_t(y)};pressed=true;advance(60);}
static void lift(){pressed=false;advance(60);}
int main(){
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buf,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);
 lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);
 uint32_t seed=7;
 {v3app::Ui u(rng,&seed,allocate,release);u.render();v3::ImuSample s;s.valid=true;s.acc_g[2]=1;s.gyr_dps[0]=2.2f;
  const auto initialSeed=seed;const auto count=timers();const auto allocated=allocations;
  for(unsigned t=20;t<=800;t+=20){s.at_ms=t;u.sample(s,t,true);const auto a=u.diagnostic(s,t);const auto b=u.diagnostic(s,t);assert(a.key()==b.key());}
  auto a=u.diagnostic(s,800);assert(a.gesture.phase==1 && a.gesture.haveOrigin && a.gesture.alignment==1 && a.eligible);
  s.valid=false;u.sample(s,820,true);a=u.diagnostic(s,820);assert(a.gesture.phase==4 && !a.healthy && a.eligible && !a.gesture.dwelling);
  assert(seed==initialSeed && allocations==allocated && timers()==count);
 }
 auto hidden=[](v3app::Ui &u,bool value){for(auto *o:{u.backButton,u.moreButton,u.first,u.second,u.motionStatus}){assert(o);assert(lv_obj_has_flag(o,LV_OBJ_FLAG_HIDDEN)==value);}};
 {v3app::Ui u(rng,&seed,allocate,release);u.render();u.perform(v3app::Ui::Action::Ask);hidden(u,true);
  const int id=u.model.index;const auto before=seed;save(300);const auto revealAlloc=allocations;advance(3000);save(303);advance(3000);assert(allocations==revealAlloc);
  assert(!u.busy() && u.eligible());hidden(u,true);save(301);
  advance(4990);hidden(u,true);advance(50);hidden(u,false);save(302);assert(u.model.index==id && seed==before);
  u.perform(v3app::Ui::Action::Another);hidden(u,true);press(180,180);lift();assert(!u.busy());hidden(u,true);
  int same=u.model.index;auto sameSeed=seed;press(180,180);lift();hidden(u,false);assert(u.model.index==same && seed==sameSeed);
  u.perform(v3app::Ui::Action::Another);advance(6000);same=u.model.index;sameSeed=seed;
  press(126,78);lift();hidden(u,false);assert(u.model.view==v3app::View::Card && u.model.index==same && seed==sameSeed && "hidden Home must not activate");
  u.perform(v3app::Ui::Action::Another);advance(6000);press(180,180);pt={220,180};advance(60);lift();hidden(u,true);
  advance(3000);u.perform(v3app::Ui::Action::Another);advance(2100);hidden(u,true);assert(u.busy());
  u.perform(v3app::Ui::Action::Guide);advance(12000);assert(u.model.view==v3app::View::Guide);hidden(u,false);
  u.perform(v3app::Ui::Action::Back);hidden(u,false);assert(!u.busy());
  u.instant=true;u.perform(v3app::Ui::Action::Another);hidden(u,true);assert(!u.busy());advance(4990);hidden(u,true);advance(50);hidden(u,false);
  u.instant=false;fail=true;u.perform(v3app::Ui::Action::Another);fail=false;assert(u.fallback && !u.busy());hidden(u,true);press(180,180);lift();hidden(u,false);
  // All four hidden control positions route to same-card tap escape, not actions.
  u.instant=true;for(auto p:{lv_point_t{126,78},lv_point_t{232,78},lv_point_t{126,280},lv_point_t{232,280}}){u.perform(v3app::Ui::Action::Another);hidden(u,true);const int same=u.model.index;const auto s=seed;press(p.x,p.y);lift();hidden(u,false);assert(u.model.view==v3app::View::Card && u.model.index==same && seed==s);}
  starveFocus=true;u.instant=false;u.perform(v3app::Ui::Action::Another);starveFocus=false;while(focusFragmentCount)lv_mem_free(focusFragments[--focusFragmentCount]);lv_mem_free(focusMemoryBlock);focusMemoryBlock=nullptr;hidden(u,false);assert(u.fallback && !u.busy());
  failFocusTimer=true;u.instant=true;u.perform(v3app::Ui::Action::Another);hidden(u,false);assert(!u.busy());
  u.instant=false;u.perform(v3app::Ui::Action::Another);hidden(u,true);advance(6000);hidden(u,false);failFocusTimer=false;
  // Independent chrome deletion must cancel the pending focus callback.
  u.instant=true;u.perform(v3app::Ui::Action::Another);auto withHold=timers();lv_obj_del(u.first);assert(timers()+1==withHold);advance(12000);assert(!u.first);
  // Deletion during settled hold must cancel the pending chrome callback.
  u.instant=true;u.perform(v3app::Ui::Action::Another);hidden(u,true);lv_obj_del(u.root);assert(!u.root);advance(12000);
 }advance(12000);assert(!outstanding);
 // Every completed flip after settle, including a bag boundary, draws exactly once.
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}){uint32_t expectedSeed=seed;v3app::Model expected(rng,&expectedSeed);
  v3app::Ui u(rng,&seed,allocate,release);u.render();u.perform(mode==v3app::Mode::Ask?v3app::Ui::Action::Ask:v3app::Ui::Action::Try);expected.draw(mode);
  auto feed=[&](float z){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=z;s.gyr_dps[1]=-2.2f;u.sample(s,s.at_ms,true);u.sample(s,s.at_ms+1,true);};
  auto quiet=[&](){for(int i=0;i<40;i++)feed(1);};auto flip=[&](){for(int i=0;i<25;i++)feed(-1);for(int i=0;i<25;i++)feed(1);};
  for(unsigned n=0;n<trailguide::cardCount+2;n++){assert(u.model.index==expected.index && seed==expectedSeed);advance(6000);hidden(u,true);assert(u.eligible());
   const int old=u.model.index;quiet();flip();expected.draw(mode);assert(u.model.index!=old && u.model.index==expected.index && seed==expectedSeed && u.busy());hidden(u,true);
  }
 }


 {v3app::Ui u(rng,&seed,allocate,release);u.render();advance(60);assert(u.model.index==-1);assert(!u.busy());assert(!u.first && !u.second && !u.toggle && !u.moreButton);
 assert(lv_obj_get_child_cnt(u.root)==1 && "Home is plain compass only");
 auto *rose=lv_obj_get_child(u.root,0);lv_area_t roseArea;lv_obj_get_coords(rose,&roseArea);
 assert(lv_obj_get_width(rose)>=320 && lv_obj_get_height(rose)>=320);
 assert(roseArea.x1+lv_obj_get_width(rose)/2==180 && roseArea.y1+lv_obj_get_height(rose)/2==180);
 assert(lv_obj_has_flag(rose,LV_OBJ_FLAG_ADV_HITTEST));
 lv_refr_now(nullptr);assert(screen[40*360+180].full!=screen[0].full && "large static compass rose must be visible");
 auto homePixels=face();save(100);for(unsigned n=0;n<40;++n){save(200+n);advance(40);}save(101);assert(face()!=homePixels && "Home compass must visibly wander");assert(u.model.index==-1 && !u.busy() && !outstanding);
 auto edgeSeed=seed;press(180,8);lift();assert(u.model.index==-1 && seed==edgeSeed && u.eligible() && "inside-object off-safe-circle contact must remain on pure compass");
 press(180,150);advance(500);assert(u.model.index==-1);lift();assert(!u.eligible() && u.model.index==-1 && seed==edgeSeed);press(126,208);lift();assert(u.busy());int id=u.model.index;assert(id>=0);bounds(u.scroll);bounds(u.backButton,true);bounds(u.moreButton,true);bounds(u.first,true);bounds(u.second,true);
 auto tappedSeed=seed;advance(60);assert(u.model.index==id && seed==tappedSeed);
 auto copy=std::string(lv_label_get_text(u.body));assert(std::strstr(lv_label_get_text(u.motionStatus),"ASK DRAFT"));assert(copy==trailguide::cards[id].ask);auto dark=face();advance(400);assert(face()==dark);u.perform(v3app::Ui::Action::Another);assert(!u.busy() && u.model.index==id);
 u.perform(v3app::Ui::Action::Guide);assert(!u.busy());assert(std::string(lv_label_get_text(u.body))==trailguide::cards[id].askGuide);bounds(u.scroll);u.perform(v3app::Ui::Action::Back);assert(u.model.index==id&&!u.busy());
 u.perform(v3app::Ui::Action::Another);auto initial=face();advance(1800);assert(initial!=face());u.perform(v3app::Ui::Action::Home);assert(!u.busy()&&!outstanding);advance(6500);assert(u.model.view==v3app::View::Home);
 u.perform(v3app::Ui::Action::Ask); // native host simulation sequence
 for(unsigned n=0;n<=60;n++){save(n);if(n<60)advance(100);}assert(!u.busy());auto settled=face();u.render();assert(face()==settled);assert(!outstanding);
 u.perform(v3app::Ui::Action::Another);u.window.render(6000);lv_obj_invalidate(u.scroll);auto projected=face();u.perform(v3app::Ui::Action::Another);assert(face()==projected); // exact final raster -> live glyph handoff
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}){u.model=v3app::Model(rng,&seed);u.model.view=v3app::View::Card;std::set<int> ids;
  for(unsigned i=0;i<trailguide::cardCount;i++){u.model.mode=mode;u.perform(v3app::Ui::Action::Another);ids.insert(u.model.index);auto &c=trailguide::cards[u.model.index];const char *text=mode==v3app::Mode::Ask?c.ask:c.action;assert(std::string(lv_label_get_text(u.body))==text);auto h=lv_obj_get_height(u.body);advance(2000);assert(std::string(lv_label_get_text(u.body))==text && lv_obj_get_height(u.body)==h);u.perform(v3app::Ui::Action::Guide);assert(!u.busy());const char *guide=mode==v3app::Mode::Ask?c.askGuide:c.tryGuide;assert(std::string(lv_label_get_text(u.body))==guide);assert(lv_obj_get_scroll_bottom(u.scroll)>0);u.perform(v3app::Ui::Action::More);assert(lv_obj_get_scroll_y(u.scroll)>0);u.perform(v3app::Ui::Action::Back);assert(!u.busy());}assert(ids.size()==trailguide::cardCount);
 }
 // Input drag from Another must not draw; release-based hold never repeats.
 id=u.model.index;press(230,280);pt={40,180};advance(100);lift();assert(u.model.index==id);press(0,0);lift();assert(u.model.index==id);
 u.perform(v3app::Ui::Action::Home);u.perform(v3app::Ui::Action::Instant);u.perform(v3app::Ui::Action::Try);assert(!u.busy());u.perform(v3app::Ui::Action::Home);u.perform(v3app::Ui::Action::Instant);
 fail=true;u.perform(v3app::Ui::Action::Ask);assert(!u.busy() && u.fallback && !outstanding);assert(!lv_obj_has_flag(u.body,LV_OBJ_FLAG_HIDDEN));fail=false;
 // Low LVGL available memory: renderer declines before snapshot assertions.
 u.render();lv_mem_monitor_t mem;lv_mem_monitor(&mem);void *block=lv_mem_alloc(mem.free_biggest_size-1500);assert(block);assert(!u.window.capture(u.scroll));lv_mem_free(block);
 u.perform(v3app::Ui::Action::Another);assert(u.busy());lv_obj_del(u.second);assert(!u.busy()&&!outstanding);
 u.render();lv_obj_del(u.motionStatus);v3::ImuSample fresh;fresh.valid=true;fresh.at_ms=lv_tick_get();fresh.acc_g[2]=1;u.sample(fresh,lv_tick_get(),true);assert(u.motionStatus==nullptr);
 u.perform(v3app::Ui::Action::Another);lv_obj_del(lv_obj_get_child(u.second,0));assert(!u.busy()&&!outstanding);
 u.perform(v3app::Ui::Action::Another);assert(u.busy());lv_obj_del(u.scroll);assert(!u.busy()&&!outstanding);advance(6500);
 for(int i=0;i<100;i++){u.perform(v3app::Ui::Action::Home);u.perform(v3app::Ui::Action::Ask);u.perform(v3app::Ui::Action::Guide);u.perform(v3app::Ui::Action::Back);assert(!u.busy()&&!outstanding);}
 lv_tick_inc(0xfffff000u-lv_tick_get());u.perform(v3app::Ui::Action::Another);advance(6100);assert(!u.busy());
 u.perform(v3app::Ui::Action::Another);lv_obj_clean(lv_scr_act());assert(!u.busy()&&!outstanding);advance(6500);
 u.render();u.perform(v3app::Ui::Action::Ask);assert(u.busy());
 }assert(!outstanding);advance(6500);
 for(unsigned t=0;t<=6000;t+=10){auto p=v3app::WindowReveal::pose(t);assert(p.light>=0&&p.light<=1);if(t<=600)assert(p.light==0);}assert(v3app::WindowReveal::pose(6000).z==0);
 // Compass lifecycle and touch alternatives use actual LVGL objects/input.
 const auto baselineTimers=timers();
 {v3app::Ui u(rng,&seed,allocate,release);u.render();advance(60);auto randomBefore=seed;auto timerBefore=timers();lv_mem_monitor_t m;lv_mem_monitor(&m);auto freeBefore=m.free_size;
  auto allocBefore=allocations;auto pixelsBefore=flushedPixels;advance(160);assert(flushedPixels-pixelsBefore<60000 && "idle animation must not invalidate full screen");advance(29840);lv_mem_monitor(&m);assert(allocations==allocBefore);assert(m.free_size==freeBefore && seed==randomBefore && u.model.index==-1 && !u.busy() && timers()==timerBefore);
  press(180,314);advance(500);assert(u.model.index==-1);lift();assert(!u.eligible() && !u.busy() && u.model.index==-1);bounds(u.first,true);bounds(u.second,true);bounds(u.toggle,true);bounds(u.backButton,true);
  save(104);press(180,276);lift();assert(u.instant && !u.busy());press(232,208);lift();assert(u.model.view==v3app::View::Card && u.model.mode==v3app::Mode::Try && !u.busy());
  u.perform(v3app::Ui::Action::Home);auto still=face();advance(1600);assert(still!=face() && !u.busy());
  lv_obj_del(u.root);assert(!u.root && timers()==baselineTimers);advance(6500);
  u.render();assert(timers()==baselineTimers+1);lv_obj_del(lv_obj_get_child(u.root,0));assert(timers()==baselineTimers);advance(6500);
  u.render();assert(timers()==baselineTimers+1);
  auto *child=lv_label_create(lv_obj_get_child(u.root,0));lv_label_set_text(child," ");lv_obj_del(child);advance(500);assert(timers()==baselineTimers+1 && !u.busy());
  // Both Options actions use real pointer release, without click-through draws.
  auto beforeOptions=seed;press(180,314);lift();assert(seed==beforeOptions && !u.eligible());press(180,276);lift();assert(!u.instant && seed==beforeOptions);press(126,208);advance(600);assert(seed==beforeOptions);lift();assert(u.model.mode==v3app::Mode::Ask && u.busy());auto afterAsk=seed;advance(200);assert(seed==afterAsk);
  u.perform(v3app::Ui::Action::Home);assert(!u.first && !u.toggle && u.eligible());
 }assert(timers()==baselineTimers);advance(6500);
 // Synthetic samples exercise the app gate, not physical motion acceptance.
 {v3app::Ui u(rng,&seed,allocate,release);u.render();
  auto feed=[&](float a,bool valid=true){advance(20);v3::ImuSample s;s.valid=valid;s.at_ms=lv_tick_get();s.acc_g[2]=a;s.gyr_dps[1]=-2.2f;u.sample(s,s.at_ms,true);u.sample(s,s.at_ms+1,true);u.sample(s,s.at_ms+2,true);};
  auto quiet=[&](){for(int i=0;i<40;i++)feed(1);};
  auto flip=[&](){for(int i=0;i<25;i++)feed(-1);for(int i=0;i<25;i++)feed(1);};
  // Options is a Home substate: no draw or RNG, no queued gesture on close.
  auto optionsSeed=seed;press(180,314);lift();quiet();flip();assert(!u.eligible() && u.model.index==-1 && seed==optionsSeed);
  press(180,140);lift();assert(u.eligible() && !u.busy() && u.model.index==-1 && seed==optionsSeed);
  quiet(); // freshly arm the returned intro
  // Contact on the intro is dropped, not queued after release.
  auto before=seed;v3app::Model expected(rng,&before);expected.draw(v3app::Mode::Ask);
  press(180,132);quiet();flip();assert(u.model.index==-1&&!u.busy());
  // Drag away cancels the compass click; quiet/rest must rearm after release.
  pt={0,0};advance(60);lift();
  assert(u.eligible());quiet();flip();assert(u.model.view==v3app::View::Card && u.busy() && u.model.mode==v3app::Mode::Ask && u.model.index==expected.index);assert(seed==before);int saved=u.model.index;
  save(102);auto darkHomeDraw=face();advance(1500);save(103);assert(face()!=darkHomeDraw);quiet();flip();assert(u.model.index==saved); // busy drop
  u.perform(v3app::Ui::Action::Guide);quiet();flip();assert(u.model.index==saved);u.perform(v3app::Ui::Action::Back);quiet();flip();assert(u.busy() && u.model.index!=saved);saved=u.model.index;quiet();flip();assert(u.model.index==saved); // draw once, never queue
  u.perform(v3app::Ui::Action::Home);advance(6500);assert(u.model.index==saved);u.perform(v3app::Ui::Action::Try);u.perform(v3app::Ui::Action::Another);saved=u.model.index;for(int i=0;i<100;i++)feed(2,false);assert(u.model.index==saved && !u.busy());
  u.perform(v3app::Ui::Action::Home);advance(3000);quiet();flip();assert(u.model.mode==v3app::Mode::Ask && u.busy());u.perform(v3app::Ui::Action::Another);
  u.perform(v3app::Ui::Action::Home);u.perform(v3app::Ui::Action::Instant);advance(3000);quiet();flip();assert(u.model.view==v3app::View::Card && u.model.mode==v3app::Mode::Ask && !u.busy());
 }
 // Regression: every pointer-down area and inertia phase blocks gesture draws.
 {v3app::Ui u(rng,&seed,allocate,release);u.render();u.perform(v3app::Ui::Action::Ask);u.perform(v3app::Ui::Action::Another);
  auto feed=[&](float a){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=a;s.gyr_dps[1]=-2.2f;u.sample(s,s.at_ms,true);};
  auto quiet=[&](){for(int i=0;i<40;i++)feed(1);};
  auto flip=[&](){for(int i=0;i<25;i++)feed(-1);for(int i=0;i<25;i++)feed(1);};
  int saved=u.model.index;press(180,106); // real input driver, blank root gap above face
  assert(!u.eligible() && "blank-background hold must suppress gesture");
  quiet();flip();assert(u.model.index==saved&&!u.busy());lift();
  assert(u.eligible());quiet();flip();assert(u.busy()&&u.model.index!=saved);
 }
 {v3app::Ui u(rng,&seed,allocate,release);u.render();u.perform(v3app::Ui::Action::Ask);u.perform(v3app::Ui::Action::Another);
  auto feed=[&](float a){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=a;s.gyr_dps[1]=-2.2f;u.sample(s,s.at_ms,true);};
  auto quiet=[&](){for(int i=0;i<40;i++)feed(1);};
  auto flip=[&](){for(int i=0;i<25;i++)feed(-1);for(int i=0;i<25;i++)feed(1);};
  int saved=u.model.index;
  lv_event_send(u.scroll,LV_EVENT_PRESSED,nullptr);lv_event_send(u.scroll,LV_EVENT_SCROLL_BEGIN,nullptr);lv_event_send(u.scroll,LV_EVENT_RELEASED,nullptr);
  assert(!u.eligible() && "release must not end inertial-scroll suppression");
  quiet();flip();assert(u.model.index==saved&&!u.busy());
  lv_event_send(u.scroll,LV_EVENT_SCROLL_END,nullptr);assert(u.eligible());
  quiet();flip();assert(u.busy()&&u.model.index!=saved);
 }
 printf("PASS actual LVGL8.3.10 ASan/UBSan: compass pixels/no idle draw or model RNG use/stable heap, Home synthetic flip one draw/Instant/contact, compass restart/child-root-owner deletion, both modes/all generated cards, overflow, wholeface/timing/handoff, skip/navigation/deletion/dtor, lowmem/allocfail, pointer hold/drag/outside, background hold and independent inertial-scroll gesture suppression, 100 cycles, rollover; card-only 6s+5s timing/tap-not-drag/hidden hit targets/stale focus/generated-count boundary post-settle flips/timerfail; caller allocations=%u outstanding=%u LVGL_peak=%u limit=49152 renderer_bytes=%zu HOST SIMULATION NOT DEVICE FPS\n",allocations,outstanding,heapPeak,v3app::WindowReveal::bytes);
}
