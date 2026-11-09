// Actual LVGL + Ui/delegated CoachUi + raw Imu + extracted sketch loop + chime.
#define TRAILGUIDE_CHIME_HOST
#include "../trailguide_v3/ui.h"
#include "../trailguide_v3/chime.h"
#include "../trailguide_v3/motion_diagnostics.h"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>
struct Bus {
 std::array<uint8_t,256> r{};unsigned address=0,reg=0,cursor=0,left=0,phase=0;int shortReg=-1;
 void beginTransmission(uint8_t a){address=a;phase=0;}
 size_t write(uint8_t v){if(phase++==0)reg=v;else r[reg]=v;return 1;}
 int endTransmission(bool=true){return address==0x6b?0:2;}
 size_t requestFrom(uint8_t,size_t n){cursor=reg;left=int(reg)==shortReg?0:n;return left;}
 int available(){return int(left);}int read(){--left;return r[cursor++];}
} bus;
struct Transport {
 std::string rx,tx;size_t pos=0;int capacity=0;size_t partial=3;
 int available(){return int(rx.size()-pos);}int read(){return pos<rx.size()?rx[pos++]:-1;}
 int availableForWrite(){return capacity;}
 size_t write(const uint8_t*p,size_t n){assert(n<=size_t(capacity));n=n<partial?n:partial;tx.append(reinterpret_cast<const char*>(p),n);return n;}
 void setTxTimeoutMs(uint32_t){} explicit operator bool()const{return true;}
 void printf(const char*,...){}
} Serial;
static unsigned seed=7;
static uint32_t rng(void*){return seed=seed*1664525u+1013904223u;}
static v3app::Ui ui(rng,nullptr,malloc,free);
static v3::Imu<Bus> imu(bus);
static v3app::MotionDiagnostics motionDiagnostics;
static bool ready=true,ownerIsCoach=false;static uint32_t clockMs=0;
static uint32_t millis(){return clockMs;}
static void Lvgl_Loop(){lv_timer_handler();}
static void loopSlice(){
#include "r3_loop_generated.h"
}
static lv_color_t buffer[360*20];
static void flush(lv_disp_drv_t*d,const lv_area_t*,lv_color_t*){lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);clockMs+=10;lv_timer_handler();}}
static void raw(unsigned axis,int value){auto b=uint16_t(value);bus.r[0x35+axis*2]=uint8_t(b);bus.r[0x36+axis*2]=uint8_t(b>>8);}
static void poll(int x=0,int z=2048){raw(0,x);raw(1,0);raw(2,z);lv_tick_inc(20);clockMs+=20;loopSlice();}
static void quiet(unsigned n=30,int z=2048){while(n--)poll(0,z);}
static void burst(unsigned n=60){for(unsigned i=0;i<n;++i)poll(i%2?2458:-2458);}
static void draw(bool coach){if(coach)ui.coach.perform(v3app::CoachUi::Action::Another);else ui.perform(v3app::Ui::Action::Ask);}
static void service(){loopSlice();}
int main(int argc,char**argv){
 setbuf(stdout,nullptr);assert(argc>=3);std::string mode=argv[1];bool coach=std::atoi(argv[2]);ownerIsCoach=coach;
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);
 assert(!ui.sound.enabled&&ui.sound.sequence==0);
 ui.render();if(coach)ui.perform(v3app::Ui::Action::Coaching);
 bus.r[0]=5;bus.r[1]=124;bus.r[0x2e]=3;raw(2,2048);imu.begin(0);for(unsigned t=0;t<10;++t)imu.tick(t);
 if(mode=="navigation"){
  ui.sound.enabled=true;draw(coach);advance(6000);assert(ui.sound.sequence==1&&!ui.busy());
  if(coach)ui.coach.perform(v3app::CoachUi::Action::Guide);else ui.perform(v3app::Ui::Action::Guide);
  service();assert(sdk::spawns==0&&"Published old cue must be invalidated before adapter accepts after navigation");
  draw(coach);advance(6000);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();service();assert(sdk::spawns==1);
 }else if(mode=="replacement"){
  ui.sound.enabled=true;draw(coach);advance(6000);assert(ui.sound.sequence==1);
  draw(coach);service();assert(sdk::spawns==0&&"New draw must invalidate already-published unaccepted old cue");advance(6000);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();
 }else if(mode=="instant"){
  ui.sound.enabled=true;ui.instant=true;ui.coach.instant=true;draw(coach);assert(ui.sound.sequence==1&&!ui.busy());
  if(coach)ui.coach.perform(v3app::CoachUi::Action::Guide);else ui.perform(v3app::Ui::Action::Guide);
  service();assert(sdk::spawns==0);draw(coach);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();
 }else if(mode=="switch"){
  ui.sound.enabled=true;draw(coach);advance(6000);assert(ui.sound.sequence==1);
  if(coach)ui.coach.perform(v3app::CoachUi::Action::Classic);else ui.perform(v3app::Ui::Action::Coaching);
  service();assert(sdk::spawns==0);draw(!coach);advance(6000);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();
 }else if(mode=="gesture"||mode=="gesture-instant"){
  const bool instant=mode=="gesture-instant";ui.instant=instant;ui.coach.instant=instant;
  ui.sound.enabled=true;draw(coach);advance(6000);assert(ui.sound.sequence==1);
  bool useFlip=argc>3&&std::atoi(argv[3]);
  // UI-level publication/acceptance gap: the actual owner consumes fresh motion,
  // then the real extracted loop performs audio acceptance (no synthetic cue).
  auto sample=[&](float x,float z){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=clockMs;s.acc_g[0]=x;s.acc_g[2]=z;ui.sample(s,clockMs,true);};
  for(unsigned i=0;i<45;++i)sample(0,1);
  if(useFlip){for(unsigned i=0;i<11;++i)sample(0,-1);for(unsigned i=0;i<11;++i)sample(0,1);}
  else for(unsigned i=0;i<46;++i)sample(i%2?1.2f:-1.2f,1);
  assert(ui.busy()==!instant&&ui.sound.sequence==(instant?2u:1u)&&sdk::spawns==0);service();assert(sdk::spawns==(instant?1:0));
  advance(6000);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();service();assert(sdk::spawns==1);
 }else if(mode=="wrap"){
  ui.sound.enabled=true;ui.sound.sequence=0xfffffffeu;draw(coach);advance(6000);assert(ui.sound.sequence==0xffffffffu);service();assert(sdk::spawns==1);sdk::execute();
  draw(coach);advance(6000);assert(ui.sound.sequence==0);draw(coach);service();assert(sdk::spawns==1);
  advance(6000);service();assert(ui.sound.sequence==1&&sdk::spawns==2);sdk::execute();service();assert(sdk::spawns==2);
 }else if(mode=="identity"){
  ui.sound.enabled=true;draw(coach);advance(6000);assert(ui.sound.sequence==1);
  {v3app::CoachUi other(rng,nullptr,malloc,free);other.sound=&ui.sound;other.enterCompass();other.render();}
  service();assert(sdk::spawns==1&&"Unrelated owner must not invalidate the fresh pending event");
  sdk::execute();assert(v3audio::diagnostics.state==v3audio::Done);
 }else if(mode=="playing"){
  ui.sound.enabled=true;draw(coach);advance(6000);service();assert(sdk::spawns==1);
  draw(coach);service();assert(v3audio::cancelRequested==0&&sdk::spawns==1);
  sdk::onWrite=[](){sdk::onWrite=nullptr;assert(v3audio::diagnostics.state==v3audio::Playing);if(ownerIsCoach)draw(true);draw(ownerIsCoach);service();assert(v3audio::cancelRequested==0&&sdk::spawns==1);};
  sdk::execute();assert(v3audio::diagnostics.state==v3audio::Done);
  advance(6000);service();assert(sdk::spawns==2);sdk::execute();
 }else if(mode=="fault"||mode=="notready"||mode=="muted"||mode=="partial"||mode=="unread"){
  if(mode=="partial"||mode=="unread"){Serial.rx="MOTION\n";Serial.capacity=mode=="partial"?7:0;}
  bool recording=argc>4&&std::atoi(argv[4]);if(recording){motionDiagnostics.recorder.allocator(malloc,free);assert(motionDiagnostics.recorder.start(clockMs,77));}
  ui.sound.enabled=mode!="muted";quiet();burst(35);
  if(mode=="fault"){
   unsigned fault=argc>3?std::atoi(argv[3]):0;auto retained=imu.sample().at_ms;
   if(fault<2)bus.shortReg=fault==0?0x2e:0x35;
   else if(fault<4)for(unsigned i=0x35;i<=0x40;++i)bus.r[i]=fault==2?0:255;
   else raw(fault-4,32767);
   lv_tick_inc(20);clockMs+=20;loopSlice();assert((imu.last_poll().flags&(8|32|64))&& !imu.sample().valid&&imu.sample().at_ms==retained);
   clockMs+=5;lv_tick_inc(5);loopSlice();bus.shortReg=-1;for(unsigned a=0;a<6;++a)raw(a,0);
   burst(100);assert(ui.sound.sequence==0&&sdk::spawns==0&&!ui.busy());quiet();burst();
  }else{bus.r[0x2e]=0;poll(2458);assert(imu.last_poll().flags&16);bus.r[0x2e]=3;burst(35);}
  assert(ui.busy());advance(6000);service();assert(ui.sound.sequence==(mode=="muted"?0u:1u));assert(sdk::spawns==(mode=="muted"?0:1));if(sdk::task)sdk::execute();
  if(recording){assert(motionDiagnostics.recorder.ram.count>0);assert(Serial.pos==0);assert(Serial.tx.empty());}
  if(mode=="partial"){
   for(unsigned i=0;i<1000&&Serial.tx.find('\n')==std::string::npos;++i)poll();
   assert(Serial.pos==Serial.rx.size());assert(Serial.tx.find("TG_MOTION1")!=std::string::npos&&Serial.tx.find('\n')!=std::string::npos);
   assert(ui.sound.sequence==1&&sdk::spawns==1);
  }
  else assert(Serial.tx.empty());
 }else if(mode=="deadline"){
  // Last 20ms completes a real shake in the SAME loop as the old LVGL deadline.
  ui.sound.enabled=true;draw(coach);quiet();advance(4060);quiet(21);burst(45);assert(ui.busy()&&ui.sound.sequence==0&&sdk::spawns==0);
  poll(2458);printf("deadline state busy=%d sequence=%u tasks=%d now=%u sample=%u\n",ui.busy(),ui.sound.sequence,sdk::spawns,clockMs,imu.sample().at_ms);assert(ui.busy()&&ui.sound.sequence==1&&sdk::spawns==0&&"Timer publication superseded by raw shake before audio service");advance(6000);service();assert(ui.sound.sequence==2&&sdk::spawns==1);sdk::execute();
 }else{assert(false);}
 if(ui.coach.root)lv_obj_del(ui.coach.root);
 if(ui.root)lv_obj_del(ui.root);
 printf("PASS real composition %s owner=%s sequence=%u tasks=%d completed=%u HOST ONLY\n",mode.c_str(),coach?"delegated CoachUi":"Ui",ui.sound.sequence,sdk::spawns,v3audio::diagnostics.completed.load());
}
