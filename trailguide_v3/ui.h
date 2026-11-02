#pragma once
#include <lvgl.h>
#include "model.h"
#include "gesture.h"
#include "window_reveal.h"
#include "cards_generated.h"
namespace v3app {
static constexpr const char *version="v3-0.1.0";
// Bounded read-only DRAFT. No update, wake/dim, storage, accounts or networking.
class Ui {
 public:
 enum class Action { Ask, Try, Home, Guide, Another, Back, More, Instant };
 Model model;WindowReveal window;
 lv_obj_t *root=nullptr,*first=nullptr,*second=nullptr,*backButton=nullptr,*moreButton=nullptr,*toggle=nullptr,*scroll=nullptr,*body=nullptr,*motionStatus=nullptr;
 bool instant=false,fallback=false;
 Ui(RandomWord r,void *c,WindowReveal::Allocate a,WindowReveal::Release f):model(r,c),window(a,f){}
 Ui(const Ui&)=delete;Ui &operator=(const Ui&)=delete;
 ~Ui(){cancel();if(root)lv_obj_del(root);}
 bool busy()const{return timer!=nullptr;}
 bool eligible()const{return root && model.view==View::Card && !busy() && !pointerDown && !scrolling;}
 void sample(const v3::ImuSample &s,uint32_t now,bool available){
  const bool healthy=available && s.valid && uint32_t(now-s.at_ms)<=60;
  if(healthy!=motionAvailable){motionAvailable=healthy;updateStatus();}
  if(gesture.consume(s,now,healthy && eligible()))draw(model.mode);
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
   label(root,"TrailGuide DRAFT",90,48,180,&lv_font_montserrat_20);
   first=button(Action::Ask,"Ask the Team",76,116,208,56);
   second=button(Action::Try,"Try a Move",76,180,208,56);
   toggle=button(Action::Instant,instant?"Instant text: ON":"Instant text: OFF",94,244,172,48);
   label(root,"v3-0.1.0 | no saving",108,300,144,&lv_font_montserrat_14);
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
  if(a==Action::Another && busy()){finish();return;}
  if(a==Action::Ask){draw(Mode::Ask);return;}
  if(a==Action::Try){draw(Mode::Try);return;}
  if(a==Action::Another){if(model.view==View::Card)draw(model.mode);return;}
  if(a==Action::More){finish();if(scroll){lv_obj_update_layout(scroll);int bottom=lv_obj_get_scroll_bottom(scroll);lv_obj_scroll_to_y(scroll,bottom<=2?0:lv_obj_get_scroll_y(scroll)+100,LV_ANIM_OFF);}return;}
  switch(a){case Action::Home:model.home();break;case Action::Guide:model.guide();break;case Action::Back:model.back();break;case Action::Instant:instant=!instant;break;default:break;}
  render();
 }
 private:
 struct Slot{Ui *owner;Action action;}slots[8]{};
 lv_timer_t *timer=nullptr;uint32_t started=0;bool motionAvailable=false,pointerDown=false,scrolling=false;Gesture gesture;
 void draw(Mode mode){model.draw(mode);render();if(!instant)start();}
 void updateStatus(){if(motionStatus)lv_label_set_text(motionStatus,model.mode==Mode::Ask?(motionAvailable?"ASK DRAFT | shake":"ASK DRAFT | touch"):(motionAvailable?"TRY DRAFT | shake":"TRY DRAFT | touch"));}
 void caption(const char *s){if(second){auto *c=lv_obj_get_child(second,0);if(c){lv_label_set_text(c,s);lv_obj_center(c);}}}
 void cancel(){if(timer){lv_timer_del(timer);timer=nullptr;}window.clear();if(body)lv_obj_clear_flag(body,LV_OBJ_FLAG_HIDDEN);if(scroll){lv_obj_add_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);lv_obj_invalidate(scroll);}}
 void finish(){cancel();if(model.view==View::Card)caption("Another");}
 void start(){lv_obj_update_layout(scroll);fallback=!window.capture(scroll);if(fallback)return;started=lv_tick_get();lv_obj_add_flag(body,LV_OBJ_FLAG_HIDDEN);lv_obj_clear_flag(scroll,LV_OBJ_FLAG_SCROLLABLE);timer=lv_timer_create(tick,40,this);if(!timer){fallback=true;cancel();return;}caption("Reveal all");lv_obj_invalidate(scroll);}
 static void tick(lv_timer_t *t){auto *u=static_cast<Ui*>(t->user_data);auto elapsed=lv_tick_elaps(u->started);if(elapsed>=6000){u->finish();return;}u->window.render(elapsed);lv_obj_invalidate(u->scroll);}
 static void drawWindow(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));lv_area_t a;lv_obj_get_coords(lv_event_get_target(e),&a);auto *ctx=lv_event_get_draw_ctx(e);if(u->window.active())u->window.draw(ctx,a);else WindowReveal::face(ctx,a,0);}
 static void contact(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));switch(lv_event_get_code(e)){case LV_EVENT_PRESSED:u->pointerDown=true;u->gesture.reset();break;case LV_EVENT_RELEASED:case LV_EVENT_PRESS_LOST:u->pointerDown=false;u->gesture.reset();break;case LV_EVENT_SCROLL_BEGIN:u->scrolling=true;u->gesture.reset();break;case LV_EVENT_SCROLL_END:u->scrolling=false;u->gesture.reset();break;default:break;}}
 static void deleted(lv_event_t *e){auto *u=static_cast<Ui*>(lv_event_get_user_data(e));auto *o=lv_event_get_target(e);if(o==u->root){u->cancel();u->gesture.reset();u->root=nullptr;u->first=u->second=u->backButton=u->moreButton=u->toggle=u->body=u->scroll=u->motionStatus=nullptr;u->pointerDown=false;u->scrolling=false;return;}if(o==u->motionStatus)u->motionStatus=nullptr;if(u->second && lv_obj_get_parent(o)==u->second)u->cancel();if(o==u->body){u->body=nullptr;u->cancel();}if(o==u->scroll){u->scroll=nullptr;u->cancel();}if(o==u->second){u->second=nullptr;u->cancel();}if(o==u->first)u->first=nullptr;if(o==u->backButton)u->backButton=nullptr;if(o==u->moreButton)u->moreButton=nullptr;if(o==u->toggle)u->toggle=nullptr;}
 lv_obj_t *label(lv_obj_t *p,const char *s,int x,int y,int w,const lv_font_t *font){auto *o=lv_label_create(p);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_set_pos(o,x,y);lv_obj_set_width(o,w);lv_label_set_long_mode(o,LV_LABEL_LONG_WRAP);lv_obj_set_style_text_font(o,font,0);lv_obj_set_style_text_line_space(o,4,0);lv_label_set_text(o,s);return o;}
 lv_obj_t *button(Action a,const char *s,int x,int y,int w,int h){auto *o=lv_btn_create(root);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,w,h);lv_obj_clear_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_PRESS_LOCK);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);lv_obj_set_style_bg_color(o,lv_color_hex(a==Action::Ask?0x27689f:0x285066),0);lv_obj_set_style_bg_color(o,lv_color_hex(0x407994),LV_STATE_PRESSED);lv_obj_set_style_radius(o,8,0);lv_obj_set_style_outline_width(o,2,LV_STATE_FOCUSED);lv_obj_set_style_outline_pad(o,0,LV_STATE_FOCUSED);lv_obj_set_style_outline_color(o,lv_color_hex(0xffffff),LV_STATE_FOCUSED);auto *text=label(o,s,0,0,w-8,&lv_font_montserrat_16);lv_obj_set_style_text_align(text,LV_TEXT_ALIGN_CENTER,0);lv_obj_center(text);auto &slot=slots[unsigned(a)];slot={this,a};lv_obj_add_event_cb(o,clicked,LV_EVENT_CLICKED,&slot);lv_obj_add_event_cb(o,deleted,LV_EVENT_DELETE,this);lv_obj_add_event_cb(o,contact,LV_EVENT_ALL,this);return o;}
 static void clicked(lv_event_t *e){auto *s=static_cast<Slot*>(lv_event_get_user_data(e));s->owner->perform(s->action);if(auto *in=lv_indev_get_act())lv_indev_wait_release(in);}
};
}
