#pragma once
#include <lvgl.h>
#include <string>
#include "model.h"
#include "reveal.h"
#include "cards_generated.h"
#include "version.h"

namespace trailguide {
class Ui {
 public:
  Ui() = default;
  Ui(const Ui &) = delete;
  Ui &operator=(const Ui &) = delete;
  ~Ui() {
    cancelReveal();
    if (first) lv_obj_del(first);
    if (second) lv_obj_del(second);
    if (backButton) lv_obj_del(backButton);
    if (scroll) lv_obj_del(scroll);
    if (overlay) lv_obj_del(overlay);
  }
  enum class Action { Ask, Try, Home, Guide, Another, Back };
  Model model;
  lv_obj_t *first = nullptr, *second = nullptr, *backButton = nullptr;
  lv_obj_t *body = nullptr, *scroll = nullptr;
  lv_obj_t *connectionLabel = nullptr, *overlay = nullptr;
  bool updating = false;
  bool connectionReady = false;

  void setConnection(bool ready) {
    if (connectionReady == ready) return;
    connectionReady = ready;
    if (connectionLabel) lv_label_set_text(connectionLabel, ready ? "Draft | WiFi/OTA" : "Draft | Offline");
  }
  void setUpdating(bool active) {
    if (active && revealTimer) finishReveal();
    updating = active;
    if (active && !overlay) {
      overlay = lv_obj_create(lv_layer_top());
      lv_obj_add_event_cb(overlay, objectDeleted, LV_EVENT_DELETE, this);
      lv_obj_remove_style_all(overlay);
      lv_obj_set_pos(overlay, 0, 0); lv_obj_set_size(overlay, 172, 320);
      lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
      lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);
      lv_obj_set_style_bg_opa(overlay, LV_OPA_COVER, 0);
      lv_obj_set_style_bg_color(overlay, lv_color_hex(0x101c24), 0);
      lv_obj_set_style_text_color(overlay, lv_color_hex(0xf3f5ee), 0);
      auto *message = label(overlay, "Updating...\nKeep power\nconnected.", 0, 0, 156, &lv_font_montserrat_20);
      lv_obj_set_style_text_align(message, LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_center(message);
    } else if (!active && overlay) {
      lv_obj_del(overlay); overlay = nullptr;
      if (auto *input = lv_indev_get_next(nullptr)) lv_indev_wait_release(input);
    }
  }

  void render() {
    // Rerenders and return navigation show complete copy, never replay a draw.
    cancelReveal();
    auto *root = lv_scr_act();
    lv_obj_clean(root);
    first = second = backButton = body = scroll = nullptr;
    connectionLabel = nullptr;
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(0x101c24), 0);
    lv_obj_set_style_text_color(root, lv_color_hex(0xf3f5ee), 0);
    lv_obj_set_style_text_font(root, &lv_font_montserrat_18, 0);
    if (model.view == View::Home) {
      label(root, "TrailGuide", 6, 12, 160, &lv_font_montserrat_22);
      connectionLabel = label(root, connectionReady ? "Draft | WiFi/OTA" : "Draft | Offline", 6, 44, 160, &lv_font_montserrat_14);
      first = button(Action::Ask, "Ask the Team", 6, 78, 160, 56);
      second = button(Action::Try, "Try a Move", 6, 146, 160, 56);
      label(root, "8 paired cards\nGuide me for help\nNo saving or tracking", 6, 224, 160, &lv_font_montserrat_14);
      label(root, TRAILGUIDE_VERSION, 6, 297, 160, &lv_font_montserrat_14);
      return;
    }
    const auto &card = cards[model.index];
    backButton = button(model.view == View::Guide ? Action::Back : Action::Home,
                        model.view == View::Guide ? "Back" : "Home", 6, 6, 60, 48);
    label(root, model.view == View::Guide ? "Guide me\nDRAFT" :
          (model.mode == Mode::Ask ? "ASK\nDRAFT" : "TRY\nDRAFT"),
          76, 10, 90, &lv_font_montserrat_14);
    if (model.view == View::Guide) {
      label(root, "Swipe up for more", 6, 60, 160, &lv_font_montserrat_14);
      textPanel(82, 232, model.mode == Mode::Ask ? card.askGuide : card.tryGuide,
                &lv_font_montserrat_18);
    } else {
      label(root, card.theme, 6, 62, 160, &lv_font_montserrat_16);
      textPanel(86, 166, model.mode == Mode::Ask ? card.ask : card.action,
                &lv_font_montserrat_20);
      first = button(Action::Guide, "Guide me", 6, 264, 76, 50);
      second = button(Action::Another, "Another", 90, 264, 76, 50);
    }
  }

  void perform(Action action) {
    if (updating) return;
    if (action == Action::Another && revealTimer) { finishReveal(); return; }
    switch (action) {
      case Action::Ask: model.start(Mode::Ask); break;
      case Action::Try: model.start(Mode::Try); break;
      case Action::Home: model.home(); break;
      case Action::Guide: model.guide(); break;
      case Action::Another: model.next(); break;
      case Action::Back: model.back(); break;
    }
    render();
    // Only explicit fresh draws animate; Guide and Back remain instant.
    if (model.view == View::Card &&
        (action == Action::Ask || action == Action::Try || action == Action::Another)) startReveal();
    if (auto *input = lv_indev_get_act()) lv_indev_wait_release(input);
  }

 private:
  lv_timer_t *revealTimer = nullptr;
  RevealText reveal;
  uint32_t revealStarted = 0;
  void cancelReveal() {
    // LVGL callbacks run serially in lv_timer_handler; deletion removes the
    // callback before any old object/context can be reused. No deferred work.
    if (revealTimer) { lv_timer_del(revealTimer); revealTimer = nullptr; }
  }
  void startReveal() {
    reveal = RevealText(model.mode == Mode::Ask ? cards[model.index].ask : cards[model.index].action);
    revealStarted = lv_tick_get();
    lv_label_set_text(body, "");
    revealTimer = lv_timer_create(revealTick, 20, this);
    revealCaption("Reveal\nall");
  }
  void revealCaption(const char *text) {
    auto *caption = lv_obj_get_child(second, 0);
    lv_label_set_text(caption, text);
    lv_obj_center(caption);
  }
  void finishReveal() {
    lv_label_set_text(body, reveal.text());
    cancelReveal();
    revealCaption("Another");
  }
  static void revealTick(lv_timer_t *timer) {
    auto *self = static_cast<Ui *>(timer->user_data);
    const uint32_t elapsed = lv_tick_elaps(self->revealStarted);
    if (elapsed < RevealText::settleMs) return;
    if (elapsed >= RevealText::completeMs) {
      self->finishReveal();
      return;
    }
    const std::string prefix(self->reveal.text(), self->reveal.prefixBytes(elapsed));
    lv_label_set_text(self->body, prefix.c_str());
  }
  static void objectDeleted(lv_event_t *event) {
    auto *self = static_cast<Ui *>(lv_event_get_user_data(event));
    auto *object = lv_event_get_target(event);
    if (object == self->body) { self->cancelReveal(); self->body = nullptr; }
    if (object == self->scroll) self->scroll = nullptr;
    if (object == self->first) self->first = nullptr;
    if (object == self->second) {
      // The finish control can also be removed independently of its label.
      if (self->revealTimer) {
        if (self->body) lv_label_set_text(self->body, self->reveal.text());
        self->cancelReveal();
      }
      self->second = nullptr;
    }
    if (object == self->backButton) self->backButton = nullptr;
    if (object == self->overlay) self->overlay = nullptr;
  }
  struct Slot { Ui *owner; Action action; } slots[6]{};
  static void clicked(lv_event_t *event) {
    auto *slot = static_cast<Slot *>(lv_event_get_user_data(event));
    slot->owner->perform(slot->action);
  }
  static lv_obj_t *label(lv_obj_t *parent, const char *text, int x, int y,
                         int width, const lv_font_t *font) {
    auto *item = lv_label_create(parent);
    lv_obj_set_pos(item, x, y);
    lv_obj_set_width(item, width);
    lv_label_set_long_mode(item, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(item, font, 0);
    lv_obj_set_style_text_line_space(item, 4, 0);
    lv_label_set_text(item, text);
    return item;
  }
  lv_obj_t *button(Action action, const char *text, int x, int y, int w, int h) {
    auto *item = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(item);
    lv_obj_set_pos(item, x, y);
    lv_obj_set_size(item, w, h);
    lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x285066), 0);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x407994), LV_STATE_PRESSED);
    lv_obj_set_style_radius(item, 8, 0);
    auto *caption = label(item, text, 0, 0, w - 8, &lv_font_montserrat_16);
    lv_obj_set_style_text_align(caption, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(caption);
    auto &slot = slots[static_cast<unsigned>(action)];
    slot = {this, action};
    lv_obj_add_event_cb(item, clicked, LV_EVENT_CLICKED, &slot);
    lv_obj_add_event_cb(item, objectDeleted, LV_EVENT_DELETE, this);
    return item;
  }
  void textPanel(int y, int height, const char *text, const lv_font_t *font) {
    scroll = lv_obj_create(lv_scr_act());
    lv_obj_add_event_cb(scroll, objectDeleted, LV_EVENT_DELETE, this);
    lv_obj_remove_style_all(scroll);
    lv_obj_set_pos(scroll, 6, y);
    lv_obj_set_size(scroll, 160, height);
    lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_bg_opa(scroll, LV_OPA_COVER, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(scroll, lv_color_hex(0xa5bdc8), LV_PART_SCROLLBAR);
    lv_obj_set_style_width(scroll, 3, LV_PART_SCROLLBAR);
    body = label(scroll, text, 0, 0, 152, font);
    lv_obj_add_event_cb(body, objectDeleted, LV_EVENT_DELETE, this);
  }
};
}
