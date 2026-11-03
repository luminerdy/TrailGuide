// Actual bundled LVGL complete-copy typography; synthetic host, not device acceptance.
#include "../trailguide_v3/ui.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <algorithm>
static lv_color_t drawBuffer[360*20], pixels[360*360];
static uint32_t rng(void *p){auto &s=*static_cast<uint32_t*>(p);s=s*1664525u+1013904223u;return s;}
static void *allocate(size_t n){return malloc(n);}static void release(void *p){free(p);}
static void flush(lv_disp_drv_t *d,const lv_area_t *a,lv_color_t *c){for(int y=a->y1;y<=a->y2;y++)for(int x=a->x1;x<=a->x2;x++)pixels[y*360+x]=*c++;lv_disp_flush_ready(d);}
static void update(){lv_obj_update_layout(lv_scr_act());lv_refr_now(nullptr);}
static void save(const char *dir,const char *id,const char *mode,const char *view,unsigned page){
 std::string path=std::string(dir)+"/"+id+"-"+mode+"-"+view+"-"+std::to_string(page)+".ppm";
 std::ofstream f(path,std::ios::binary);assert(f);f<<"P6\n360 360\n255\n";
 for(auto pixel:pixels){lv_color32_t c;c.full=lv_color_to32(pixel);f.put(c.ch.red);f.put(c.ch.green);f.put(c.ch.blue);}
}
static void safe(lv_obj_t *o){lv_area_t a;lv_obj_get_coords(o,&a);for(int x:{a.x1-2,a.x2+2})for(int y:{a.y1-2,a.y2+2})assert((x-180)*(x-180)+(y-180)*(y-180)<=168*168);}
int main(int argc,char **argv){
 assert(argc==2);lv_init();lv_disp_draw_buf_t buffer;lv_disp_draw_buf_init(&buffer,drawBuffer,nullptr,360*20);
 lv_disp_drv_t driver;lv_disp_drv_init(&driver);driver.hor_res=driver.ver_res=360;driver.draw_buf=&buffer;driver.flush_cb=flush;lv_disp_drv_register(&driver);
 uint32_t seed=123;unsigned contexts=0,pages=0;size_t longestAsk=0,longestTry=0;const char *askId="",*tryId="";unsigned peak=0;
 for(unsigned i=0;i<trailguide::cardCount;i++)for(auto mode:{v3app::Mode::Ask,v3app::Mode::Try})for(auto view:{v3app::View::Card,v3app::View::Guide}){
  v3app::Ui u(rng,&seed,allocate,release);u.instant=true;u.model.index=i;u.model.mode=mode;u.model.view=view;u.render();update();
  auto &c=trailguide::cards[i];const bool main=view==v3app::View::Card;const char *text=main?(mode==v3app::Mode::Ask?c.ask:c.action):(mode==v3app::Mode::Ask?c.askGuide:c.tryGuide);
  assert(std::string(lv_label_get_text(u.body))==text);const lv_font_t *font=main?&lv_font_montserrat_20:&lv_font_montserrat_18;
  assert(lv_obj_get_style_text_font(u.body,LV_PART_MAIN)==font);assert(lv_obj_get_width(u.body)==240 && lv_obj_get_width(u.scroll)==256 && lv_obj_get_height(u.scroll)==140);
  assert(lv_obj_get_style_text_line_space(u.body,LV_PART_MAIN)==4);assert(lv_obj_get_style_text_opa(u.body,0)==LV_OPA_COVER);
  for(const unsigned char *s=reinterpret_cast<const unsigned char*>(text);*s;++s){assert(*s<128);if(*s!='\n'){lv_font_glyph_dsc_t glyph;assert(lv_font_get_glyph_dsc(font,&glyph,*s,0) && !glyph.is_placeholder);}}
  safe(u.scroll);safe(u.first);safe(u.second);safe(u.backButton);safe(u.moreButton);
  if(!main){if(mode==v3app::Mode::Ask && strlen(text)>longestAsk){longestAsk=strlen(text);askId=c.id;}if(mode==v3app::Mode::Try && strlen(text)>longestTry){longestTry=strlen(text);tryId=c.id;}}
  unsigned page=0;int previous=-1;
  // Every page overlaps by forty pixels; the bottom must be reachable via real More text.
  for(;;){update();assert(std::string(lv_label_get_text(u.body))==text);save(argv[1],c.id,mode==v3app::Mode::Ask?"ask":"try",main?"main":"guide",page++);++pages;
   if(lv_obj_get_scroll_bottom(u.scroll)<=2)break;
   const int y=lv_obj_get_scroll_y(u.scroll);assert(y>previous);previous=y;assert(page<100);u.perform(v3app::Ui::Action::More);
  }
  lv_area_t labelArea,viewport;lv_obj_get_coords(u.body,&labelArea);lv_obj_get_coords(u.scroll,&viewport);assert(labelArea.y2<=viewport.y2 && labelArea.y2>=viewport.y1);
  lv_mem_monitor_t memory;lv_mem_monitor(&memory);peak=std::max(peak,unsigned(memory.total_size-memory.free_size));++contexts;
  printf("COPY %s %s %s bytes=%zu height=%d pages=%u complete=1 font=%u\n",c.id,mode==v3app::Mode::Ask?"ask":"try",main?"main":"guide",strlen(text),lv_obj_get_height(u.body),page,unsigned(font->line_height));
 }
 assert(contexts==trailguide::cardCount*4);
 printf("PASS actual LVGL typography: %u contexts %u pages; longest Ask Guide %s %zu bytes; longest Try Guide %s %zu bytes; peak=%u; unchanged 20/18 fonts and geometry; synthetic host only\n",contexts,pages,askId,longestAsk,tryId,longestTry,peak);
}
