// Original Waveshare ESP32-S3-Touch-LCD-1.85 Rev2.1, silent display/touch + optional IMU diagnostics.
// Prepare pinned drivers and the reviewed DMA safety adapter with tools/prepare_v3_bringup.py.
#include <Arduino.h>
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "V3 bring-up requires the ESP32-S3 target; never use V1/V2 binaries or pins"
#endif
#include "LVGL_Driver.h"
#include "ui.h"
#include "imu.h"

static v3::Ui ui;
static v3::Imu<TwoWire> imu(Wire);
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
  // Existing shared expander/IMU bus (SDA11/SCL10), not the touch Wire1 bus.
  // Each IMU tick does at most two bounded register operations, no waits/retries.
  Wire.setTimeOut(10);
  imu.begin(millis());
  Serial.printf("V3_UI version=%s initialized; pixels_and_physical_touch=unverified; synthetic_selftest=not_run\n", v3::kVersion);
}

void loop() {
  if (ready) Lvgl_Loop();
  const uint32_t now = millis();
  if (ready) {
    const auto before = imu.state();
    imu.tick(now);
    ui.set_imu_status(imu.status());
    if (before == v3::ImuState::Probing && imu.state() != before) {
      for (unsigned i = 0; i < 2; ++i) {
        const auto &c = imu.candidate(i);
        if (c.read_ok)
          Serial.printf("V3_IMU_PROBE address=0x%02x WHO_AM_I=0x%02x expected=0x05 match=%u suffix=unidentified\n", 0x6a+i, c.id, c.id==5);
        else
          Serial.printf("V3_IMU_PROBE address=0x%02x WHO_AM_I=unreadable; no identity claim\n", 0x6a+i);
      }
    }
    if (before != v3::ImuState::Running && imu.state() == v3::ImuState::Running) {
      Serial.printf("V3_IMU_CONFIG address=0x%02x WHO_AM_I=0x05 revision=0x%02x readback=ok acc_range_g=4 gyr_range_dps=512 odr_enum=normal60 reset=not_issued suffix=unidentified\n",
                    imu.address(), imu.revision());
    }
  }
  if (v3::heartbeat_due(now, last_heartbeat)) {
    Serial.printf("V3_HEARTBEAT version=%s uptime_ms=%lu tapcount=%lu ui_ready=%u imu_status=\"%s\" imu_sample_valid=%u imu_read_errors=%lu synthetic_selftest=not_run\n",
                  v3::kVersion, static_cast<unsigned long>(now),
                  static_cast<unsigned long>(ui.taps()), ready, imu.status(),
                  imu.sample().valid, static_cast<unsigned long>(imu.read_errors()));
    const auto &sample = imu.sample();
    if (sample.valid) {
      Serial.printf("V3_IMU_SAMPLE source=physical_bus sample_ms=%lu age_ms=%lu acc_g=%.4f,%.4f,%.4f gyr_dps=%.3f,%.3f,%.3f axes=sensor_native uncalibrated=1\n",
                    static_cast<unsigned long>(sample.at_ms), static_cast<unsigned long>(now-sample.at_ms),
                    sample.acc_g[0],sample.acc_g[1],sample.acc_g[2],
                    sample.gyr_dps[0],sample.gyr_dps[1],sample.gyr_dps[2]);
    } // No numeric sensor output for absent, short, invalid or stale reads.
  }
  delay(5);
}
