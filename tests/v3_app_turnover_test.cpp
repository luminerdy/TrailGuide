// Actual pinned LVGL framebuffer/input; invented IMU poses, not physical evidence.
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <algorithm>
static lv_color_t buffer[360*20],pixels[360*360];
static uint32_t seed=7;
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static bool black(){lv_refr_now(nullptr);return std::all_of(std::begin(pixels),std::end(pixels),[](lv_color_t p){return p.full==0;});}
static void save(const std::string &name){lv_refr_now(nullptr);static std::ofstream timeline("build/v3-0.1.21/turnover-frames/timeline.tsv");assert(timeline);timeline<<name<<".ppm\t"<<lv_tick_get()<<"\n";timeline.flush();std::ofstream f("build/v3-0.1.21/turnover-frames/"+name+".ppm",std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";for(auto p:pixels){lv_color32_t c;c.full=lv_color_to32(p);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}}
static v3::ImuSample feed(v3app::Ui &u,float z){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=z;s.gyr_dps[1]=2.2f;u.sample(s,s.at_ms,true);u.sample(s,s.at_ms+1,true);return s;}
static void pose(v3app::Ui &u,float z,unsigned n){for(unsigned i=0;i<n;i++)feed(u,z);}
static unsigned timers(){unsigned n=0;for(auto *t=lv_timer_get_next(nullptr);t;t=lv_timer_get_next(t))++n;return n;}
static lv_point_t point;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static void press(int x,int y){point={lv_coord_t(x),lv_coord_t(y)};pressed=true;advance(60);}
static void lift(){pressed=false;advance(60);}
int main(){lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);
 lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);
 const auto baseline=timers();
 {v3app::Ui u(rng,&seed,malloc,free);u.render();advance(60);save("compass-start");assert(!black());pose(u,1,40);const auto before=seed;auto *root=u.root;auto t=timers();
  pose(u,-1,5);assert(!black() && "partial inversion must not blank");pose(u,-1,5);assert(black() && "qualified inversion must be TRUE black over entire 360 frame");save("compass-inverted");assert(u.root==root && u.eligible() && seed==before && u.model.index==-1 && timers()==t);
  pose(u,1,5);assert(black() && "return must stay black before existing qualification");save("compass-return-preconfirm");
  uint32_t expectedSeed=seed;v3app::Model expected(rng,&expectedSeed);expected.draw(v3app::Mode::Ask);
  pose(u,1,5);assert(u.busy() && u.model.index==expected.index && seed==expectedSeed);advance(3000);save("compass-reveal-mid");assert(!black());advance(3000);save("compass-settled");assert(!u.busy() && u.eligible());
  // A new eligible cycle DURING the settled hold, not queued during reveal.
  save("card-start");pose(u,1,40);pose(u,-1,10);assert(black());save("card-inverted");pose(u,1,5);assert(black());save("card-return-preconfirm");pose(u,1,5);expected.draw(v3app::Mode::Ask);assert(u.busy() && u.model.index==expected.index && seed==expectedSeed);const int id=u.model.index;
  pose(u,1,40);pose(u,-1,10);pose(u,1,10);assert(u.model.index==id && seed==expectedSeed && u.busy());advance(1800);save("card-reveal-mid");assert(!black());advance(3000);save("card-settled");assert(!u.busy() && u.eligible() && lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN));advance(4990);assert(lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN));advance(50);assert(!lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN));
 }assert(timers()==baseline);
 for(int cause=0;cause<5;++cause){v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(v3app::Ui::Action::Ask);pose(u,1,40);std::string oldPixels;if(cause==0){save("card-cancel-start");oldPixels.assign(reinterpret_cast<const char*>(pixels),sizeof pixels);}pose(u,-1,10);assert(black());if(cause==0)save("card-cancel-inverted");const auto before=seed;const int id=u.model.index;auto *root=u.root;
  if(cause==0){auto s=feed(u,-1);u.sample(s,s.at_ms,false);} // duplicate timestamp health loss
  if(cause==1){auto s=feed(u,-1);s.valid=false;u.sample(s,s.at_ms,true);}
  if(cause==2){auto s=feed(u,-1);s.acc_g[0]=4;u.sample(s,s.at_ms+20,true);s.at_ms+=20;u.sample(s,s.at_ms,true);}
  if(cause==3)pose(u,-1,620); // fresh continuous cycle timeout, no extra presentation deadline
  if(cause==4){auto s=feed(u,-1);u.sample(s,s.at_ms+61,true);}
  assert(!black() && "cancellation restores prior scene, never stuck black");assert(u.model.index==id && seed==before && u.root==root && !u.busy());if(cause==0){save("card-cancel-restore");assert(oldPixels==std::string(reinterpret_cast<const char*>(pixels),sizeof pixels) && "cancel restores actual previous card pixels, not just its model ID");}
 }
 assert(timers()==baseline);
 for(int context=0;context<3;++context)for(auto p:{lv_point_t{126,78},lv_point_t{232,78},lv_point_t{126,280},lv_point_t{232,280},lv_point_t{180,106}}){v3app::Ui u(rng,&seed,malloc,free);u.render();if(context){u.instant=true;u.perform(v3app::Ui::Action::Ask);if(context==2)advance(5100);}pose(u,1,40);pose(u,-1,10);assert(black());const auto before=seed;const int id=u.model.index;auto *root=u.root;
  press(p.x,p.y);assert(!black() && "touch cancels presentation immediately without sample dispatch");assert(!u.eligible());lift();assert(u.model.index==id && seed==before && u.root==root && u.eligible() && !u.busy());if(context)assert(lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN)==(context==1) && "cancel tap must preserve chrome state, not activate restored control");
 }
 assert(timers()==baseline);
 {v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(v3app::Ui::Action::Ask);pose(u,1,40);pose(u,-1,10);assert(black());const auto before=seed;const int id=u.model.index;pose(u,-1,210);assert(black() && "focus deadline must not expose any child through black presentation");auto s=feed(u,-1);u.sample(s,s.at_ms,false);assert(!black() && !lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN) && u.model.index==id && seed==before);}
 assert(timers()==baseline);
 {v3app::Ui u(rng,&seed,malloc,free);u.render();pose(u,1,40);pose(u,-1,10);assert(black());lv_obj_del(lv_obj_get_child(u.root,0));assert(!black() && "independent compass deletion cancels black presentation");auto s=feed(u,1);assert(!black() && u.diagnostic(s,s.at_ms).gesture.phase!=3);}
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}){uint32_t expectedSeed=seed;v3app::Model expected(rng,&expectedSeed);v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(mode==v3app::Mode::Ask?v3app::Ui::Action::Ask:v3app::Ui::Action::Try);expected.draw(mode);
  for(unsigned n=0;n<2*trailguide::cardCount+2;++n){pose(u,1,170);const auto before=seed;const int id=u.model.index;pose(u,-1,10);assert(black() && seed==before && u.model.index==id);auto s=feed(u,-1);auto d=u.diagnostic(s,s.at_ms);u.sample(s,s.at_ms+1,true);assert(black() && u.diagnostic(s,s.at_ms).gesture.resets==d.gesture.resets);pose(u,1,10);expected.draw(mode);assert(!black() && !u.busy() && u.model.index==expected.index && seed==expectedSeed);}
 }
 assert(timers()==baseline);
 // Exhaust the actual LVGL heap without attempting an asserting object allocator.
 for(bool card:{false,true}){v3app::Ui u(rng,&seed,malloc,free);u.render();if(card){u.instant=true;u.perform(v3app::Ui::Action::Ask);}pose(u,1,40);lv_mem_monitor_t m;lv_mem_monitor(&m);void *block=lv_mem_alloc(m.free_biggest_size-1024);assert(block);void *fragments[256];unsigned count=0;while(count<256){void *p=lv_mem_alloc(16);if(!p)break;fragments[count++]=p;}assert(count<256);lv_mem_monitor(&m);const auto freeBefore=m.free_size;assert(m.free_biggest_size<sizeof(lv_obj_t));const auto before=seed;const auto t=timers();
  v3::ImuSample s;s.valid=true;s.acc_g[2]=-1;for(int i=0;i<10;i++){lv_tick_inc(20);s.at_ms=lv_tick_get();u.sample(s,s.at_ms,true);}lv_mem_monitor(&m);assert(m.free_size==freeBefore && timers()==t && seed==before);assert(black());
  lv_tick_inc(20);s.at_ms=lv_tick_get();s.acc_g[0]=4;u.sample(s,s.at_ms,true);lv_mem_monitor(&m);assert(m.free_size==freeBefore && timers()==t && seed==before);while(count)lv_mem_free(fragments[--count]);lv_mem_free(block);assert(!black());
 }
 for(int part=0;part<7;++part){v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(v3app::Ui::Action::Ask);pose(u,1,40);pose(u,-1,10);assert(black());lv_obj_t *objects[]={u.root,u.body,u.scroll,u.first,u.second,u.backButton,u.moreButton};lv_obj_del(objects[part]);if(u.root){auto s=feed(u,1);assert(!black() && u.diagnostic(s,s.at_ms).gesture.phase!=3);}advance(6000);}
 for(auto action:{v3app::Ui::Action::Home,v3app::Ui::Action::Guide,v3app::Ui::Action::More}){v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(v3app::Ui::Action::Ask);pose(u,1,40);pose(u,-1,10);const int id=u.model.index;const auto before=seed;u.perform(action);assert(!black() && u.model.index==id && seed==before);advance(6000);}
 {v3app::Ui u(rng,&seed,malloc,free);u.render();pose(u,1,40);pose(u,-1,10);assert(black());}assert(timers()==baseline);
 {v3app::Ui u(rng,&seed,malloc,free);u.render();u.instant=true;u.perform(v3app::Ui::Action::Ask);pose(u,1,40);pose(u,-1,10);auto s=feed(u,-1);const auto before=seed;const int id=u.model.index;u.model.guide();u.sample(s,s.at_ms+1,true);assert(black() && u.model.index==id && seed==before && u.eligible());pose(u,1,10);assert(!black() && u.model.view==v3app::View::Card && u.model.index!=id && seed==before);}
 {v3app::Ui u(rng,&seed,[](size_t)->void*{return nullptr;},free);u.render();pose(u,1,40);pose(u,-1,10);assert(black());pose(u,1,10);assert(u.fallback && !u.busy() && !black() && !lv_obj_has_flag(u.body,LV_OBJ_FLAG_HIDDEN));const auto before=seed;const int id=u.model.index;press(180,180);lift();assert(!lv_obj_has_flag(u.first,LV_OBJ_FLAG_HIDDEN) && u.model.index==id && seed==before);}
 {v3app::Ui u(rng,&seed,malloc,free);u.render();for(int i=0;i<32;++i)lv_obj_create(u.root);pose(u,1,40);pose(u,-1,10);assert(!black() && u.eligible() && "unknown oversized root must decline optional blackout rather than overrun visibility mask");}
 assert(timers()==baseline);puts("PASS turnover: full 360 black after qualified-only inversion; unchanged identity/RNG/root/eligibility/timers before return; repeated timestamps; one independent-model draw per completion across two boundaries/both modes; native 6s reveal + 5s hold/new eligible cycle during hold/cooldown drops short repeat; health/invalid/clip/timeout/age/context/navigation cancellation; real press/release at hidden controls/focus preservation; focus deadline stays black; exhausted LVGL heap zeroalloc blank/restore; Instant/renderer-null accessible fallback; child/root/destruction cleanup. SYNTHETIC HOST ONLY");
}
