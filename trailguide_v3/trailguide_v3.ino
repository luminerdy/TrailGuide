// Original-family Rev2.1 read-only DRAFT app. No WiFi/OTA/audio/storage.
// Prepare pinned drivers and the reviewed DMA safety adapter with tools/prepare_v3.py.
#include <Arduino.h>
#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error "V3 bring-up requires the ESP32-S3 target; never use V1/V2 binaries or pins"
#endif
#include "LVGL_Driver.h"
#include "ui.h"
#include "imu.h"
#include "motion_diagnostics.h"

#include <esp_heap_caps.h>
#include <esp_system.h>
static uint32_t randomWord(void *) { return esp_random(); }
// Never consume the internal DMA/object heap for a large reveal raster.
static void *rasterAllocate(size_t n) { return heap_caps_malloc(n, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); }
static void rasterRelease(void *p) { heap_caps_free(p); }
static v3app::Ui ui(randomWord,nullptr,rasterAllocate,rasterRelease);
static v3::Imu<TwoWire> imu(Wire);
static bool ready = false;
static uint32_t last_heartbeat = 0;
static v3app::MotionDiagnostics motionDiagnostics;

static bool acknowledges(TwoWire &bus, uint8_t address) {
  bus.beginTransmission(address);
  return bus.endTransmission() == 0;
}

void setup() {
  motionDiagnostics.recorder.allocator(rasterAllocate,rasterRelease);
  Serial.begin(115200);  // Never wait for a connected USB host.
  // Arduino-ESP32 3.3.2 HWCDC::write decrements unsigned tries before checking
  // zero: timeout 0 underflows on a full, unread transport. 1ms is finite
  // (CONFIG_FREERTOS_HZ=1000); mutex acquisition is bounded, not wait-free.
  Serial.setTxTimeoutMs(1);
  Serial.printf("V3_START version=%s runtime_compatibility=unverified draft=1 no_saving=1\n", v3app::version);
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
  ui.render(); // Removes the vendor's temporary greeting before the first GUI refresh.
  ready = true;
  // Existing shared expander/IMU bus (SDA11/SCL10), not the touch Wire1 bus.
  // Each IMU tick does at most two bounded register operations, no waits/retries.
  Wire.setTimeOut(10);
  imu.begin(millis());
  Serial.printf("V3_UI version=%s initialized; pixels_and_physical_touch=unverified; synthetic_selftest=not_run\n", v3app::version);
}

void loop() {
  if (ready) Lvgl_Loop();
  const uint32_t now = millis();
  motionDiagnostics.service(Serial,now);
  if (ready) {
    const auto before = imu.state();
    const auto pollBefore=imu.last_poll().sequence;
    imu.tick(now);
    (void)pollBefore;
    if(motionDiagnostics.active()){
      const auto pre=ui.diagnostic(imu.sample(),now);
      bool triggered=false;
      ui.sample(imu.sample(),now,imu.state()==v3::ImuState::Running,&triggered);
      const auto post=ui.diagnostic(imu.sample(),now);
      motionDiagnostics.observe(Serial,now,imu.sample(),imu.status(),pre,post,triggered);
      if(imu.last_poll().sequence!=pollBefore)motionDiagnostics.recorder.observe(imu.last_poll(),imu.sample(),pre,post,triggered);
    }else{
      ui.sample(imu.sample(),now,imu.state()==v3::ImuState::Running);
    }
    if (!motionDiagnostics.active() && !motionDiagnostics.draining() && before == v3::ImuState::Probing && imu.state() != before) {
      for (unsigned i = 0; i < 2; ++i) {
        const auto &c = imu.candidate(i);
        if (c.read_ok)
          Serial.printf("V3_IMU_PROBE address=0x%02x WHO_AM_I=0x%02x expected=0x05 match=%u suffix=unidentified\n", 0x6a+i, c.id, c.id==5);
        else
          Serial.printf("V3_IMU_PROBE address=0x%02x WHO_AM_I=unreadable; no identity claim\n", 0x6a+i);
      }
    }
    if (!motionDiagnostics.active() && !motionDiagnostics.draining() && before != v3::ImuState::Running && imu.state() == v3::ImuState::Running) {
      Serial.printf("V3_IMU_CONFIG address=0x%02x WHO_AM_I=0x05 revision=0x%02x readback=ok acc_range_g=4 gyr_range_dps=2048 odr_enum=normal60 reset=not_issued suffix=unidentified\n",
                    imu.address(), imu.revision());
    }
  }
  if (uint32_t(now-last_heartbeat)>=5000) {
    if(!motionDiagnostics.active() && !motionDiagnostics.draining()){
    last_heartbeat=now;
    lv_mem_monitor_t memory{};
    if (ready) lv_mem_monitor(&memory);
    Serial.printf("V3_HEARTBEAT version=%s uptime_ms=%lu ui_ready=%u imu_status=\"%s\" imu_sample_valid=%u imu_read_errors=%lu poll_ms=20 axes=sensor_native uncalibrated=1 gesture=EXPERIMENTAL renderer=%s raster_bytes=%u lvgl_heap_available=%u lvgl_free=%u lvgl_biggest=%u psram_free=%u synthetic_selftest=not_run storage=none networking=none\n",
      v3app::version,static_cast<unsigned long>(now),ready,imu.status(),imu.sample().valid,
      static_cast<unsigned long>(imu.read_errors()),ui.busy()?"projected":(ui.fallback?"instant_fallback":"still"),
      unsigned(v3app::WindowReveal::bytes),unsigned(ready),unsigned(memory.free_size),unsigned(memory.free_biggest_size),unsigned(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)));
    }
  }
  delay(5);
}
