#include "../trailguide_v2/ui.h"
#include <cassert>
#include <cstring>
static lv_color_t pixels[172 * 40];
static void flush(lv_disp_drv_t *driver, const lv_area_t *, lv_color_t *) { lv_disp_flush_ready(driver); }
static void click(lv_obj_t *button) { assert(button); lv_event_send(button, LV_EVENT_CLICKED, nullptr); lv_obj_update_layout(lv_scr_act()); }
static void bounds(lv_obj_t *button) {
  assert(button); lv_area_t a; lv_obj_get_coords(button, &a);
  assert(a.x1 >= 0 && a.y1 >= 0 && a.x2 < 172 && a.y2 < 320);
  assert(lv_obj_get_width(button) >= 48 && lv_obj_get_height(button) >= 48);
}
int main() {
  lv_init(); lv_disp_draw_buf_t buffer; lv_disp_draw_buf_init(&buffer, pixels, nullptr, 172 * 40);
  lv_disp_drv_t driver; lv_disp_drv_init(&driver); driver.hor_res = 172; driver.ver_res = 320; driver.draw_buf = &buffer; driver.flush_cb = flush; lv_disp_drv_register(&driver);
  trailguide::Ui ui; ui.render(); lv_obj_update_layout(lv_scr_act());
  assert(ui.model.view == trailguide::View::Home); bounds(ui.first); bounds(ui.second);
  for (unsigned mode = 0; mode < 2; ++mode) {
    click(mode == 0 ? ui.first : ui.second);
    for (unsigned n = 0; n < 8; ++n) {
      assert(ui.model.view == trailguide::View::Card); assert(ui.model.index == n);
      assert(ui.model.mode == (mode == 0 ? trailguide::Mode::Ask : trailguide::Mode::Try));
      bounds(ui.backButton); bounds(ui.first); bounds(ui.second);
      const auto &card = trailguide::cards[n];
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0);
      click(ui.first); assert(ui.model.view == trailguide::View::Guide); bounds(ui.backButton);
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.askGuide : card.tryGuide) == 0);
      assert(lv_obj_get_height(ui.body) > lv_obj_get_height(ui.scroll));
      lv_obj_scroll_to_y(ui.scroll, lv_obj_get_height(ui.body), LV_ANIM_OFF);
      assert(lv_obj_get_scroll_y(ui.scroll) > 0);
      click(ui.backButton); assert(ui.model.index == n);
      click(ui.second);
    }
    assert(ui.model.index == 0); click(ui.backButton); assert(ui.model.view == trailguide::View::Home);
  }
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
  lv_refr_now(nullptr);
}
