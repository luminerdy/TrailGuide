#include "../trailguide_v2/ui.h"
#include <cassert>
#include <cstring>
#include "../trailguide_v2/reveal.h"
#include <type_traits>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
static bool failSnapshot = false;
static void *snapshotAllocation = nullptr;
static unsigned snapshotAllocations = 0;
extern "C" void *__real_malloc(size_t);
extern "C" void __real_free(void *);
extern "C" void *__wrap_malloc(size_t size) {
  if (size == 160U * 166U * 4U) {
    if (failSnapshot) return nullptr;
    assert(!snapshotAllocation);
    snapshotAllocation = __real_malloc(size);
    if (snapshotAllocation) ++snapshotAllocations;
    return snapshotAllocation;
  }
  return __real_malloc(size);
}
extern "C" void __wrap_free(void *p) {
  if (p && p == snapshotAllocation) snapshotAllocation = nullptr;
  __real_free(p);
}
static lv_color_t pixels[172 * 40];
static lv_color_t rendered[172 * 320];
static void flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *colors) {
  for (int y = area->y1; y <= area->y2; ++y)
    for (int x = area->x1; x <= area->x2; ++x) rendered[y * 172 + x] = *colors++;
  lv_disp_flush_ready(driver);
}
static void click(lv_obj_t *button) { assert(button); lv_event_send(button, LV_EVENT_CLICKED, nullptr); lv_obj_update_layout(lv_scr_act()); }
static unsigned observedHeapPeak=0;
static void sampleHeap() { lv_mem_monitor_t m; lv_mem_monitor(&m); observedHeapPeak=std::max(observedHeapPeak,unsigned(m.total_size-m.free_size)); }
static void advance(unsigned ms) { for (unsigned n = 0; n < ms; n += 10) { lv_tick_inc(10); lv_timer_handler(); sampleHeap(); } lv_obj_update_layout(lv_scr_act()); }
static std::string fixedImage() {
  lv_refr_now(nullptr);
  std::string result(reinterpret_cast<const char *>(rendered),172*86*sizeof(lv_color_t));
  for(int y=264;y<314;++y)
    result.append(reinterpret_cast<const char *>(&rendered[y*172+6]),76*sizeof(lv_color_t));
  return result;
}
static std::string promptImage() {
  lv_refr_now(nullptr);
  std::string data;
  for (int y = 86; y < 252; ++y) for (int x = 6; x < 166; ++x) {
    lv_color32_t color; color.full = lv_color_to32(rendered[y * 172 + x]);
    data += static_cast<char>(color.ch.red);
    data += static_cast<char>(color.ch.green);
    data += static_cast<char>(color.ch.blue);
  }
  return data;
}
struct PixelBounds {
  int x1=160,y1=166,x2=-1,y2=-1; unsigned count=0;
  int width() const { return x2-x1+1; }
};
static PixelBounds feature(const std::string &image, bool blue) {
  PixelBounds result;
  for(int y=0;y<166;++y) for(int x=0;x<160;++x) {
    const auto p=(y*160+x)*3;
    const unsigned r=static_cast<unsigned char>(image[p]),g=static_cast<unsigned char>(image[p+1]),b=static_cast<unsigned char>(image[p+2]);
    const bool yes=blue?(b>18 && b>r*3 && b*10>g*13):(r>20 && r*10>g*9 && r*10>b*8);
    if(yes) { result.x1=std::min(result.x1,x); result.x2=std::max(result.x2,x); result.y1=std::min(result.y1,y); result.y2=std::max(result.y2,y); ++result.count; }
  }
  return result;
}
static void saveFrame(const char *name) {
  lv_refr_now(nullptr);
  std::ofstream file(std::string("build/v2-0.1.8/") + name + ".ppm", std::ios::binary);
  assert(file);
  file << "P6\n172 320\n255\n";
  for (const auto &pixel : rendered) {
    lv_color32_t color; color.full = lv_color_to32(pixel);
    file.put(static_cast<char>(color.ch.red));
    file.put(static_cast<char>(color.ch.green));
    file.put(static_cast<char>(color.ch.blue));
  }
}
static lv_point_t touchPoint{0,0};
static bool touching=false;
static void readTouch(lv_indev_drv_t *, lv_indev_data_t *data) {
  data->point=touchPoint; data->state=touching?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;
}
static unsigned timers() { unsigned count = 0; for (auto *t = lv_timer_get_next(nullptr); t; t = lv_timer_get_next(t)) ++count; return count; }
static void bounds(lv_obj_t *button) {
  assert(button); lv_area_t a; lv_obj_get_coords(button, &a);
  assert(a.x1 >= 0 && a.y1 >= 0 && a.x2 < 172 && a.y2 < 320);
  assert(lv_obj_get_width(button) >= 48 && lv_obj_get_height(button) >= 48);
}
static trailguide::Ui diagnosticUi;
static bool diagnosticPassed=false;
static bool diagnosticBounds(lv_obj_t *obj) { bounds(obj); return true; }
static struct {
  void println(const char *message) { diagnosticPassed=std::strstr(message,"SELFTEST PASS")!=nullptr; }
} Serial;
#define ui diagnosticUi
#define buttonBounds diagnosticBounds
#include "native_selftest.h"
#undef buttonBounds
#undef ui
static bool utf8Complete(const std::string &text) {
  size_t i = 0;
  while (i < text.size()) {
    const auto c = static_cast<unsigned char>(text[i++]);
    unsigned extra = c < 0x80 ? 0 : (c & 0xe0) == 0xc0 ? 1 : (c & 0xf0) == 0xe0 ? 2 : (c & 0xf8) == 0xf0 ? 3 : 99;
    if (i + extra > text.size()) return false;
    while (extra--) if ((static_cast<unsigned char>(text[i++]) & 0xc0) != 0x80) return false;
  }
  return true;
}
int main() {
  static_assert(!std::is_copy_constructible<trailguide::Ui>::value, "timer owner cannot be copied");
  assert(trailguide::RevealTiming::completeMs == 6000); // Approved six seconds, not the rejected fade.
  lv_init(); lv_disp_draw_buf_t buffer; lv_disp_draw_buf_init(&buffer, pixels, nullptr, 172 * 40);
  lv_disp_drv_t driver; lv_disp_drv_init(&driver); driver.hor_res = 172; driver.ver_res = 320; driver.draw_buf = &buffer; driver.flush_cb = flush; lv_disp_drv_register(&driver);
  std::string longText;
  for (unsigned n = 0; n < 6000; ++n) longText += u8"é中🌱 ";
  for (const auto &text : {std::string(), std::string(u8"é中🌱"), longText}) {
    unsigned previous = 0;
    auto *label = lv_label_create(lv_scr_act());
    for (unsigned elapsed = 0; elapsed <= 6000; elapsed += 10) {
      const auto opacity = unsigned(trailguide::WindowReveal::pose(elapsed).light * 255);
      assert(opacity >= previous && opacity <= 255);
      if (elapsed <= 600) assert(opacity == 0);
      assert(utf8Complete(text));
      if (text.size() < 100) { // LVGL's fixed 48KB heap cannot hold a 60KB label.
        lv_label_set_text(label, text.c_str());
        lv_obj_set_style_text_opa(label, opacity, 0);
        assert(text == lv_label_get_text(label));
      }
      previous = opacity;
    }
    assert(previous == 255);
    assert(trailguide::WindowReveal::pose(0xffffffffU).light == 1);
    lv_obj_del(label);
  }
  const uint32_t keyTimes[]={0,600,1680,3780,5040,6000};
  const trailguide::WindowReveal::Pose expected[]={
    {3,25,-110,13,-9,-3,0},{3,25,-110,13,-9,-3,0},
    {2,21,-92,11,-7,-2,.22f},{-2,7,-30,4,3,1,.73f},
    {1,1,-3,1,-1,-.3f,.96f},{0,0,0,0,0,0,1}};
  for(unsigned k=0;k<6;++k) {
    const auto p=trailguide::WindowReveal::pose(keyTimes[k]); const auto e=expected[k];
    assert(std::fabs(p.x-e.x)<.001f && std::fabs(p.y-e.y)<.001f && std::fabs(p.z-e.z)<.001f);
    assert(std::fabs(p.rx-e.rx)<.001f && std::fabs(p.ry-e.ry)<.001f && std::fabs(p.rz-e.rz)<.001f && std::fabs(p.light-e.light)<.001f);
  }
  trailguide::Ui ui; ui.render(); lv_obj_update_layout(lv_scr_act());
  assert(ui.model.view == trailguide::View::Home); bounds(ui.first); bounds(ui.second);
  for (unsigned mode = 0; mode < 2; ++mode) {
    click(mode == 0 ? ui.first : ui.second);
    for (unsigned n = 0; n < 8; ++n) {
      assert(ui.model.view == trailguide::View::Card); assert(ui.model.index == n);
      assert(ui.model.mode == (mode == 0 ? trailguide::Mode::Ask : trailguide::Mode::Try));
      bounds(ui.backButton); bounds(ui.first); bounds(ui.second);
      const auto &card = trailguide::cards[n];
      const char *complete = mode == 0 ? card.ask : card.action;
      assert(std::strcmp(lv_label_get_text(ui.body), complete) == 0);
      assert(lv_obj_has_flag(ui.body, LV_OBJ_FLAG_HIDDEN));
      const auto fullHeight = lv_obj_get_height(ui.body);
      const std::string framePrefix=std::string(mode==0?"ask-":"try-")+std::to_string(n);
      const auto fixed=fixedImage();
      assert(lv_obj_get_x(ui.scroll)==6 && lv_obj_get_y(ui.scroll)==86 && lv_obj_get_width(ui.scroll)==160 && lv_obj_get_height(ui.scroll)==166);
      assert(lv_obj_get_x(ui.first)==6 && lv_obj_get_y(ui.first)==264 && lv_obj_get_width(ui.first)==76 && lv_obj_get_height(ui.first)==50);
      assert(lv_obj_get_x(ui.second)==90 && lv_obj_get_y(ui.second)==264 && lv_obj_get_width(ui.second)==76 && lv_obj_get_height(ui.second)==50);
      const auto hiddenImage = promptImage();
      saveFrame((framePrefix+"-start").c_str());
      if (mode == 0 && n == 0) saveFrame("hidden");
      advance(500); assert(lv_obj_has_flag(ui.body, LV_OBJ_FLAG_HIDDEN));
      assert(promptImage()==hiddenImage); // Ten percent of the SIX seconds stays dark.
      advance(1180);
      assert(utf8Complete(lv_label_get_text(ui.body)));
      assert(lv_obj_has_flag(ui.body, LV_OBJ_FLAG_HIDDEN)); // Immutable native label is captured, not faded.
      assert(std::strcmp(lv_label_get_text(ui.body), complete) == 0);
      assert(lv_obj_get_height(ui.body) == fullHeight); // No typing/reflow.
      const auto partialImage = promptImage();
      const auto blueEarly=feature(partialImage,true), wordsEarly=feature(partialImage,false);
      assert(blueEarly.count>4000 && wordsEarly.count>100);
      const unsigned allocations = snapshotAllocations;

      if (mode == 0 && n == 0) saveFrame("early");
      advance(2100);
      if (mode == 0 && n == 0) saveFrame("emerging");
      saveFrame((framePrefix+"-intermediate").c_str());
      assert(promptImage() != hiddenImage); // Actual rendered pixels change, not just style metadata.
      advance(1260);
      assert(lv_obj_has_flag(ui.body, LV_OBJ_FLAG_HIDDEN));
      advance(920);
      const auto nearlySettled=promptImage();
      advance(40); // Complete at 6000ms, including the 600ms dark settle.
      const auto settled=promptImage(); unsigned changed=0;
      for(size_t p=0;p<settled.size();p+=3)
        if(settled.compare(p,3,nearlySettled,p,3)!=0) ++changed;
      assert(changed<1000); // No last-frame one-pixel whole-word/background pop.
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0);
      assert(!lv_obj_has_flag(ui.body, LV_OBJ_FLAG_HIDDEN));
      assert(promptImage() != partialImage);
      const auto finalImage=promptImage();
      assert(fixedImage()==fixed);
      const auto blueFinal=feature(finalImage,true), wordsFinal=feature(finalImage,false);
      // Spatial evidence: BOTH coherent surface and complete-word raster approach
      // from depth. A stationary-opacity animation cannot satisfy these bounds.
      assert(blueFinal.width()>blueEarly.width()+15 && blueFinal.y1+20<blueEarly.y1);
      assert(wordsFinal.width()>wordsEarly.width()+10 && wordsFinal.y1+20<wordsEarly.y1);
      assert(snapshotAllocations==allocations && !snapshotAllocation);
      if (mode == 0 && n == 0) saveFrame("clear");
      saveFrame((framePrefix+"-final").c_str());
      assert(lv_obj_get_style_text_font(ui.body, 0) == &lv_font_montserrat_20);
      assert(lv_obj_get_width(ui.body) == 152);
      if (lv_obj_get_height(ui.body) > lv_obj_get_height(ui.scroll)) {
        lv_obj_scroll_to_y(ui.scroll, lv_obj_get_height(ui.body), LV_ANIM_OFF);
        assert(lv_obj_get_scroll_y(ui.scroll) > 0);
      }
      click(ui.first); assert(ui.model.view == trailguide::View::Guide); bounds(ui.backButton);
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.askGuide : card.tryGuide) == 0);
      assert(lv_obj_get_style_text_font(ui.body,0)==&lv_font_montserrat_18);
      assert(lv_obj_get_height(ui.body) > lv_obj_get_height(ui.scroll));
      lv_obj_scroll_to_y(ui.scroll, lv_obj_get_height(ui.body), LV_ANIM_OFF);
      assert(lv_obj_get_scroll_y(ui.scroll) > 0);
      click(ui.backButton); assert(ui.model.index == n);
      click(ui.second);
    }
    assert(ui.model.index == 0); click(ui.backButton); assert(ui.model.view == trailguide::View::Home);
  }
  for (unsigned mode = 0; mode < 2; ++mode) {
    click(mode == 0 ? ui.first : ui.second);
    for (unsigned n = 0; n < 8; ++n) {
      assert(ui.model.index == n);
      assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second, 0)), "Reveal\nall") == 0);
      bounds(ui.second);
      lv_area_t buttonArea, captionArea;
      lv_obj_get_coords(ui.second, &buttonArea);
      lv_obj_get_coords(lv_obj_get_child(ui.second, 0), &captionArea);
      assert(captionArea.x1 >= buttonArea.x1 && captionArea.x2 <= buttonArea.x2);
      assert(captionArea.y1 >= buttonArea.y1 && captionArea.y2 <= buttonArea.y2);
      lv_event_send(ui.scroll, LV_EVENT_SCROLL_BEGIN, nullptr);
      lv_obj_scroll_to_y(ui.scroll, 40, LV_ANIM_OFF);
      lv_event_send(ui.scroll, LV_EVENT_SCROLL_END, nullptr);
      assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second, 0)), "Reveal\nall") == 0);
      click(ui.second); // Skip is not a draw.
      assert(ui.model.index == n);
      const auto &card = trailguide::cards[n];
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0);
      assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second, 0)), "Another") == 0);
      advance(3100); assert(ui.model.index == n);
      click(ui.second); assert(ui.model.index == (n + 1) % 8);
    }
    click(ui.backButton);
  }
  // Every Guide is immediately available even while its main card is settling.
  for (unsigned mode = 0; mode < 2; ++mode) {
    click(mode == 0 ? ui.first : ui.second);
    for (unsigned n = 0; n < 8; ++n) {
      click(ui.first);
      const auto &card = trailguide::cards[n];
      assert(ui.model.index == n && ui.model.view == trailguide::View::Guide);
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.askGuide : card.tryGuide) == 0);
      advance(3100);
      click(ui.backButton);
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0);
      assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second, 0)), "Another") == 0);
      click(ui.second);
    }
    click(ui.backButton);
  }
  // Context changes always restore complete copy, never restart a draw.
  for (unsigned n = 0; n < 40; ++n) {
    click(ui.first); advance(300);
    click(ui.first);
    assert(ui.model.view == trailguide::View::Guide);
    assert(std::strcmp(lv_label_get_text(ui.body), trailguide::cards[0].askGuide) == 0);
    advance(3100);
    click(ui.backButton);
    assert(std::strcmp(lv_label_get_text(ui.body), trailguide::cards[0].ask) == 0);
    click(ui.second); advance(300); ui.render(); advance(3100);
    assert(ui.model.index == 1);
    assert(std::strcmp(lv_label_get_text(ui.body), trailguide::cards[1].ask) == 0);
    click(ui.second); advance(300); click(ui.backButton); advance(3100);
    assert(ui.model.view == trailguide::View::Home);
  }
  click(ui.first); advance(300);
  const auto otaIndex = ui.model.index;
  ui.setUpdating(true);
  assert(std::strcmp(lv_label_get_text(ui.body), trailguide::cards[otaIndex].ask) == 0);
  advance(3100);
  ui.perform(trailguide::Ui::Action::Another);
  assert(ui.model.index == otaIndex);
  ui.setUpdating(false);
  assert(std::strcmp(lv_label_get_text(ui.body), trailguide::cards[otaIndex].ask) == 0);
  assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second, 0)), "Another") == 0);
  click(ui.backButton);
  ui.setConnection(true);
  assert(std::strstr(lv_label_get_text(ui.connectionLabel), "WiFi/OTA"));
  click(ui.first); click(ui.first);
  const auto index = ui.model.index;
  ui.setUpdating(true);
  assert(ui.updating && ui.overlay);
  lv_obj_update_layout(lv_layer_top());
  assert(lv_obj_get_width(ui.overlay) == 172 && lv_obj_get_height(ui.overlay) == 320);
  ui.perform(trailguide::Ui::Action::Home);
  assert(ui.model.view == trailguide::View::Guide && ui.model.index == index);
  ui.setUpdating(false);
  assert(!ui.updating && !ui.overlay && ui.model.view == trailguide::View::Guide);
  click(ui.backButton); click(ui.backButton);
  ui.setConnection(false);
  assert(std::strstr(lv_label_get_text(ui.connectionLabel), "Offline"));
  const auto baseTimers = timers();
  for (unsigned n = 0; n < 40; ++n) {
    trailguide::Ui temporary;
    temporary.render(); click(temporary.first); advance(300);
    assert(timers() == baseTimers + 1);
    lv_obj_clean(lv_scr_act()); // External LVGL teardown must cancel before stale callbacks.
    assert(timers() == baseTimers);
    advance(3100);
    temporary.model.home(); temporary.render(); click(temporary.second); advance(300);
    lv_obj_del(temporary.second);
    assert(timers() == baseTimers);
    assert(std::strcmp(lv_label_get_text(temporary.body), trailguide::cards[0].action) == 0);
    assert(lv_obj_get_style_text_opa(temporary.body, 0) == LV_OPA_COVER);
    temporary.render();
    temporary.perform(trailguide::Ui::Action::Another); advance(300);
  } // Destruction must delete the active timer and owner callbacks too.
  assert(timers() == baseTimers);
  advance(3100);
  assert(!snapshotAllocation);
  // Reserve enough LVGL space BEFORE the snapshot API's asserting allocator.
  // Caller RGB565 memory is separate; low object heap must fall back as well.
  ui.model.start(trailguide::Mode::Ask); ui.render(); lv_obj_update_layout(ui.scroll);
  lv_mem_monitor_t constrained; lv_mem_monitor(&constrained);
  void *pressure=lv_mem_alloc(constrained.free_biggest_size-1024);
  assert(pressure);
  {
    trailguide::WindowReveal probe;
    assert(!probe.capture(ui.scroll));
  }
  lv_mem_free(pressure);
  {
    std::string fixture;
    for(unsigned n=0;n<80;++n) fixture+=u8"Check café — complete UTF-8 copy. ";
    lv_label_set_text(ui.body,fixture.c_str()); lv_obj_update_layout(ui.scroll);
    assert(lv_obj_get_height(ui.body)>166 && fixture==lv_label_get_text(ui.body));
    trailguide::WindowReveal longWindow;
    assert(longWindow.capture(ui.scroll)); longWindow.render(3780);
    lv_obj_add_flag(ui.body,LV_OBJ_FLAG_HIDDEN);
    auto draw=[](lv_event_t *event) {
      auto *effect=static_cast<trailguide::WindowReveal *>(lv_event_get_user_data(event));
      lv_area_t area; lv_obj_get_coords(lv_event_get_target(event),&area);
      effect->draw(lv_event_get_draw_ctx(event),area);
    };
    lv_obj_add_event_cb(ui.scroll,draw,LV_EVENT_DRAW_MAIN,&longWindow);
    saveFrame("utf8-long-emerging");
    lv_obj_remove_event_cb(ui.scroll,draw);
    lv_obj_clear_flag(ui.body,LV_OBJ_FLAG_HIDDEN);
    longWindow.clear();
    lv_obj_scroll_to_y(ui.scroll,lv_obj_get_height(ui.body),LV_ANIM_OFF);
    assert(lv_obj_get_scroll_y(ui.scroll)>0 && fixture==lv_label_get_text(ui.body));
    saveFrame("utf8-long-scrolled");
  }
  // A failed caller-buffer allocation is explicitly instant text, NOT the effect.
  ui.model.home(); ui.render();
  failSnapshot=true;
  click(ui.first);
  assert(!snapshotAllocation && timers()==baseTimers);
  assert(!lv_obj_has_flag(ui.body,LV_OBJ_FLAG_HIDDEN));
  assert(std::strcmp(lv_label_get_text(ui.body),trailguide::cards[0].ask)==0);
  assert(std::strcmp(lv_label_get_text(lv_obj_get_child(ui.second,0)),"Another")==0);
  assert(feature(promptImage(),true).count>15000);
  failSnapshot=false;
  click(ui.backButton);
  // uint32 millisecond rollover: relative elapsed subtraction, never absolute deadlines.
  lv_tick_inc(0xfffffe00U-lv_tick_get());
  click(ui.second); advance(6000);
  assert(!snapshotAllocation && !lv_obj_has_flag(ui.body,LV_OBJ_FLAG_HIDDEN));
  click(ui.backButton);
  // Independent body and panel deletion must cancel before any stale callback.
  for(unsigned n=0;n<2;++n) {
    click(ui.first); advance(700);
    lv_obj_del(n?ui.scroll:ui.body);
    assert(!snapshotAllocation && timers()==baseTimers);
    advance(6100); ui.model.home(); ui.render();
  }
  {
    auto *original=lv_scr_act(); auto *other=lv_obj_create(nullptr); lv_scr_load(other);
    ui.render(); click(ui.first); assert(snapshotAllocation);
    lv_scr_load(original); lv_obj_del(other);
    assert(!snapshotAllocation && timers()==baseTimers);
    advance(6100); ui.model.home(); ui.render();
  }
  // Real LVGL input driver: held press does not draw until release, one release
  // skips the SAME card, a subsequent distinct release draws once, drag cancels.
  lv_indev_drv_t touchDriver; lv_indev_drv_init(&touchDriver);
  touchDriver.type=LV_INDEV_TYPE_POINTER; touchDriver.read_cb=readTouch;
  auto *touch=lv_indev_drv_register(&touchDriver);
  touchPoint={40,100}; touching=true; advance(500);
  assert(ui.model.view==trailguide::View::Home);
  touching=false; advance(40);
  assert(ui.model.view==trailguide::View::Card && ui.model.index==0 && snapshotAllocation);
  touchPoint={125,290}; touching=true; advance(200); touching=false; advance(40);
  assert(ui.model.index==0 && !snapshotAllocation);
  touching=true; advance(100); touching=false; advance(40);
  assert(ui.model.index==1 && snapshotAllocation);
  touchPoint={125,290}; touching=true; advance(50); touchPoint={80,180}; advance(100);
  touching=false; advance(40); assert(ui.model.index==1);
  ui.setUpdating(true);
  touchPoint={40,30}; touching=true; advance(50); touching=false; advance(40);
  assert(ui.model.view==trailguide::View::Card && ui.model.index==1);
  ui.setUpdating(false); advance(40); // Allow the normal released-input sample after overlay removal.
  ui.model.mode=trailguide::Mode::Try; ui.model.index=5; ui.render();
  touchPoint={80,225}; touching=true; advance(40);
  for(int y=215;y>=110;y-=15) { touchPoint={80,lv_coord_t(y)}; advance(40); }
  touching=false; advance(100);
  assert(lv_obj_get_scroll_y(ui.scroll)>0 && ui.model.index==5);
  lv_indev_delete(touch);
  // Repeated-cycle LVGL heap (not only malloc/timer counters) must stabilize.
  unsigned stableFree=0;
  for(unsigned cycle=0;cycle<100;++cycle) {
    ui.model.home(); ui.render(); click(ui.first); advance(80); click(ui.second);
    ui.model.home(); ui.render(); lv_refr_now(nullptr);
    lv_mem_monitor_t state; lv_mem_monitor(&state);
    if(cycle==0) stableFree=state.free_size;
    assert(state.free_size==stableFree && !snapshotAllocation);
  }
  // Export an ACTUAL LVGL framebuffer sequence, 25fps, not a browser substitute.
  click(ui.first);
  for(unsigned frame=0;frame<=150;++frame) {
    char name[48]; std::snprintf(name,sizeof(name),"motion-%03u",frame);
    saveFrame(name);
    if(frame<150) advance(40);
  }
  assert(!snapshotAllocation);
  selfTest(); assert(diagnosticPassed && !snapshotAllocation);
  failSnapshot=true; selfTest(); assert(diagnosticPassed && !snapshotAllocation);
  failSnapshot=false;
  lv_obj_clean(lv_scr_act());
  lv_refr_now(nullptr);
  lv_mem_monitor_t memory; lv_mem_monitor(&memory);
  std::printf("PASS native frames; %u caller-buffer allocations, zero remaining; LVGL used=%u sampled-peak=%u free-biggest=%u bytes\n",snapshotAllocations,unsigned(memory.total_size-memory.free_size),observedHeapPeak,unsigned(memory.free_biggest_size));
}
