#include "../trailguide_v3_bringup/ui.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <initializer_list>
#include <fstream>

static lv_color_t rendered[360 * 360];
static lv_color_t pixels[360 * 40];
static void flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *colors) {
  for (int y = area->y1; y <= area->y2; ++y)
    for (int x = area->x1; x <= area->x2; ++x) rendered[y * 360 + x] = *colors++;
  lv_disp_flush_ready(driver);
}
static bool pressed = false;
static lv_point_t point = {180, 188};
static void input(lv_indev_drv_t *, lv_indev_data_t *data) {
  data->point = point;
  data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}
static void advance(unsigned ms) {
  for (unsigned n = 0; n < ms; n += 10) { lv_tick_inc(10); lv_timer_handler(); }
}
static const char *count_text() {
  for (uint32_t i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); ++i) {
    auto *obj = lv_obj_get_child(lv_scr_act(), i);
    if (lv_obj_check_type(obj, &lv_label_class) && std::strncmp(lv_label_get_text(obj), "Taps:", 5) == 0)
      return lv_label_get_text(obj);
  }
  return "missing";
}
int main() {
  static_assert(LV_TICK_CUSTOM == 0 && LV_FONT_MONTSERRAT_20 && LV_FONT_MONTSERRAT_22,
                "Use the same esp_timer tick/font configuration as the S3 build");
  static_assert(LVGL_VERSION_MAJOR == 8 && LVGL_VERSION_MINOR == 3 && LVGL_VERSION_PATCH == 10,
                "This test must use the target-bundled LVGL 8.3.10");
  lv_init();
  lv_disp_draw_buf_t buffer;
  lv_disp_draw_buf_init(&buffer, pixels, nullptr, 360 * 40);
  lv_disp_drv_t driver;
  lv_disp_drv_init(&driver);
  driver.hor_res = 360; driver.ver_res = 360;
  driver.draw_buf = &buffer; driver.flush_cb = flush;
  lv_disp_drv_register(&driver);
  v3::Ui ui;
  ui.begin();
  assert(std::strcmp(lv_label_get_text(ui.title()), "TrailGuide V3") == 0);
  lv_obj_update_layout(lv_scr_act());
  const char *copy[] = {"TrailGuide V3", "Display / touch test", "v3-bringup-0.1.0", "Taps: 0", "Tap target"};
  for (const auto *text : copy) {
    bool found = false;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); ++i) {
      auto *obj = lv_obj_get_child(lv_scr_act(), i);
      if (lv_obj_check_type(obj, &lv_label_class) && std::strcmp(lv_label_get_text(obj), text) == 0) found = true;
      for (uint32_t j = 0; j < lv_obj_get_child_cnt(obj); ++j) {
        auto *child = lv_obj_get_child(obj, j);
        if (lv_obj_check_type(child, &lv_label_class) && std::strcmp(lv_label_get_text(child), text) == 0) found = true;
      }
    }
    if (!found) std::fprintf(stderr, "Missing UI text: %s\n", text);
    assert(found);
  }
  assert(lv_color_to32(lv_obj_get_style_bg_color(lv_scr_act(), LV_PART_MAIN)) == 0xff101418);
  for (uint32_t i = 0; i < lv_obj_get_child_cnt(lv_scr_act()); ++i) {
    auto *obj = lv_obj_get_child(lv_scr_act(), i);
    const bool button = lv_obj_check_type(obj, &lv_btn_class);
    if (button) {
      assert(lv_obj_get_width(obj) >= 48 && lv_obj_get_height(obj) >= 48);
      lv_obj_add_state(obj, LV_STATE_FOCUSED | LV_STATE_PRESSED);
      lv_obj_update_layout(lv_scr_act());
    }
    lv_area_t a; lv_obj_get_coords(obj, &a);
    const int extra = button ? 4 : 0; // Include focus outline and offset.
    for (int x : {a.x1 - extra, a.x2 + extra}) for (int y : {a.y1 - extra, a.y2 + extra})
      assert((x - 180)*(x - 180) + (y - 180)*(y - 180) <= 168*168);
    assert(lv_obj_get_style_shadow_width(obj, LV_PART_MAIN) == 0);
    if (button) lv_obj_clear_state(obj, LV_STATE_FOCUSED | LV_STATE_PRESSED);
  }
  lv_refr_now(nullptr);
  unsigned white = 0, amber = 0;
  std::ofstream frame("build/v3-s3-bringup/minimal/ui-initial.ppm", std::ios::binary);
  assert(frame);
  frame << "P6\n360 360\n255\n";
  for (int y = 0; y < 360; ++y) for (int x = 0; x < 360; ++x) {
    lv_color32_t c; c.full = lv_color_to32(rendered[y * 360 + x]);
    frame.put(static_cast<char>(c.ch.red)); frame.put(static_cast<char>(c.ch.green)); frame.put(static_cast<char>(c.ch.blue));
    if (c.ch.red > 230 && c.ch.green > 230 && c.ch.blue > 230) ++white;
    if (c.ch.red > 200 && c.ch.green > 110 && c.ch.green < 210 && c.ch.blue < 110) ++amber;
    if (c.ch.red > 150) assert((x - 180)*(x - 180) + (y - 180)*(y - 180) <= 168*168);
  }
  assert(white > 500 && amber > 5000); // Actual rendered text/button, not labels alone.
  lv_indev_drv_t indev;
  lv_indev_drv_init(&indev);
  indev.type = LV_INDEV_TYPE_POINTER; indev.read_cb = input;
  lv_indev_drv_register(&indev);
  advance(100);
  pressed = true; advance(100);
  assert(std::strcmp(count_text(), "Taps: 0") == 0);
  advance(2400); // Real LVGL long-press/repeat path, not a direct synthetic click.
  assert(std::strcmp(count_text(), "Taps: 0") == 0);
  pressed = false; advance(100);
  assert(std::strcmp(count_text(), "Taps: 1") == 0);
  for (unsigned n = 0; n < 99; ++n) {
    pressed = true; advance(60); pressed = false; advance(60);
  }
  assert(std::strcmp(count_text(), "Taps: 100") == 0);
  point = {10, 10}; pressed = true; advance(100); pressed = false; advance(100);
  assert(std::strcmp(count_text(), "Taps: 100") == 0);
  point = {180, 188}; pressed = true; advance(100); point = {10, 10}; advance(100);
  pressed = false; advance(100);
  assert(std::strcmp(count_text(), "Taps: 100") == 0); // Drag-out cancels click.
  assert(ui.taps() == 100);
  ui.begin();
  assert(ui.taps() == 0);
  uint32_t last = 0;
  assert(!v3::heartbeat_due(4999, last));
  assert(v3::heartbeat_due(5000, last) && last == 5000);
  assert(!v3::heartbeat_due(5001, last));
  last = UINT32_MAX - 999;
  assert(!v3::heartbeat_due(3999, last));
  assert(v3::heartbeat_due(4000, last) && last == 4000);
  assert(std::strcmp(count_text(), "Taps: 0") == 0);
  puts("PASS: bundled LVGL 8.3.10 copy/geometry; synthetic pointer press/hold/release, 100 clicks, outside/drag cancel, reset");
}
