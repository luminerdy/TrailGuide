// Original Waveshare ESP32-S3-Touch-LCD-1.85 Rev2.1, display/touch only.
// Prepare pinned drivers and the reviewed DMA safety adapter with tools/prepare_v3_bringup.py.
#include <Arduino.h>
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "V3 bring-up requires the ESP32-S3 target; never use V1/V2 binaries or pins"
#endif
#include "LVGL_Driver.h"
#include "ui.h"

static v3::Ui ui;
static bool ready = false;
static uint32_t last_heartbeat = 0;

static bool acknowledges(TwoWire &bus, uint8_t address) {
  bus.beginTransmission(address);
  return bus.endTransmission() == 0;
}

void setup() {
  Serial.begin(115200);  // Never wait for a connected USB host.
  Serial.printf("V3_START version=%s runtime_compatibility=unverified\n", v3::kVersion);
  I2C_Init();
  Wire.setTimeOut(50);
  if (!acknowledges(Wire, TCA9554_ADDRESS)) {
    Serial.println("V3_BUS expander_ack=0 touch_ack=not_tested; startup stopped");
    return;
  }
  // Only reset outputs EXIO1/EXIO2 become outputs; preserve other directions.
  Set_EXIO(EXIO_PIN1, High);
  Set_EXIO(EXIO_PIN2, High);
  TCA9554PWR_Init(Read_REG(TCA9554_CONFIG_REG) & 0xfc);
  Wire1.begin(CST816_SDA_PIN, CST816_SCL_PIN, I2C_MASTER_FREQ_HZ);
  Wire1.setTimeOut(50);
  CST816_Touch_Reset();  // EXIO1, not native GPIO1.
  const bool touch_ack = acknowledges(Wire1, CST816_ADDR);
  Serial.printf("V3_BUS expander_ack=1 touch_ack=%u after_EXIO1_reset; ACK is not pixels or physical touch\n", touch_ack);
  if (!touch_ack) return; // Vendor read routines do not safely propagate missing-device errors.
  LCD_Init(); // Exact vendor path: EXIO2 LCD reset/panel init, then Touch_Init/EXIO1 resets.
  const bool expander_after = acknowledges(Wire, TCA9554_ADDRESS);
  const bool touch_after = acknowledges(Wire1, CST816_ADDR);
  Serial.printf("V3_BUS after_vendor_resets expander_ack=%u touch_ack=%u; ACK is not pixels or physical touch\n", expander_after, touch_after);
  if (!expander_after || !touch_after) return;
  Backlight_Init();
  Lvgl_Init(); // Vendor esp_timer supplies lv_tick_inc; LV_TICK_CUSTOM must stay zero.
  ui.begin(); // Removes the vendor's temporary greeting before the first GUI refresh.
  ready = true;
  Serial.printf("V3_UI version=%s initialized; pixels_and_physical_touch=unverified; synthetic_selftest=not_run\n", v3::kVersion);
}

void loop() {
  if (ready) Lvgl_Loop();
  const uint32_t now = millis();
  if (v3::heartbeat_due(now, last_heartbeat)) {
    Serial.printf("V3_HEARTBEAT version=%s uptime_ms=%lu tapcount=%lu ui_ready=%u synthetic_selftest=not_run\n",
                  v3::kVersion, static_cast<unsigned long>(now),
                  static_cast<unsigned long>(ui.taps()), ready);
  }
  delay(5);
}
