// Actual Ui::sample/Gesture/model/LVGL; synthetic unless private CSV supplied.
// No captured sensor vectors are embedded in this public test.
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
struct Frame { v3::ImuSample sample;uint32_t now; };
static lv_color_t pixels[360*10];
static void flush(lv_disp_drv_t *d,const lv_area_t *,lv_color_t *){lv_disp_flush_ready(d);}
static uint32_t randomWord(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static Frame make(uint32_t t,float x,float y,float z){Frame f{};f.now=f.sample.at_ms=t;f.sample.valid=true;f.sample.acc_g[0]=x;f.sample.acc_g[1]=y;f.sample.acc_g[2]=z;f.sample.gyr_dps[0]=2.2f;return f;}
int main(int argc,char **argv){
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,pixels,nullptr,360*10);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);
 float origin[3]={0,0,1};std::vector<Frame> frames;
 if(argc==2){
  std::ifstream file(argv[1]);assert(file);size_t count=0;
  file>>origin[0]>>origin[1]>>origin[2]>>count;
  for(size_t i=0;i<count;++i){Frame f{};unsigned valid;
   file>>f.now>>f.sample.at_ms>>valid;
   for(float &v:f.sample.acc_g)file>>v;
   for(float &v:f.sample.gyr_dps)file>>v;
   assert(file);f.sample.valid=valid;frames.push_back(f);
  }
  std::string extra;assert(!(file>>extra));assert(frames.size()==count);
 }else{
  assert(argc==1);
  // SYNTHETIC near-opposed held data, not a physical/captured trace.
  for(uint32_t t=2020;t<=7020;t+=40)frames.push_back(make(t,.025f,.05f,-.99f));
 }
 assert(frames.size()>4 && frames.front().sample.at_ms>1000);
 uint32_t seed=9,expectedSeed=9;v3app::Model expected(randomWord,&expectedSeed);
 v3app::Ui ui(randomWord,&seed,malloc,free);ui.render();unsigned triggers=0;
 auto feed=[&](const Frame &f){bool trigger=false;ui.sample(f.sample,f.now,true,&trigger);triggers+=trigger;
  // Actual UI must ignore healthy duplicate dispatch, not add hold samples.
  const auto beforeDuplicate=ui.diagnostic(f.sample,f.now).gesture;
  bool duplicate=false;ui.sample(f.sample,f.now,true,&duplicate);assert(!duplicate);
  const auto afterDuplicate=ui.diagnostic(f.sample,f.now).gesture;
  assert(afterDuplicate.dwellSamples==beforeDuplicate.dwellSamples && afterDuplicate.dwellMs==beforeDuplicate.dwellMs);
 };
 const auto base=frames.front().sample.at_ms-900;
 for(uint32_t t=base;t<base+800;t+=20)feed(make(t,origin[0],origin[1],origin[2]));
 const auto seeded=ui.diagnostic(frames.front().sample,frames.front().now).gesture;
 assert(seeded.phase==1);
 std::copy(seeded.origin,seeded.origin+3,origin); // Normalized seeded reference.
 ui.perform(v3app::Ui::Action::Home); // Actual UI reset into Recover.
 assert(ui.diagnostic(frames.front().sample,frames.front().now).gesture.phase==4);
 const auto before=seed;bool rearmed=false;
 for(const auto &f:frames){
  feed(f);auto g=ui.diagnostic(f.sample,f.now).gesture;
  assert(triggers==0 && seed==before && ui.model.index==-1 && !ui.busy());
  if(g.phase==1)rearmed=true;
  if(!rearmed)for(unsigned i=0;i<3;++i)assert(g.origin[i]==origin[i]);
 }
 assert(rearmed);const auto g=ui.diagnostic(frames.back().sample,frames.back().now).gesture;
 assert(g.phase==1 && !g.dwelling);
 float newOrigin[3];std::copy(g.origin,g.origin+3,newOrigin);uint32_t t=frames.back().sample.at_ms;
 auto pose=[&](float sign,unsigned ms){for(unsigned n=0;n<ms;n+=20){t+=20;feed(make(t,sign*newOrigin[0],sign*newOrigin[1],sign*newOrigin[2]));}};
 // Reacquisition isn't a card. Inversion alone and partial return aren't cards.
 pose(-1,500);assert(triggers==0 && seed==before);
 t+=20;feed(make(t,newOrigin[1],-newOrigin[0],0));assert(triggers==0);
 pose(1,100);assert(triggers==0);pose(-1,300);assert(triggers==0);
 pose(1,500);expected.draw(v3app::Mode::Ask);
 assert(triggers==1 && ui.model.view==v3app::View::Card && ui.busy());
 assert(ui.model.index==expected.index && seed==expectedSeed);
 // Motion during reveal is dropped, not queued. Settling cannot draw on its own.
 const auto drawn=seed;pose(-1,1400);pose(1,500);assert(triggers==1 && seed==drawn);
 for(unsigned n=0;n<6500;n+=20){lv_tick_inc(20);lv_timer_handler();}
 assert(!ui.busy() && triggers==1 && seed==drawn);
 // Complete cycle after fresh rest gives exactly one additional card.
 pose(1,900);pose(-1,500);pose(1,500);expected.draw(v3app::Mode::Ask);
 assert(triggers==2 && ui.model.index==expected.index && seed==expectedSeed);
 printf("PASS recovery %s: %zu rows; rearmed without draw; new whole flip exactly 1 card; busy drop/no queue; fresh-rest next cycle exactly 1 card; actual Ui/sample/LVGL ASan/UBSan; NOT PHYSICAL ACCEPTANCE\n",argc==2?"PRIVATE capture replay":"SYNTHETIC",frames.size());
}
