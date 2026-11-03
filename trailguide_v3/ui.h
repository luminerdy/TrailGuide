#pragma once
#include <lvgl.h>
#include "model.h"
#include "gesture.h"
#include "window_reveal.h"
#include "cards_generated.h"
namespace v3app {
static constexpr const char *version="v3-0.1.5";
// Bounded read-only DRAFT. No update, wake/dim, storage, accounts or networking.
class Ui {
 public:
 enum class Action { Ask, Try, Home, Guide, Another, Back, More, Instant, Options, Count };
 Model model;WindowReveal window;
 lv_obj_t *root=nullptr,*first=nullptr,*second=nullptr,*backButton=nullptr,*moreButton=nullptr,*toggle=nullptr,*scroll=nullptr,*body=nullptr,*motionStatus=nullptr;
 bool instant=false,fallback=false;
 Ui(RandomWord r,void *c,WindowReveal::Allocate a,WindowReveal::Release f):model(r,c),window(a,f){}
 Ui(const Ui&)=delete;Ui &operator=(const Ui&)=delete;
 ~Ui(){cancel();if(root)lv_obj_del(root);}
 bool busy()const{return timer!=nullptr;}
 bool eligible()const{return root && (model.view==View::Home || model.view==View::Card) && !optionsOpen && !busy() && !pointerDown && !scrolling;}
 struct Diagnostic {
  bool pointer=false,scrolling=false,options=false,reveal=false,eligible=false,root=false,healthy=false;
  unsigned view=0;Gesture::Diagnostic gesture;
  uint32_t key()const{return unsigned(pointer)|(unsigned(scrolling)<<1)|(unsigned(options)<<2)|(unsigned(reveal)<<3)|(unsigned(eligible)<<4)|(unsigned(root)<<5)|(unsigned(healthy)<<6)|(view<<7)|(gesture.phase<<10)|(unsigned(gesture.cooling)<<14);}
 };
 Diagnostic diagnostic(const v3::ImuSample &s,uint32_t now)const{
  Diagnostic d;d.pointer=pointerDown;d.scrolling=scrolling;d.options=optionsOpen;d.reveal=busy();d.eligible=eligible();d.root=root!=nullptr;d.healthy=motionAvailable;d.view=unsigned(model.view);d.gesture=gesture.diagnostic(s,now);return d;
 }
 void sample(const v3::ImuSample &s,uint32_t now,bool available,bool *triggered=nullptr){
  const bool healthy=available && s.valid && uint32_t(now-s.at_ms)<=60;
  if(healthy!=motionAvailable){motionAvailable=healthy;updateStatus();}
  // The application loop is faster than the IMU poll. Dispatch each sample
  // once, but cancel immediately on lost health/context even on repeated data.
  const bool freshFrame=!sampleSeen || s.at_ms!=lastSampleAt;
  sampleSeen=true;lastSampleAt=s.at_ms;
  if(!healthy || !eligible()){gesture.consume(s,now,false);return;}
  if(freshFrame && gesture.consume(s,now,true)){if(triggered)*triggered=true;draw(model.view==View::Home?Mode::Ask:model.mode);}
 }
 void render(){
  cancel();gesture.reset();
  if(root)lv_obj_del(root);
  root=lv_obj_create(lv_scr_act());lv_obj_remove_style_all(root);lv_obj_set_pos(root,0,0);lv_obj_set_size(root,360,360);lv_obj_clear_flag(root,LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(root,deleted,LV_EVENT_DELETE,this);
  lv_obj_add_flag(root,LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(root,contact,LV_EVENT_ALL,this);
  lv_obj_set_style_bg_color(root,lv_color_hex(0x02070d),0);lv_obj_set_style_bg_opa(root,LV_OPA_COVER,0);lv_obj_set_style_text_color(root,lv_color_hex(0xf3f5ee),0);lv_obj_set_style_text_font(root,&lv_font_montserrat_18,0);
  if(model.view==View::Home){
   if(optionsOpen){
    label(root,"Options",130,80,120,&lv_font_montserrat_20);
    backButton=button(Action::Back,"Back",130,118,100,48);
    first=button(Action::Ask,"Ask the Team",76,180,100,56);
    second=button(Action::Try,"Try a Move",184,180,100,56);
    toggle=button(Action::Instant,instant?"Instant text: ON":"Instant text: OFF",94,252,172,48);
    return;
   }
   compass=lv_obj_create(root);lv_obj_remove_style_all(compass);lv_obj_set_pos(compass,0,0);lv_obj_set_size(compass,360,360);lv_obj_clear_flag(compass,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_PRESS_LOCK);lv_obj_add_flag(compass,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_ADV_HITTEST);
   lv_obj_add_event_cb(compass,safeCompassHit,LV_EVENT_HIT_TEST,this);
   auto &slot=slots[unsigned(Action::Options)];slot={this,Action::Options};lv_obj_add_event_cb(compass,clicked,LV_EVENT_CLICKED,&slot);lv_obj_add_event_cb(compass,contact,LV_EVENT_ALL,this);
   lv_obj_add_event_cb(compass,drawCompass,LV_EVENT_DRAW_MAIN,this);lv_obj_add_event_cb(compass,deleted,LV_EVENT_DELETE,this);
   compassState=lv_tick_get()^0x8b31fca5u;compassAngle=0;compassTarget=130;compassTicks=0;compassTimer=lv_timer_create(wander,160,this);
   return;
  }
  backButton=button(model.view==View::Guide?Action::Back:Action::Home,model.view==View::Guide?"Back":"Home",76,54,100,48);
  moreButton=button(Action::More,"More text",184,54,100,48);
  const auto &c=trailguide::cards[model.index];
  scroll=lv_obj_create(root);lv_obj_remove_style_all(scroll);lv_obj_set_pos(scroll,52,112);lv_obj_set_size(scroll,256,140);lv_obj_set_scroll_dir(scroll,LV_DIR_VER);lv_obj_set_scrollbar_mode(scroll,LV_SCROLLBAR_MODE_AUTO);
  lv_obj_set_style_width(scroll,3,LV_PART_SCROLLBAR);lv_obj_set_style_bg_color(scroll,lv_color_hex(0xc5d8e4),LV_PART_SCROLLBAR);lv_obj_set_style_bg_opa(scroll,LV_OPA_COVER,LV_PART_SCROLLBAR);
  lv_obj_add_event_cb(scroll,deleted,LV_EVENT_DELETE,this);
  lv_obj_add_event_cb(scroll,contact,LV_EVENT_ALL,this);
  const bool main=model.view==View::Card;
  if(main)lv_obj_add_event_cb(scroll,drawWindow,LV_EVENT_DRAW_MAIN,this);
  body=label(scroll,main?(model.mode==Mode::Ask?c.ask:c.action):(model.mode==Mode::Ask?c.askGuide:c.tryGuide),8,8,240,main?&lv_font_montserrat_20:&lv_font_montserrat_18);
  lv_obj_add_event_cb(body,deleted,LV_EVENT_DELETE,this);
  if(main){first=button(Action::Guide,"Guide me",76,260,100,48);second=button(Action::Another,"Another",184,260,100,48);}
  else {first=button(Action::Home,"Home",76,260,100,48);second=button(Action::Back,"Back",184,260,100,48);}
  motionStatus=label(root,"",104,310,152,&lv_font_montserrat_14);updateStatus();lv_obj_update_layout(root);
 }
 void perform(Action a){
  gesture.reset();
  if(a==Action::Options){optionsOpen=true;render();return;}
  if(a==Action::Home || (a==Action::Back && optionsOpen)){optionsOpen=false;model.home();render();return;}
  if(a==Action::Ask || a==Action::Try)optionsOpen=false;
  if(a==Action::Another && busy()){finish(true);return;}
  if(a==Action::Ask){draw(Mode::Ask);return;}
  if(a==Action::Try){draw(Mode::Try);return;}
  if(a==Action::Another){if(model.view==View::Card)draw(model.mode);return;}
  if(a==Action::More){finish();if(scroll){lv_obj_update_layout(scroll);int bottom=lv_obj_get_scroll_bottom(scroll);lv_obj_scroll_to_y(scroll,bottom<=2?0:lv_obj_get_scroll_y(scroll)+100,LV_ANIM_OFF);}return;}
  switch(a){case Action::Home:model.home();break;case Action::Guide:model.guide();break;case Action::Back:model.back();break;case Action::Instant:instant=!instant;break;default:break;}
  render();
 }
 private:
 struct Slot{Ui *owner;Action action;}slots[unsigned(Action::Count)]{};
 bool optionsOpen=false;
 lv_obj_t *compass=nullptr;lv_timer_t *compassTimer=nullptr;uint32_t compassState=0;unsigned compassTicks=0;float compassAngle=0,compassTarget=0;
 // Readable hold is separate from reveal busy: completed flips remain eligible.
 lv_timer_t *focusTimer=nullptr;bool focused=false,dragged=false;lv_point_t touchOrigin{};
 lv_timer_t *timer=nullptr;uint32_t started=0;bool motionAvailable=false,pointerDown=false,scrolling=false;Gesture gesture;
 bool sampleSeen=false;uint32_t lastSampleAt=0;
 void stopCompass(){if(compassTimer){lv_timer_del(compassTimer);compassTimer=nullptr;}}
 static void safeCompassHit(lv_event_t *e){auto *h=lv_event_get_hit_test_info(e);const int32_t x=int32_t(h->point->x)-180,y=int32_t(h->point->y)-180;if(x*x+y*y>168*168)h->res=false;}
 static void wander(lv_timer_t *t){auto *u=static_cast<Ui*>(t->user_data);
  const auto old=u->needleArea();
  if(++u->compassTicks%12==0){u->compassState=u->compassState*1664525u+1013904223u;u->compassTarget+=float(70+u->compassState%160)*(u->compassState&256?1:-1);}
  u->compassAngle+=(u->compassTarget-u->compassAngle)*.12f;
  // Bound the angle over long idle sessions; no model RNG or per-tick allocation.
  if(u->compassAngle>720 || u->compassAngle< -720){float shift=u->compassAngle>0?360:-360;u->compassAngle-=shift;u->compassTarget-=shift;}
  auto area=u->needleArea();area.x1=LV_MIN(area.x1,old.x1);area.y1=LV_MIN(area.y1,old.y1);area.x2=LV_MAX(area.x2,old.x2);area.y2=LV_MAX(area.y2,old.y2);lv_obj_invalidate_area(u->compass,&area);
 }
 lv_area_t needleArea()const{lv_area_t a;lv_obj_get_coords(compass,&a);float r=compassAngle*.01745329252f;int x=a.x1+180,y=a.y1+180;int px=x-std::sin(r)*72,py=y+std::cos(r)*72,qx=x+std::sin(r)*92,qy=y-std::cos(r)*92;return {lv_coord_t(LV_MIN(px,qx)-18),lv_coord_t(LV_MIN(py,qy)-18),lv_coord_t(LV_MAX(px,qx)+18),lv_coord_t(LV_MAX(py,qy)+18)};}
 static void drawCompass(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));auto *ctx=lv_event_get_draw_ctx(e);lv_area_t a;lv_obj_get_coords(lv_event_get_target(e),&a);const int x=a.x1+180,y=a.y1+180;
  lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(0x54778c);d.width=2;
  for(int deg=0;deg<360;deg+=15){float r=deg*.01745329252f;lv_point_t p={lv_coord_t(x+std::sin(r)*154),lv_coord_t(y-std::cos(r)*154)},q={lv_coord_t(x+std::sin(r)*164),lv_coord_t(y-std::cos(r)*164)};lv_draw_line(ctx,&d,&p,&q);}
  // Fixed eight-point rose: decorative, with no measured-heading claim.
  d.color=lv_color_hex(0x305365);d.width=2;
  for(int deg=0;deg<360;deg+=45){float r=deg*.01745329252f;int length=deg%90?112:140;lv_point_t tip={lv_coord_t(x+std::sin(r)*length),lv_coord_t(y-std::cos(r)*length)},c={lv_coord_t(x),lv_coord_t(y)};lv_draw_line(ctx,&d,&c,&tip);for(int side=-1;side<=1;side+=2){float s=r+side*1.5707963f;lv_point_t wing={lv_coord_t(x+std::sin(s)*12),lv_coord_t(y-std::cos(s)*12)};lv_draw_line(ctx,&d,&wing,&tip);}}
  float r=u->compassAngle*.01745329252f;lv_point_t p={lv_coord_t(x-std::sin(r)*72),lv_coord_t(y+std::cos(r)*72)},q={lv_coord_t(x+std::sin(r)*92),lv_coord_t(y-std::cos(r)*92)},c={lv_coord_t(x),lv_coord_t(y)};
  d.width=8;d.round_start=d.round_end=1;d.color=lv_color_hex(0x83b4ca);lv_draw_line(ctx,&d,&p,&c);d.color=lv_color_hex(0xf2bb62);lv_draw_line(ctx,&d,&c,&q);
  // Arrow shoulders distinguish the wandering needle from a clock hand.
  for(int side=-1;side<=1;side+=2){float s=r+side*.48f;lv_point_t wing={lv_coord_t(q.x-std::sin(s)*24),lv_coord_t(q.y+std::cos(s)*24)};d.width=3;lv_draw_line(ctx,&d,&q,&wing);}
 }
 void draw(Mode mode){model.draw(mode);render();if(!instant)start();else hold();}
 void updateStatus(){if(motionStatus)lv_label_set_text(motionStatus,model.mode==Mode::Ask?(motionAvailable?"ASK DRAFT | flip":"ASK DRAFT | touch"):(motionAvailable?"TRY DRAFT | flip":"TRY DRAFT | touch"));}
 void caption(const char *s){if(second){auto *c=lv_obj_get_child(second,0);if(c){lv_label_set_text(c,s);lv_obj_center(c);}}}
 void chrome(bool hide){focused=hide;lv_obj_t *controls[]={backButton,moreButton,first,second,motionStatus};for(auto *o:controls)if(o){if(hide)lv_obj_add_flag(o,LV_OBJ_FLAG_HIDDEN);else lv_obj_clear_flag(o,LV_OBJ_FLAG_HIDDEN);}}
 void stopFocus(){if(focusTimer){lv_timer_del(focusTimer);focusTimer=nullptr;}chrome(false);}
 static void focusDone(lv_timer_t *t){auto *u=static_cast<Ui*>(t->user_data);u->stopFocus();}
 // Hide only after a deadline callback exists; allocation failure leaves access intact.
 void hold(){stopFocus();lv_mem_monitor_t m;lv_mem_monitor(&m);if(m.free_biggest_size<sizeof(lv_timer_t)+64)return;focusTimer=lv_timer_create(focusDone,5000,this);if(focusTimer)chrome(true);}
 void cancel(){stopFocus();stopCompass();if(timer){lv_timer_del(timer);timer=nullptr;}window.clear();if(body)lv_obj_clear_flag(body,LV_OBJ_FLAG_HIDDEN);if(scroll){lv_obj_add_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);lv_obj_invalidate(scroll);}}
 void finish(bool readableHold=false){cancel();if(model.view==View::Card){caption("Another");if(readableHold)hold();}}
 void start(){lv_obj_update_layout(scroll);fallback=!window.capture(scroll);if(fallback){hold();return;}started=lv_tick_get();lv_obj_add_flag(body,LV_OBJ_FLAG_HIDDEN);lv_obj_clear_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);timer=lv_timer_create(tick,40,this);if(!timer){fallback=true;cancel();hold();return;}chrome(true);caption("Reveal all");lv_obj_invalidate(scroll);}
 static void tick(lv_timer_t *t){auto *u=static_cast<Ui*>(t->user_data);auto elapsed=lv_tick_elaps(u->started);if(elapsed>=6000){u->finish(true);return;}u->window.render(elapsed);lv_obj_invalidate(u->scroll);}
 static void drawWindow(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));lv_area_t a;lv_obj_get_coords(lv_event_get_target(e),&a);auto *ctx=lv_event_get_draw_ctx(e);if(u->window.active())u->window.draw(ctx,a);else WindowReveal::face(ctx,a,0);}
 static void contact(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));auto *in=lv_indev_get_act();switch(lv_event_get_code(e)){
  case LV_EVENT_PRESSED:u->pointerDown=true;u->dragged=false;if(in)lv_indev_get_point(in,&u->touchOrigin);u->gesture.reset();break;
  case LV_EVENT_PRESSING:if(in){lv_point_t p;lv_indev_get_point(in,&p);if(std::abs(int(p.x)-u->touchOrigin.x)>12 || std::abs(int(p.y)-u->touchOrigin.y)>12)u->dragged=true;}break;
  case LV_EVENT_PRESS_LOST:u->dragged=true;u->pointerDown=false;u->gesture.reset();break;
  case LV_EVENT_RELEASED:u->pointerDown=false;u->gesture.reset();break;
  case LV_EVENT_SCROLL_BEGIN:u->dragged=true;u->scrolling=true;u->gesture.reset();break;
  case LV_EVENT_SCROLL_END:u->scrolling=false;u->gesture.reset();break;
  case LV_EVENT_CLICKED:if(u->focused && !u->dragged && !u->scrolling && u->model.view==View::Card){if(u->busy())u->finish(true);else u->stopFocus();}break;
  default:break;}}
 static void deleted(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));auto *o=lv_event_get_target(e);if(o==u->root){u->cancel();u->gesture.reset();u->root=nullptr;u->compass=nullptr;u->first=u->second=u->backButton=u->moreButton=u->toggle=u->body=u->scroll=u->motionStatus=nullptr;u->pointerDown=false;u->scrolling=false;return;}if(o==u->compass){u->stopCompass();u->compass=nullptr;}if(o==u->motionStatus){u->motionStatus=nullptr;u->cancel();}if(u->second && lv_obj_get_parent(o)==u->second)u->cancel();if(o==u->body){u->body=nullptr;u->cancel();}if(o==u->scroll){u->scroll=nullptr;u->cancel();}if(o==u->second){u->second=nullptr;u->cancel();}if(o==u->first){u->first=nullptr;u->cancel();}if(o==u->backButton){u->backButton=nullptr;u->cancel();}if(o==u->moreButton){u->moreButton=nullptr;u->cancel();}if(o==u->toggle){u->toggle=nullptr;u->cancel();}}
 lv_obj_t *label(lv_obj_t *p,const char *s,int x,int y,int w,const lv_font_t *font){auto *o=lv_label_create(p);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_clear_flag(o,LV_OBJ_FLAG_CLICKABLE);lv_obj_set_pos(o,x,y);lv_obj_set_width(o,w);lv_label_set_long_mode(o,LV_LABEL_LONG_WRAP);lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_line_space(o,4,0);lv_label_set_text(o,s);return o;}
 lv_obj_t *button(Action a,const char *s,int x,int y,int w,int h){auto *o=lv_btn_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);lv_obj_clear_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_PRESS_LOCK);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);lv_obj_set_style_bg_color(o,lv_color_hex(a==Action::Ask?0x27689f:0x285066),0);lv_obj_set_style_bg_color(o,lv_color_hex(0x407994),LV_STATE_PRESSED);lv_obj_set_style_radius(o,8,0);lv_obj_set_style_outline_width(o,2,LV_STATE_FOCUSED);lv_obj_set_style_outline_pad(o,0,LV_STATE_FOCUSED);lv_obj_set_style_outline_color(o,lv_color_hex(0xffffff),LV_STATE_FOCUSED);auto *text=label(o,s,0,0,w-8,&lv_font_montserrat_16);lv_obj_set_style_text_align(text,LV_TEXT_ALIGN_CENTER,0);lv_obj_center(text);auto &slot=slots[unsigned(a)];slot={this,a};lv_obj_add_event_cb(o,clicked,LV_EVENT_CLICKED,&slot);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_add_event_cb(o,contact,LV_EVENT_ALL,this);return o;}
 static void clicked(lv_event_t *e){auto *s=static_cast<Slot*>(lv_event_get_user_data(e));s->owner->perform(s->action);if(auto *in=lv_indev_get_act())lv_indev_wait_release(in);}
};
}
