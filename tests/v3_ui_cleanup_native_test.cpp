// Real LVGL 8.3.10 pixels and pointer input, no widget mocks.
#include <lvgl.h>
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>
static constexpr auto OUT="build/v3-0.1.31/evidence/ui-cleanup/frames/";
static lv_color_t buffer[360*20],pixels[360*360];
static uint32_t seed=7;
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;++y)for(int x=a->x1;x<=a->x2;++x)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static lv_point_t point;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static void tap(int x,int y){point={lv_coord_t(x),lv_coord_t(y)};pressed=true;advance(60);pressed=false;advance(60);}
static lv_obj_t *find(lv_obj_t *o,const char *text){if(lv_obj_check_type(o,&lv_label_class)&&!strcmp(lv_label_get_text(o),text))return o;for(unsigned i=0;i<lv_obj_get_child_cnt(o);++i)if(auto *p=find(lv_obj_get_child(o,i),text))return p;return nullptr;}
static void save(const std::string &name){lv_refr_now(nullptr);std::ofstream f(std::string(OUT)+name+".ppm",std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";for(auto p:pixels){lv_color32_t c;c.full=lv_color_to32(p);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}}
static void footerAbsent(){for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}){v3app::Ui u(rng,&seed,malloc,free);u.model.draw(mode);u.render();save(mode==v3app::Mode::Ask?"classic-ask":"classic-try");assert(!find(u.root,mode==v3app::Mode::Ask?"ASK DRAFT | touch":"TRY DRAFT | touch"));assert(!u.motionStatus);auto index=u.model.index;tap(125,284);assert(u.model.view==v3app::View::Guide&&u.model.index==index);tap(125,78);assert(u.model.view==v3app::View::Card&&u.model.index==index);}} 
static bool visible(lv_obj_t *o){return o&&!lv_obj_has_flag(o,LV_OBJ_FLAG_HIDDEN);}
static const char *longCopy="Line one\nLine two\nLine three\nLine four\nLine five\nLine six\nLine seven\nLine eight\nLine nine\nLine ten\nLine eleven\nLine twelve";
static void copy(lv_obj_t *root,lv_obj_t *body,const char *s){lv_label_set_text(body,s);lv_obj_update_layout(root);advance(60);}
static void classicMore(){for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try}){v3app::Ui u(rng,&seed,malloc,free);u.model.draw(mode);u.render();copy(u.root,u.body,"Fits.");assert(lv_obj_get_scroll_bottom(u.scroll)<=0);assert(!visible(u.moreButton));save("classic-short");copy(u.root,u.body,longCopy);assert(lv_obj_get_scroll_bottom(u.scroll)>0);assert(visible(u.moreButton));int y=lv_obj_get_scroll_y(u.scroll);tap(230,78);assert(lv_obj_get_scroll_y(u.scroll)>y);assert(visible(u.moreButton));save("classic-middle");for(int i=0;i<20&&visible(u.moreButton);++i)tap(230,78);assert(lv_obj_get_scroll_bottom(u.scroll)<=0);assert(!visible(u.moreButton));save("classic-bottom");lv_obj_scroll_to_y(u.scroll,0,LV_ANIM_OFF);advance(60);assert(visible(u.moreButton));copy(u.root,u.body,"");assert(!visible(u.moreButton));copy(u.root,u.body,"Fits again.");assert(!visible(u.moreButton));u.render();copy(u.root,u.body,"Fits after rerender.");assert(!visible(u.moreButton));}puts("PASS Classic actual More pointer pages long text; short/empty/bottom hidden; scroll-top restores and rerender recomputes");}
static void drag(int x,int from,int to){point={lv_coord_t(x),lv_coord_t(from)};pressed=true;advance(60);for(int i=1;i<=10;++i){point.y=from+(to-from)*i/10;advance(30);}pressed=false;advance(500);}
static void coachHint(){for(bool opening:{false,true}){v3app::CoachUi c(rng,&seed,malloc,free);c.model.enter();if(!opening)c.model.path(0);c.render();auto *hint=find(c.root,"Scroll for full text");assert(hint);copy(c.root,c.body,"Fits.");assert(!visible(hint));save(opening?"coach-opening-short":"coach-path-short");copy(c.root,c.body,longCopy);assert(visible(hint));auto y=lv_obj_get_scroll_y(c.scroll);drag(180,210,145);assert(lv_obj_get_scroll_y(c.scroll)>y);assert(visible(hint));save(opening?"coach-opening-middle":"coach-path-middle");lv_obj_scroll_to_y(c.scroll,LV_COORD_MAX,LV_ANIM_OFF);advance(60);assert(!visible(hint));save(opening?"coach-opening-bottom":"coach-path-bottom");lv_obj_scroll_to_y(c.scroll,0,LV_ANIM_OFF);advance(60);assert(visible(hint));copy(c.root,c.body,"");assert(!visible(hint));copy(c.root,c.body,"Fits again.");assert(!visible(hint));c.render();copy(c.root,c.body,"Fits after rerender.");assert(!visible(find(c.root,"Scroll for full text")));}puts("PASS Coaching actual drag moves full long copy; fit/empty/bottom hide hint; top restores, short/rerender no false cue");}
template<class U> static void pose(U &u,float z,unsigned n){while(n--){advance(20);v3::ImuSample s;s.valid=true;s.at_ms=lv_tick_get();s.acc_g[2]=z;s.gyr_dps[1]=2.2f;u.sample(s,s.at_ms,true);}}
static bool black(){lv_refr_now(nullptr);for(auto p:pixels)if(p.full)return false;return true;}
template<class U> static void blankCancel(U &u){pose(u,1,50);pose(u,-1,10);assert(black());v3::ImuSample s;s.at_ms=lv_tick_get();s.valid=false;u.sample(s,s.at_ms,false);assert(!black());}
static void guards(){
 for(int kind=0;kind<3;++kind){
  v3app::Ui u(rng,&seed,malloc,free);u.instant=kind!=0;u.perform(v3app::Ui::Action::Ask);
  if(kind==0){assert(u.busy());save("classic-reveal-start");advance(3000);assert(u.busy()&&!visible(u.moreButton));save("classic-reveal-mid");advance(3000);assert(!u.busy());}
  copy(u.root,u.body,kind?longCopy:"Fits.");if(kind==2){lv_obj_scroll_to_y(u.scroll,LV_COORD_MAX,LV_ANIM_OFF);advance(60);}assert(!visible(u.moreButton));pose(u,1,50);pose(u,-1,10);assert(black());advance(2100);assert(black());{v3::ImuSample s;s.at_ms=lv_tick_get();u.sample(s,s.at_ms,false);}assert(visible(u.moreButton)==(kind==1));
  blankCancel(u);assert(visible(u.moreButton)==(kind==1));assert(!u.motionStatus);save("classic-blank-restored-"+std::to_string(kind));
  point={180,180};pressed=true;advance(60);assert(!u.eligible());pressed=false;advance(60);
 }
 for(int kind=0;kind<3;++kind){
  v3app::CoachUi c(rng,&seed,malloc,free);c.instant=kind!=0;c.perform(v3app::CoachUi::Action::Another);
  auto *hint=find(c.root,"Scroll for full text");assert(hint);
  if(kind==0){assert(c.busy());save("coach-reveal-start");advance(3000);assert(c.busy()&&!visible(hint));save("coach-reveal-mid");advance(3000);assert(!c.busy());}
  copy(c.root,c.body,kind?longCopy:"Fits.");if(kind==2){lv_obj_scroll_to_y(c.scroll,LV_COORD_MAX,LV_ANIM_OFF);advance(60);}assert(!visible(hint));pose(c,1,50);pose(c,-1,10);assert(black());advance(2100);assert(black());{v3::ImuSample s;s.at_ms=lv_tick_get();c.sample(s,s.at_ms,false);}assert(visible(hint)==(kind==1));
  blankCancel(c);assert(visible(hint)==(kind==1));save("coach-blank-restored-"+std::to_string(kind));
  point={180,180};pressed=true;advance(60);assert(!c.eligible());pressed=false;advance(60);
 }
 puts("PASS real reveal 6s + hold 2s, short/top-long/bottom focus expiry and blank cancellation restore correct cues; pointer gate retained");
}
static void catalog(){unsigned classic=0,coaching=0;
 for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try})for(unsigned i=0;i<trailguide::cardCount;++i)for(auto view:{v3app::View::Card,v3app::View::Guide}){
  v3app::Ui u(rng,&seed,malloc,free);u.model.mode=mode;u.model.index=i;u.model.view=view;u.render();auto &card=trailguide::cards[i];const char *expected=view==v3app::View::Card?(mode==v3app::Mode::Ask?card.ask:card.action):(mode==v3app::Mode::Ask?card.askGuide:card.tryGuide);assert(!strcmp(lv_label_get_text(u.body),expected));lv_area_t a,b;lv_obj_get_coords(u.body,&a);lv_obj_get_coords(u.scroll,&b);assert(visible(u.moreButton)==(*expected&&a.y2>b.y2));assert(!u.motionStatus);lv_obj_scroll_to_y(u.scroll,LV_COORD_MAX,LV_ANIM_OFF);advance(60);assert(!visible(u.moreButton));++classic;
 }
 for(unsigned i=0;i<trailguide_coaching::entryCount;++i)for(bool guide:{false,true}){
  const auto &e=trailguide_coaching::entries[i];v3app::CoachUi c(rng,&seed,malloc,free);auto screen=!strcmp(e.type,"opener")?trailguide_coach::Screen::Opening:!strcmp(e.type,"question")?trailguide_coach::Screen::Question:!strcmp(e.type,"experiment")?trailguide_coach::Screen::Experiment:!strcmp(e.type,"closing")?trailguide_coach::Screen::Closing:trailguide_coach::Screen::Reminders;
  c.model.current={guide?trailguide_coach::Screen::Guide:screen,e.path,int(i),0};c.render();const char *expected=guide?e.guidance:e.text;assert(!strcmp(lv_label_get_text(c.body),expected));auto *hint=find(c.root,"Scroll for full text");assert(hint);lv_area_t a,b;lv_obj_get_coords(c.body,&a);lv_obj_get_coords(c.scroll,&b);int mask=!guide&&screen==trailguide_coach::Screen::Opening?14:0;assert(visible(hint)==(*expected&&a.y2>b.y2-mask));assert(lv_obj_get_style_text_font(c.body,0)==&lv_font_montserrat_18);if(mask){assert(lv_obj_get_width(c.body)==168);assert(lv_obj_get_width(c.scroll)==256&&lv_obj_get_height(c.scroll)==140);}
  const bool editorial = guide ? (!strcmp(e.id,"STUCK-E01") || !strcmp(e.id,"EXPLORING-E01") || !strcmp(e.id,"AI-RESULTS-E02") || !strcmp(e.id,"TEAM-E01")) : (!strcmp(e.id,"AI-RESULTS-Q02") || !strcmp(e.id,"PROGRESS-E03") || !strcmp(e.id,"TEAM-Q04"));
  const std::string capture=std::string("C1-")+e.id+(guide?"-guide":"-main");
  const auto previousSeed=seed;const auto previousDraws=c.model.draws;
  if(editorial){save(capture+"-top");if(lv_obj_get_scroll_bottom(c.scroll)>0){const auto before=lv_obj_get_scroll_y(c.scroll);drag(180,210,145);assert(lv_obj_get_scroll_y(c.scroll)>before&&"approved copy must support real pointer scrolling");}}
  lv_obj_scroll_to_y(c.scroll,LV_COORD_MAX,LV_ANIM_OFF);advance(60);assert(!visible(hint));assert(lv_obj_has_flag(c.scroll,LV_OBJ_FLAG_SCROLLABLE));
  if(editorial){lv_obj_get_coords(c.body,&a);lv_obj_get_content_coords(c.scroll,&b);assert(a.y2<=b.y2-mask&&"last line must be inside the visible content endpoint");assert(seed==previousSeed&&c.model.draws==previousDraws);save(capture+"-bottom");}
  ++coaching;
 }
 printf("PASS canonical full copy and cues at top/bottom: %u Classic faces; %u Coaching faces; 18px/168px opening text and unchanged snapshot geometry\n",classic,coaching);
}
int main(){setbuf(stdout,nullptr);lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);footerAbsent();classicMore();coachHint();guards();catalog();puts("PASS Classic Ask/Try footer absent; actual Guide/Back pointer preserves card context");}
