#pragma once
#include <lvgl.h>
#include "coaching_model.h"
#include "gesture.h"
#include "window_reveal.h"
#include <cstdio>
namespace v3app {
using trailguide_coach::CoachModel;
using trailguide_coach::Screen;

// Bounded read-only DRAFT. No update, wake/dim, storage, accounts or networking.
class CoachUi {
 public:
 enum class Action { Progress, Stuck, Unclear, Exploring, Ai, Team, Options, Back, Another, Guide, Release, Classic, Instant, Picker, Opening, NextStep, Next, Experiment, Question, Reminders, Count };
 CoachModel model;WindowReveal window;
 void (*exitCallback)(void*)=nullptr;void *exitContext=nullptr;
 bool leaveRequested=false;

 lv_obj_t *root=nullptr,*first=nullptr,*second=nullptr,*backButton=nullptr,*scroll=nullptr,*body=nullptr;
 bool instant=false,fallback=false;
 CoachUi(trailguide_coach::RandomWord r,void *c,WindowReveal::Allocate a,WindowReveal::Release f):model(r,c),window(a,f){}
 CoachUi(const CoachUi&)=delete;CoachUi &operator=(const CoachUi&)=delete;
 ~CoachUi(){cancel();if(root)lv_obj_del(root);}
 bool busy()const{return timer!=nullptr;}
 bool eligible()const{return root && model.current.screen==Screen::Opening && !optionsOpen && !busy() && !pointerDown && !scrolling;}
 struct Diagnostic {
  bool pointer=false,scrolling=false,options=false,reveal=false,eligible=false,root=false,healthy=false;
  unsigned view=0;uint32_t resetCounts[8]={};Gesture::Diagnostic gesture;
  uint32_t key()const{return unsigned(pointer)|(unsigned(scrolling)<<1)|(unsigned(options)<<2)|(unsigned(reveal)<<3)|(unsigned(eligible)<<4)|(unsigned(root)<<5)|(unsigned(healthy)<<6)|(view<<7)|(gesture.phase<<10)|(unsigned(gesture.cooling)<<14);}
 };
 Diagnostic diagnostic(const v3::ImuSample &s,uint32_t now)const{
  Diagnostic d;for(unsigned i=0;i<8;++i)d.resetCounts[i]=resetCounts[i];d.pointer=pointerDown;d.scrolling=scrolling;d.options=optionsOpen;d.reveal=busy();d.eligible=eligible();d.root=root!=nullptr;d.healthy=motionAvailable;d.view=model.current.screen==Screen::Opening?0:2;d.gesture=gesture.diagnostic(s,now);return d;
 }
 void sample(const v3::ImuSample &s,uint32_t now,bool available,bool *triggered=nullptr){
  const bool healthy=available && s.valid && uint32_t(now-s.at_ms)<=60;
  motionAvailable=healthy;
  // The application loop is faster than the IMU poll. Dispatch each sample
  // once, but cancel immediately on lost health/context even on repeated data.
  const bool freshFrame=!sampleSeen || s.at_ms!=lastSampleAt;
  sampleSeen=true;lastSampleAt=s.at_ms;
  if(!healthy || !eligible()){gesture.consume(s,now,false);setBlank(false);return;}
  if(freshFrame && gesture.consume(s,now,true)){if(triggered)*triggered=true;draw();}
  else setBlank(gesture.qualifiedInverted());
 }
 void render(){
  cancel();++resetCounts[0];gesture.reset();
  if(root)lv_obj_del(root);
  root=lv_obj_create(lv_scr_act());lv_obj_remove_style_all(root);lv_obj_set_pos(root,0,0);lv_obj_set_size(root,360,360);lv_obj_clear_flag(root,LV_OBJ_FLAG_SCROLLABLE);lv_obj_add_flag(root,LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_bg_color(root,lv_color_hex(0x02070d),0);lv_obj_set_style_bg_opa(root,LV_OPA_COVER,0);lv_obj_set_style_text_color(root,lv_color_hex(0xf3f5ee),0);
  lv_obj_add_event_cb(root,deleted,LV_EVENT_DELETE,this);lv_obj_add_event_cb(root,contact,LV_EVENT_ALL,this);lv_obj_add_event_cb(root,drawBlank,LV_EVENT_DRAW_MAIN,this);
  auto screen=model.current.screen;optionsOpen=screen==Screen::Options;
  if(screen==Screen::Opening || screen==Screen::Picker){
   const int xy[6][2]={{142,24},{264,94},{264,218},{142,288},{40,218},{40,94}};
   const char *shortLabels[]={"Progress","Stuck","Unclear","Explore","AI","Team"};
   for(int i=0;i<6;++i){auto *o=button(Action(i),shortLabels[i],xy[i][0],xy[i][1],i==0||i==3?76:56,48);lv_obj_set_style_bg_opa(o,LV_OPA_TRANSP,0);lv_obj_set_style_radius(o,4,0);if(model.current.path==i){lv_obj_set_style_border_width(o,1,0);lv_obj_set_style_border_color(o,lv_color_hex(0xf2bb62),0);}lv_obj_add_event_cb(o,icon,LV_EVENT_DRAW_MAIN,&slots[i]);}
  }
  if(screen==Screen::Options){
   backButton=button(Action::Back,"Back",76,54,100,48);
   first=button(Action::Another,"Another opener",76,112,208,48);
   second=button(Action::Guide,"Guide me",76,164,100,48);
   button(Action::Release,"Release",184,164,100,48);
   button(Action::Classic,"Classic",76,216,100,48);
   button(Action::Instant,instant?"Instant ON":"Instant OFF",184,216,100,48);
   button(Action::Reminders,"Reminders",112,274,136,48);return;
  }
  if(screen==Screen::Picker){
   label(root,"Choose a path",112,84,136,&lv_font_montserrat_14);
   button(Action::Opening,"Opening",128,118,104,48);button(Action::NextStep,"Next step",128,174,104,48);button(Action::Back,"Back",128,230,104,48);return;
  }
  const bool opening=screen==Screen::Opening;
  if(!opening){if(screen==Screen::Question || screen==Screen::Experiment){backButton=button(Action::Picker,"Paths",70,58,70,48);button(Action::Guide,"Guide me",145,58,70,48);button(Action::Release,"Release",220,58,70,48);}else{backButton=button(Action::Back,"Back",76,54,100,48);button(Action::Release,"Release",184,54,100,48);}}
  if(model.current.path>=0){label(root,trailguide_coaching::pathLabels[model.current.path],76,108,208,&lv_font_montserrat_14);}else if(!opening)label(root,screen==Screen::Reminders?"Private listening cues":screen==Screen::Closing?"Next step - optional":"Guide me - optional",76,108,208,&lv_font_montserrat_14);
  if(opening)backButton=button(Action::Options,"Options",130,72,100,48);
  const int x=opening?52:58,y=opening?112:126,w=opening?256:244,h=opening?140:104;
  scroll=lv_obj_create(root);lv_obj_remove_style_all(scroll);lv_obj_set_pos(scroll,x,y);lv_obj_set_size(scroll,w,h);lv_obj_set_scroll_dir(scroll,LV_DIR_VER);lv_obj_set_scrollbar_mode(scroll,LV_SCROLLBAR_MODE_AUTO);lv_obj_add_event_cb(scroll,deleted,LV_EVENT_DELETE,this);lv_obj_add_event_cb(scroll,contact,LV_EVENT_ALL,this);
  if(opening)lv_obj_add_event_cb(scroll,drawWindow,LV_EVENT_DRAW_MAIN,this);
  auto *entry=model.entry();const char *text=entry?(screen==Screen::Guide?entry->guidance:entry->text):"Choose what would help.";
  body=label(scroll,text,opening?60:4,opening?8:4,opening?136:w-8,&lv_font_montserrat_20);lv_obj_move_background(scroll);lv_obj_add_event_cb(body,contact,LV_EVENT_ALL,this);lv_obj_update_layout(root);lv_obj_scroll_to_y(scroll,model.current.scroll,LV_ANIM_OFF);
  label(root,"Scroll for full text",opening?112:76,opening?258:236,opening?136:208,&lv_font_montserrat_14);
  if(!opening){
   if(screen==Screen::Question || screen==Screen::Experiment){first=button(Action::Next,"Another",76,254,100,48);second=button(screen==Screen::Question?Action::Experiment:Action::Question,screen==Screen::Question?"Try (optional)":"Question",184,254,100,48);}
   else {first=button(Action::Opening,"Opening",76,254,100,48);second=button(screen==Screen::Guide?Action::Reminders:Action::Next,screen==Screen::Guide?"Reminders":"Next",184,254,100,48);}
  }
 }
 void perform(Action a){
  if(scroll)model.remember(lv_obj_get_scroll_y(scroll));
  ++resetCounts[1];gesture.reset();
  if(unsigned(a)<6){model.path(unsigned(a));render();return;}
  if(a==Action::Another){if(busy())finish(true);else draw();return;}
  switch(a){case Action::Options:model.options();break;case Action::Back:model.back();break;case Action::Guide:model.guide();break;case Action::Release:model.release();break;case Action::Classic:leaveRequested=true;cancel();if(exitCallback)exitCallback(exitContext);return;case Action::Instant:instant=!instant;break;case Action::Picker:model.picker();break;case Action::Opening:model.toOpening();break;case Action::NextStep:model.nextStep();break;case Action::Next:model.next();break;case Action::Experiment:model.experiment();break;case Action::Question:model.question();break;case Action::Reminders:model.reminders();break;default:break;}
  render();
 }

 private:
 struct Slot{CoachUi *owner;Action action;}slots[unsigned(Action::Count)]{};
 bool optionsOpen=false;
 // Existing root/children only: no overlay, snapshot, timer or heap allocation.
 bool blanked=false,touchCancelledBlank=false;uint32_t hiddenBeforeBlank=0;
 void setBlank(bool hide){
  if(!root || hide==blanked)return;
  const unsigned n=lv_obj_get_child_cnt(root);
  // Owned coaching layouts are bounded below 32 direct children.
  if(hide && n>32)return;
  if(hide){hiddenBeforeBlank=0;for(unsigned i=0;i<n;++i){auto *o=lv_obj_get_child(root,i);if(lv_obj_has_flag(o,LV_OBJ_FLAG_HIDDEN))hiddenBeforeBlank|=1u<<i;lv_obj_add_flag(o,LV_OBJ_FLAG_HIDDEN);}}
  else for(unsigned i=0;i<n && i<32;++i)if(!(hiddenBeforeBlank&(1u<<i)))lv_obj_clear_flag(lv_obj_get_child(root,i),LV_OBJ_FLAG_HIDDEN);
  blanked=hide;lv_obj_invalidate(root);
 }
 static void drawBlank(lv_event_t *e){auto *u=static_cast<CoachUi*>(lv_event_get_user_data(e));if(!u->blanked)return;lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);d.bg_color=lv_color_black();d.bg_opa=LV_OPA_COVER;lv_area_t a;lv_obj_get_coords(u->root,&a);lv_draw_rect(lv_event_get_draw_ctx(e),&d,&a);}
 // Readable hold is separate from reveal busy: completed flips remain eligible.
 lv_timer_t *focusTimer=nullptr;bool focused=false,dragged=false;lv_point_t touchOrigin{};
 lv_timer_t *timer=nullptr;uint32_t started=0;bool motionAvailable=false,pointerDown=false,scrolling=false;Gesture gesture;
 uint32_t resetCounts[8]={};
 bool sampleSeen=false;uint32_t lastSampleAt=0;
 void draw(){model.draw();render();if(!instant)start();else hold();}
 void chrome(bool hide){focused=hide;if(!root)return;for(unsigned i=0;i<lv_obj_get_child_cnt(root)&&i<32;++i){auto *o=lv_obj_get_child(root,i);if(o==scroll)continue;if(blanked){if(hide)hiddenBeforeBlank|=1u<<i;else hiddenBeforeBlank&=~(1u<<i);}else if(hide)lv_obj_add_flag(o,LV_OBJ_FLAG_HIDDEN);else lv_obj_clear_flag(o,LV_OBJ_FLAG_HIDDEN);}}

 void stopFocus(){if(focusTimer){lv_timer_del(focusTimer);focusTimer=nullptr;}chrome(false);}
 static void focusDone(lv_timer_t *t){auto *u=static_cast<CoachUi*>(t->user_data);u->stopFocus();}
 // Hide only after a deadline callback exists; allocation failure leaves access intact.
 void hold(){stopFocus();lv_mem_monitor_t m;lv_mem_monitor(&m);if(m.free_biggest_size<sizeof(lv_timer_t)+64)return;focusTimer=lv_timer_create(focusDone,5000,this);if(focusTimer)chrome(true);}
 void cancel(){setBlank(false);stopFocus();if(timer){lv_timer_del(timer);timer=nullptr;}window.clear();if(body)lv_obj_clear_flag(body,LV_OBJ_FLAG_HIDDEN);if(scroll){lv_obj_add_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);lv_obj_invalidate(scroll);}}
 void finish(bool readableHold=false){cancel();if(model.current.screen==Screen::Opening){if(readableHold)hold();}}
 void start(){lv_obj_update_layout(scroll);fallback=!window.capture(scroll);if(fallback){hold();return;}started=lv_tick_get();lv_obj_add_flag(body,LV_OBJ_FLAG_HIDDEN);lv_obj_clear_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);timer=lv_timer_create(tick,40,this);if(!timer){fallback=true;cancel();hold();return;}chrome(true);lv_obj_invalidate(scroll);}
 static void tick(lv_timer_t *t){auto *u=static_cast<CoachUi*>(t->user_data);auto elapsed=lv_tick_elaps(u->started);if(elapsed>=6000){u->finish(true);return;}u->window.render(elapsed);lv_obj_invalidate(u->scroll);}
 static void drawWindow(lv_event_t *e){auto *u=static_cast<CoachUi*>(lv_event_get_user_data(e));lv_area_t a;lv_obj_get_coords(lv_event_get_target(e),&a);auto *ctx=lv_event_get_draw_ctx(e);if(u->window.active())u->window.draw(ctx,a);else WindowReveal::face(ctx,a,0);}
 static void contact(lv_event_t *e){auto *u=static_cast<CoachUi*>(lv_event_get_user_data(e));auto *in=lv_indev_get_act();switch(lv_event_get_code(e)){
  case LV_EVENT_PRESSED:u->touchCancelledBlank=u->blanked;u->setBlank(false);u->pointerDown=true;u->dragged=false;if(in)lv_indev_get_point(in,&u->touchOrigin);++u->resetCounts[2];u->gesture.reset();break;
  case LV_EVENT_PRESSING:if(in){lv_point_t p;lv_indev_get_point(in,&p);if(std::abs(int(p.x)-u->touchOrigin.x)>12 || std::abs(int(p.y)-u->touchOrigin.y)>12)u->dragged=true;}break;
  case LV_EVENT_PRESS_LOST:u->dragged=true;u->pointerDown=false;++u->resetCounts[3];u->gesture.reset();break;
  case LV_EVENT_RELEASED:u->pointerDown=false;++u->resetCounts[4];u->gesture.reset();break;
  case LV_EVENT_SCROLL_BEGIN:u->dragged=true;u->scrolling=true;++u->resetCounts[5];u->gesture.reset();break;
  case LV_EVENT_SCROLL_END:u->scrolling=false;++u->resetCounts[6];u->gesture.reset();break;
  case LV_EVENT_CLICKED:if(u->touchCancelledBlank){u->touchCancelledBlank=false;break;}if(u->focused && !u->dragged && !u->scrolling && u->model.current.screen==Screen::Opening){if(u->busy())u->finish(true);else u->stopFocus();}break;
  default:break;}}
 static void deleted(lv_event_t *e){auto *u=static_cast<CoachUi*>(lv_event_get_user_data(e));auto *o=lv_event_get_target(e);
  // Restore before LVGL removes a child and changes the bounded mask indices.
  if(u->blanked){u->setBlank(false);u->gesture.reset();}
  if(o==u->root){u->cancel();++u->resetCounts[7];u->gesture.reset();u->root=nullptr;u->first=u->second=u->backButton=u->body=u->scroll=nullptr;u->pointerDown=false;u->scrolling=false;return;}
  if(o==u->body)u->body=nullptr;
  if(o==u->scroll)u->scroll=nullptr;
  if(o==u->first)u->first=nullptr;
  if(o==u->second)u->second=nullptr;
  if(o==u->backButton)u->backButton=nullptr;
  u->cancel();u->gesture.reset();
 }

 lv_obj_t *label(lv_obj_t *p,const char *s,int x,int y,int w,const lv_font_t *font){auto *o=lv_label_create(p);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_clear_flag(o,LV_OBJ_FLAG_CLICKABLE);lv_obj_set_pos(o,x,y);lv_obj_set_width(o,w);lv_label_set_long_mode(o,LV_LABEL_LONG_WRAP);lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_line_space(o,4,0);lv_label_set_text(o,s);return o;}
 lv_obj_t *button(Action a,const char *s,int x,int y,int w,int h){auto *o=lv_btn_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);lv_obj_clear_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_PRESS_LOCK);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);lv_obj_set_style_bg_color(o,lv_color_hex(a==Action::Progress?0x27689f:0x285066),0);lv_obj_set_style_bg_color(o,lv_color_hex(0x407994),LV_STATE_PRESSED);lv_obj_set_style_radius(o,8,0);lv_obj_set_style_outline_width(o,2,LV_STATE_FOCUSED);lv_obj_set_style_outline_pad(o,0,LV_STATE_FOCUSED);lv_obj_set_style_outline_color(o,lv_color_hex(0xffffff),LV_STATE_FOCUSED);auto *text=label(o,s,0,0,unsigned(a)<6?w:w-8,&lv_font_montserrat_14);lv_obj_set_style_text_align(text,LV_TEXT_ALIGN_CENTER,0);lv_obj_center(text);if(unsigned(a)<6){lv_obj_set_align(text,LV_ALIGN_TOP_LEFT);lv_obj_set_pos(text,0,27);}auto &slot=slots[unsigned(a)];slot={this,a};lv_obj_add_event_cb(o,clicked,LV_EVENT_CLICKED,&slot);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_add_event_cb(o,contact,LV_EVENT_ALL,this);return o;}
 static void icon(lv_event_t *e){auto *slot=static_cast<Slot*>(lv_event_get_user_data(e));auto *ctx=lv_event_get_draw_ctx(e);lv_area_t a;lv_obj_get_coords(lv_event_get_target(e),&a);int x=(a.x1+a.x2)/2,y=a.y1+13;lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(0x83b4ca);d.width=2;d.round_start=d.round_end=1;
  auto line=[&](int x1,int y1,int x2,int y2){lv_point_t p={lv_coord_t(x+x1),lv_coord_t(y+y1)},q={lv_coord_t(x+x2),lv_coord_t(y+y2)};lv_draw_line(ctx,&d,&p,&q);};
  switch(slot->action){case Action::Progress:line(-10,6,-3,0);line(-3,0,9,-6);line(3,-6,9,-6);line(9,-6,9,0);break;case Action::Stuck:line(-10,6,-3,6);line(0,-8,0,8);line(4,-8,4,8);line(7,6,11,6);break;case Action::Unclear:line(-6,-4,-3,-8);line(-3,-8,4,-8);line(4,-8,7,-4);line(7,-4,0,1);line(0,1,0,3);line(0,7,0,8);break;case Action::Exploring:line(0,8,0,-2);line(0,-2,-9,-8);line(0,-2,9,-8);line(-9,-8,-9,-3);line(9,-8,9,-3);break;case Action::Ai:line(-8,-8,5,-8);line(-8,-8,-8,8);line(-8,8,5,8);line(5,-8,5,8);line(-4,-3,1,-3);line(-4,2,1,2);line(10,-6,10,0);line(7,-3,13,-3);break;case Action::Team:line(-8,-7,-4,-7);line(4,-7,8,-7);line(-10,7,-10,0);line(-10,0,-2,0);line(-2,0,-2,7);line(2,7,2,0);line(2,0,10,0);line(10,0,10,7);break;default:break;}
 }
 static void clicked(lv_event_t *e){auto *s=static_cast<Slot*>(lv_event_get_user_data(e));s->owner->perform(s->action);if(auto *in=lv_indev_get_act())lv_indev_wait_release(in);}
};
}
