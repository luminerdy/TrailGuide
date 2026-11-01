// TrailGuide portrait ESP32-C6 hardware trial. Draft content; optional authenticated local OTA.
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "esp_lcd_touch_axs5106l.h"
#include "ui.h"
#include "ota_support.h"
#ifdef TRAILGUIDE_WIFI_CONFIGURED
#include <WebServer.h>
#include <esp_ota_ops.h>
static WebServer statusServer(80);
static bool statusStarted = false;
#endif
#if !defined(CONFIG_IDF_TARGET_ESP32C6)
#error "TrailGuide V2 requires the identified ESP32-C6 target, not V1 or V3"
#endif

Arduino_DataBus *bus = new Arduino_HWSPI(15, 14, 1, 2);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 22, 0, false, 172, 320, 34, 0, 34, 0);
#include "vendor_panel_init.h"

static trailguide::Ui ui;
static lv_color_t pixels[172 * 40];
static lv_disp_draw_buf_t drawBuffer;
static lv_disp_drv_t displayDriver;
static lv_indev_drv_t inputDriver;

static void updateOverlay() {
  ui.setUpdating(updateInProgress);
  lv_refr_now(nullptr);
}
static void beginStatus() {
#ifdef TRAILGUIDE_WIFI_CONFIGURED
  statusServer.on("/status", HTTP_GET, []() {
    char response[220];
    const esp_partition_t *partition = esp_ota_get_running_partition();
    snprintf(response, sizeof(response),
      "{\"device\":\"trailguide-v2\",\"version\":\"%s\",\"ota_ready\":%s,\"uptime_ms\":%lu,\"slot\":\"0x%lx\"}",
      TRAILGUIDE_VERSION, otaReady ? "true" : "false", static_cast<unsigned long>(millis()),
      static_cast<unsigned long>(partition ? partition->address : 0));
    statusServer.sendHeader("Cache-Control", "no-store");
    statusServer.send(200, "application/json", response);
  });
#endif
}
static void serviceStatus() {
#ifdef TRAILGUIDE_WIFI_CONFIGURED
  if (otaReady && !statusStarted) { statusServer.begin(); statusStarted = true; }
  if (!otaReady && statusStarted) { statusServer.stop(); statusStarted = false; }
  if (statusStarted) statusServer.handleClient();
#endif
}

static void fatal(const char *message) {
  Serial.println(message);
  while (true) delay(1000);
}
static void flush(lv_disp_drv_t *driver, const lv_area_t *area, lv_color_t *colors) {
  gfx->draw16bitRGBBitmap(area->x1, area->y1, reinterpret_cast<uint16_t *>(colors),
                         area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
  lv_disp_flush_ready(driver);
}
static void readTouch(lv_indev_drv_t *, lv_indev_data_t *data) {
  touch_data_t point{};
  bsp_touch_read();
  const bool down = bsp_touch_get_coordinates(&point);
  if (down && point.coords[0].x < 172 && point.coords[0].y < 320) {
    data->point.x = point.coords[0].x;
    data->point.y = point.coords[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else data->state = LV_INDEV_STATE_RELEASED;
}
static bool buttonBounds(lv_obj_t *button) {
  if (!button) return false;
  lv_area_t a; lv_obj_get_coords(button, &a);
  return a.x1 >= 0 && a.y1 >= 0 && a.x2 < 172 && a.y2 < 320 &&
         lv_obj_get_width(button) >= 48 && lv_obj_get_height(button) >= 48;
}
static void click(lv_obj_t *button) {
  if (button) lv_event_send(button, LV_EVENT_CLICKED, nullptr);
  lv_obj_update_layout(lv_scr_act());
}
// Explicit USB diagnostic; synthetic UI events are not physical touch acceptance.
static void selfTest() {
  using namespace trailguide;
  bool ok = true;
  ui.model.home(); ui.render(); lv_obj_update_layout(lv_scr_act());
  ok &= buttonBounds(ui.first) && buttonBounds(ui.second);
  for (unsigned mode = 0; mode < 2; ++mode) {
    click(mode == 0 ? ui.first : ui.second);
    for (unsigned n = 0; n < Model::count; ++n) {
      const auto &card = cards[n];
      ok &= ui.model.view == View::Card && ui.model.index == n;
      ok &= ui.model.mode == (mode == 0 ? Mode::Ask : Mode::Try);
      ok &= buttonBounds(ui.first) && buttonBounds(ui.second) && buttonBounds(ui.backButton);
      click(ui.second); // Reveal all completes this draw, without advancing the model.
      ok &= ui.model.view == View::Card && ui.model.index == n;
      ok &= ui.body && strcmp(lv_label_get_text(ui.body), mode == 0 ? card.ask : card.action) == 0;
      click(ui.first);
      ok &= ui.model.view == View::Guide && buttonBounds(ui.backButton);
      ok &= ui.body && strcmp(lv_label_get_text(ui.body), mode == 0 ? card.askGuide : card.tryGuide) == 0;
      ok &= ui.body && ui.scroll && lv_obj_get_height(ui.body) > lv_obj_get_height(ui.scroll);
      click(ui.backButton);
      ok &= ui.model.view == View::Card && ui.model.index == n;
      click(ui.second);
    }
    ok &= ui.model.index == 0;
    click(ui.backButton);
    ok &= ui.model.view == View::Home;
  }
  ui.model.home(); ui.render(); lv_refr_now(nullptr);
  Serial.println(ok ? "SELFTEST PASS: both modes, 8 cards, 16 guides, bounds, full text, Back and Another" : "SELFTEST FAIL");
}

void setup() {
  Serial.begin(115200);
  Serial.println("TrailGuide " TRAILGUIDE_VERSION " DRAFT");
  if (!gfx->begin()) fatal("FATAL: display begin failed");
  lcd_reg_init(); gfx->setRotation(0); gfx->fillScreen(RGB565_BLACK);
  pinMode(23, OUTPUT); digitalWrite(23, HIGH);
  if (gfx->width() != 172 || gfx->height() != 320) fatal("FATAL: portrait dimensions mismatch");
  if (!Wire.begin(18, 19)) fatal("FATAL: touch I2C begin failed");
  bsp_touch_init(&Wire, 20, 21, 0, 172, 320);
  Wire.beginTransmission(0x63);
  if (Wire.endTransmission() != 0) fatal("FATAL: touch controller did not acknowledge");
  lv_init();
  lv_disp_draw_buf_init(&drawBuffer, pixels, nullptr, 172 * 40);
  lv_disp_drv_init(&displayDriver);
  displayDriver.hor_res = 172; displayDriver.ver_res = 320;
  displayDriver.draw_buf = &drawBuffer; displayDriver.flush_cb = flush;
  if (!lv_disp_drv_register(&displayDriver)) fatal("FATAL: LVGL display registration failed");
  lv_indev_drv_init(&inputDriver); inputDriver.type = LV_INDEV_TYPE_POINTER;
  inputDriver.read_cb = readTouch;
  if (!lv_indev_drv_register(&inputDriver)) fatal("FATAL: LVGL input registration failed");
  ui.render(); lv_refr_now(nullptr);
  onUpdateState = updateOverlay;
  beginStatus();
  beginUpdates();
  Serial.printf("READY: 172x320 Home; 8 DRAFT pairs; offline; free heap=%u\n", ESP.getFreeHeap());
  Serial.println("No saving or tracking. USB T runs synthetic UI diagnostics; local OTA is optional.");
}
void loop() {
  lv_timer_handler();
  serviceUpdates();
  ui.setConnection(otaReady);
  serviceStatus();
  if (Serial.available()) {
    const int command = Serial.read();
    if (command == 'T') selfTest();
#if defined(TRAILGUIDE_BENCH_TESTS) && defined(TRAILGUIDE_WIFI_CONFIGURED)
    if (command == 'N') {
      WiFi.disconnect(false, false);
      wifiAttemptAt = millis();
      Serial.println("BENCH: Wi-Fi disconnected; retry is due in thirty seconds.");
    }
#endif
  }
  static int oldView = -1, oldMode = -1, oldIndex = -1;
  const int view = static_cast<int>(ui.model.view), mode = static_cast<int>(ui.model.mode);
  const int index = static_cast<int>(ui.model.index);
  if (view != oldView || mode != oldMode || index != oldIndex) {
    Serial.printf("UI view=%d mode=%s card=%s\n", view, mode == 0 ? "ASK" : "TRY",
                  ui.model.hasCard() ? trailguide::cards[index].id : "none");
    oldView = view; oldMode = mode; oldIndex = index;
  }
  delay(5);
}
