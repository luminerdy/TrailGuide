#pragma once
#include <lvgl.h>

namespace v3 {
constexpr const char *kVersion = "v3-bringup-0.1.0";
inline bool heartbeat_due(uint32_t now, uint32_t &last) {
  if (static_cast<uint32_t>(now - last) < 5000) return false;
  last = now;
  return true;
}
class Ui {
 public:
  void begin() {
    auto *screen = lv_scr_act();
    lv_obj_clean(screen);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101418), 0);
    title_ = label(screen, "TrailGuide V3", 72, &lv_font_montserrat_22);
    label(screen, "Display / touch test", 112, &lv_font_montserrat_20);
    button_ = lv_btn_create(screen);
    lv_obj_clear_flag(button_, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_remove_style_all(button_);
    lv_obj_set_size(button_, 160, 64);
    lv_obj_set_pos(button_, 100, 156);
    lv_obj_set_style_radius(button_, 12, 0);
    lv_obj_set_style_bg_color(button_, lv_color_hex(0xf3b34c), 0);
    lv_obj_set_style_bg_opa(button_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(button_, lv_color_hex(0xc98928), LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(button_, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(button_, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(button_, lv_color_white(), LV_STATE_FOCUSED);
    auto *caption = label(button_, "Tap target", 0, &lv_font_montserrat_22);
    lv_obj_set_style_text_color(caption, lv_color_hex(0x101418), 0);
    lv_obj_center(caption);
    taps_ = 0;
    count_ = label(screen, "Taps: 0", 242, &lv_font_montserrat_22);
    lv_obj_add_event_cb(button_, clicked, LV_EVENT_CLICKED, this);
    label(screen, kVersion, 284, &lv_font_montserrat_16);
  }
  lv_obj_t *title() const { return title_; }
  uint32_t taps() const { return taps_; }
 private:
  static void clicked(lv_event_t *event) {
    auto *self = static_cast<Ui *>(lv_event_get_user_data(event));
    ++self->taps_;
    lv_label_set_text_fmt(self->count_, "Taps: %lu", static_cast<unsigned long>(self->taps_));
  }
  uint32_t taps_ = 0;
  static lv_obj_t *label(lv_obj_t *parent, const char *text, int y, const lv_font_t *font) {
    auto *obj = lv_label_create(parent);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
    lv_label_set_text(obj, text);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
    return obj;
  }
  lv_obj_t *title_ = nullptr;
  lv_obj_t *button_ = nullptr;
  lv_obj_t *count_ = nullptr;
};
}  // namespace v3
