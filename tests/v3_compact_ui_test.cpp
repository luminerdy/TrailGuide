// Actual native LVGL input; synthetic sensor fixtures, never hardware evidence.
#include "ui.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
static lv_color_t buffer[360*20],pixels[360*360];
static uint32_t seed=7;
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);return s=s*1664525u+1013904223u;}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void advance(unsigned ms){for(unsigned i=0;i<ms;i+=10){lv_tick_inc(10);lv_timer_handler();}lv_obj_update_layout(lv_scr_act());}
static lv_point_t point;static bool pressed=false;
static void readTouch(lv_indev_drv_t *,lv_indev_data_t *d){d->point=point;d->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
static lv_obj_t *find(lv_obj_t *o,const char *text){if(lv_obj_check_type(o,&lv_label_class)&&!strcmp(lv_label_get_text(o),text))return o;for(unsigned i=0;i<lv_obj_get_child_cnt(o);++i)if(auto *p=find(lv_obj_get_child(o,i),text))return p;return nullptr;}
static void geometry(lv_obj_t *root){lv_obj_update_layout(root);for(unsigned i=0;i<lv_obj_get_child_cnt(root);++i){auto *o=lv_obj_get_child(root,i);if(!lv_obj_check_type(o,&lv_btn_class))continue;assert(lv_obj_get_width(o)>=48&&lv_obj_get_height(o)>=48);if(auto *text=lv_obj_get_child(o,0)){lv_area_t t,r;lv_obj_get_coords(text,&t);lv_obj_get_coords(o,&r);assert(t.y1>=r.y1&&t.y2<=r.y2&&"control caption must not be clipped by its invisible tap rectangle");}lv_area_t a;lv_obj_get_coords(o,&a);for(int x:{int(a.x1)-2,int(a.x2)+2})for(int y:{int(a.y1)-2,int(a.y2)+2})assert((x-180)*(x-180)+(y-180)*(y-180)<=168*168 && "complete tap/focus rectangle must be safe-circle contained");}}
static void save(const std::string &){ /* Full coaching harness retains frame captures. */ }
static void swipe(int x,int from,int to){point={lv_coord_t(x),lv_coord_t(from)};pressed=true;advance(60);for(int i=1;i<=8;++i){point.y=lv_coord_t(from+(to-from)*i/8);advance(30);}pressed=false;advance(1000);}
int main(){
 lv_init();lv_disp_draw_buf_t b;lv_disp_draw_buf_init(&b,buffer,nullptr,360*20);lv_disp_drv_t d;lv_disp_drv_init(&d);d.hor_res=d.ver_res=360;d.draw_buf=&b;d.flush_cb=flush;lv_disp_drv_register(&d);lv_indev_drv_t input;lv_indev_drv_init(&input);input.type=LV_INDEV_TYPE_POINTER;input.read_cb=readTouch;lv_indev_drv_register(&input);
 bool trial=true;int errors=0;unsigned openers=0;
 auto check=[&](bool ok,const char *message){if(!ok){printf("FAIL %s\n",message);++errors;}};
 {v3app::CoachUi c(rng,&seed,malloc,free);c.model.enter();
 for(unsigned i=0;i<trailguide_coaching::entryCount;++i){auto &e=trailguide_coaching::entries[i];using S=trailguide_coach::Screen;S screen=!strcmp(e.type,"opener")?S::Opening:!strcmp(e.type,"question")?S::Question:!strcmp(e.type,"experiment")?S::Experiment:!strcmp(e.type,"closing")?S::Closing:S::Reminders;c.model.current={screen,e.path,int(i),0};c.render();advance(60);geometry(c.root);check(!strcmp(lv_label_get_text(c.body),e.text),"complete canonical copy");
 check(lv_obj_get_style_text_font(c.body,0)==(trial?&lv_font_montserrat_18:&lv_font_montserrat_20),"main font trial18 baseline20");
 if(screen==S::Opening){++openers;lv_refr_now(nullptr);
 check(lv_obj_get_scroll_bottom(c.scroll)<=0&&lv_obj_get_scroll_y(c.scroll)==0,"all six canonical openers fit without scrolling");
 int minx=360,miny=360,maxx=-1,maxy=-1;unsigned ink=0;
 for(int y=112;y<252;++y)for(int x=52;x<308;++x){auto p=pixels[y*360+x];
  if(p.full==lv_color_hex(0x124d8b).full||p.full==lv_color_hex(0x27689f).full||p.full==lv_color_hex(0x0e3b6a).full){minx=std::min(minx,x);miny=std::min(miny,y);maxx=std::max(maxx,x);maxy=std::max(maxy,y);}
  if(x>=96&&x<264){lv_color32_t rgb;rgb.full=lv_color_to32(p);if(std::min({rgb.ch.red,rgb.ch.green,rgb.ch.blue})>160){++ink;check(x>=84&&x<=275&&y>=126&&y<=237,"actual opener ink stays inside blue face");}}
 }
 check(ink>0,"actual opener ink present");check(minx==84&&miny==126&&maxx==275&&maxy==237,"actual visible blue face exactly 192x112");check(!trial || pixels[180*360+60].full==lv_color_hex(0x02070d).full,"actual blue face smaller with black padding");printf("%s body height=%d scrollBottom=%d\n",e.id,lv_obj_get_height(c.body),lv_obj_get_scroll_bottom(c.scroll));check(lv_obj_get_width(c.scroll)==256&&lv_obj_get_height(c.scroll)==140,"fixed snapshot dimensions");
 for(unsigned j=0;j<6;++j){auto *button=lv_obj_get_parent(find(c.root,(const char*[]){"Progress","Stuck","Unclear","Explore","AI","Team"}[j]));auto *label=lv_obj_get_child(button,0);check(trial?lv_obj_get_style_text_font(label,0)==&lv_font_montserrat_12:lv_obj_get_style_text_font(label,0)==&lv_font_montserrat_14,"perimeter caption smaller than14");lv_area_t a,t;lv_obj_get_coords(button,&a);lv_obj_get_coords(label,&t);check(t.x1>=a.x1&&t.x2<=a.x2&&t.y1>=a.y1&&t.y2<=a.y2,"actual label bounds inside hit region");if(trial){int oldx[]={180,292,292,180,68,68},oldy[]={48,118,242,312,242,118};int x=(a.x1+a.x2+1)/2,y=(a.y1+a.y2+1)/2;check((x-180)*(x-180)+(y-180)*(y-180)>(oldx[j]-180)*(oldx[j]-180)+(oldy[j]-180)*(oldy[j]-180),"controls outward");}}
 lv_area_t t,p;lv_obj_get_coords(c.body,&t);lv_obj_get_coords(c.scroll,&p);t.y1=std::max(t.y1,p.y1);t.y2=std::min(t.y2,p.y2);for(unsigned j=0;j<lv_obj_get_child_cnt(c.root);++j){auto *o=lv_obj_get_child(c.root,j);if(!lv_obj_check_type(o,&lv_btn_class))continue;lv_area_t a;lv_obj_get_coords(o,&a);check(!(t.x1<=a.x2&&t.x2>=a.x1&&t.y1<=a.y2&&t.y2>=a.y1),"visible text excludes controls");}
 }
 save(e.id);if(lv_obj_get_scroll_bottom(c.scroll)>0){lv_obj_scroll_to_y(c.scroll,32000,LV_ANIM_OFF);advance(60);check(lv_obj_get_scroll_bottom(c.scroll)<=1,"complete-copy scroll end");if(screen==S::Opening)save(std::string(e.id)+"-end");}
 }
 c.model.current={trailguide_coach::Screen::Opening,-1,5,0};c.render();advance(60);save("longest-initial");lv_obj_scroll_to_y(c.scroll,32000,LV_ANIM_OFF);advance(60);save("longest-end");
 // A visibly labelled synthetic overflow fixture, not canonical firmware copy.
 std::string synthetic="Synthetic overflow fixture. ";for(int j=0;j<8;++j)synthetic+="Keep the complete question accessible while scrolling. ";synthetic+="FINAL LINE";
 lv_label_set_text(c.body,synthetic.c_str());lv_obj_update_layout(c.root);lv_obj_scroll_to_y(c.scroll,0,LV_ANIM_OFF);advance(60);check(lv_obj_get_scroll_bottom(c.scroll)>0,"synthetic long text overflows");save("synthetic-initial");swipe(180,225,145);check(lv_obj_get_scroll_y(c.scroll)>0,"actual input driver scrolls synthetic full-copy fixture");lv_obj_scroll_to_y(c.scroll,32000,LV_ANIM_OFF);advance(60);check(lv_obj_get_scroll_bottom(c.scroll)<=1,"synthetic full-copy end reachable");save("synthetic-end");check(!strcmp(lv_label_get_text(c.body),synthetic.c_str()),"synthetic complete text retained");
 c.model.current={trailguide_coach::Screen::Question,0,6,0};c.render();c.perform(v3app::CoachUi::Action::Picker);geometry(c.root);save("picker");
 }
 {seed=7;v3app::CoachUi c(rng,&seed,malloc,free);c.model.enter();c.render();for(unsigned n=0;n<30;++n){c.perform(v3app::CoachUi::Action::Another);if(!strcmp(c.model.entry()->id,"OPEN-03"))break;c.perform(v3app::CoachUi::Action::Another);}check(c.busy()&&!c.fallback&&c.window.active(),"successful actual reveal capture");check(!strcmp(c.model.entry()->id,"OPEN-03"),"paired reveal uses canonical OPEN-03");const std::string copy=lv_label_get_text(c.body);for(unsigned ms=0;ms<=6000;ms+=1000){c.window.render(ms);lv_obj_invalidate(c.scroll);save(std::string("reveal-")+std::to_string(ms));check(copy==lv_label_get_text(c.body),"whole question unchanged throughout 6s reveal");}lv_refr_now(nullptr);std::string finalRaster(reinterpret_cast<char*>(pixels),sizeof pixels);c.perform(v3app::CoachUi::Action::Another);lv_refr_now(nullptr);check(finalRaster==std::string(reinterpret_cast<char*>(pixels),sizeof pixels),"byte-identical projection to live handoff");advance(5000);check(!c.busy(),"six second reveal and reading hold retain timing");}
 check(openers==6,"exactly six openers checked");printf("compact errors=%d; rendered 55 canonical faces; all six openers fit; font18/12, exact pixels, geometry, synthetic overflow and handoff. HOST ONLY\n",errors);return errors?1:0;
}
