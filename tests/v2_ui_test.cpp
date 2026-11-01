#include "../trailguide_v2/ui.h"
#include <cassert>
#include <cstring>
#include "../trailguide_v2/reveal.h"
#include <type_traits>
#include <string>
static lv_color_t pixels[172 * 40];
static void flush(lv_disp_drv_t *driver, const lv_area_t *, lv_color_t *) { lv_disp_flush_ready(driver); }
static void click(lv_obj_t *button) { assert(button); lv_event_send(button, LV_EVENT_CLICKED, nullptr); lv_obj_update_layout(lv_scr_act()); }
static void advance(unsigned ms) { for (unsigned n = 0; n < ms; n += 10) { lv_tick_inc(10); lv_timer_handler(); } lv_obj_update_layout(lv_scr_act()); }
static unsigned timers() { unsigned count = 0; for (auto *t = lv_timer_get_next(nullptr); t; t = lv_timer_get_next(t)) ++count; return count; }
static void bounds(lv_obj_t *button) {
  assert(button); lv_area_t a; lv_obj_get_coords(button, &a);
  assert(a.x1 >= 0 && a.y1 >= 0 && a.x2 < 172 && a.y2 < 320);
  assert(lv_obj_get_width(button) >= 48 && lv_obj_get_height(button) >= 48);
}
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
  lv_init(); lv_disp_draw_buf_t buffer; lv_disp_draw_buf_init(&buffer, pixels, nullptr, 172 * 40);
  lv_disp_drv_t driver; lv_disp_drv_init(&driver); driver.hor_res = 172; driver.ver_res = 320; driver.draw_buf = &buffer; driver.flush_cb = flush; lv_disp_drv_register(&driver);
  std::string longText;
  for (unsigned n = 0; n < 6000; ++n) longText += u8"é中🌱 ";
  for (const auto &text : {std::string(), std::string(u8"é中🌱"), longText}) {
    trailguide::RevealText reveal(text.c_str());
    size_t previous = 0;
    auto *label = lv_label_create(lv_scr_act());
    for (unsigned elapsed = 0; elapsed <= 3000; elapsed += 10) {
      const auto bytes = reveal.prefixBytes(elapsed);
      assert(bytes >= previous && bytes <= text.size());
      if (elapsed < 160) assert(bytes == 0);
      const auto prefix = text.substr(0, bytes);
      assert(utf8Complete(prefix));
      if (text.size() < 100) { // LVGL's fixed 48KB heap cannot hold a 60KB label.
        lv_label_set_text(label, prefix.c_str());
        assert(prefix == lv_label_get_text(label));
      }
      previous = bytes;
    }
    assert(previous == text.size());
    assert(reveal.prefixBytes(0xffffffffU) == text.size());
    lv_obj_del(label);
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
      assert(std::strcmp(lv_label_get_text(ui.body), "") == 0); // Fresh draw settles first.
      advance(140); assert(std::strcmp(lv_label_get_text(ui.body), "") == 0);
      advance(360);
      assert(utf8Complete(lv_label_get_text(ui.body)));
      const auto partial = std::strlen(lv_label_get_text(ui.body));
      assert(partial > 0 && partial < std::strlen(mode == 0 ? card.ask : card.action));
      advance(2440);
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) != 0);
      advance(20); // Complete at 2960ms, including the 160ms settle.
      assert(std::strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0);
      assert(lv_obj_get_style_text_font(ui.body, 0) == &lv_font_montserrat_20);
      assert(lv_obj_get_width(ui.body) == 152);
      if (lv_obj_get_height(ui.body) > lv_obj_get_height(ui.scroll)) {
        lv_obj_scroll_to_y(ui.scroll, lv_obj_get_height(ui.body), LV_ANIM_OFF);
        assert(lv_obj_get_scroll_y(ui.scroll) > 0);
      }
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
    temporary.render();
    temporary.perform(trailguide::Ui::Action::Another); advance(300);
  } // Destruction must delete the active timer and owner callbacks too.
  assert(timers() == baseTimers);
  advance(3100);
  lv_obj_clean(lv_scr_act());
  lv_refr_now(nullptr);
}
