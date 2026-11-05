#pragma once
// Main loop only: service(uiSoundEnabled, cardReadableSequence, cueStillOwned).
// Validity is checked at acceptance, after this iteration's motion/navigation.
// Invalid edges are consumed without cancelling an already accepted worker.
// Sequence starts at zero. Sound defaults OFF at caller. No boot/toggle cue.
#include "chime_tone.h"
#include <atomic>
#ifdef TRAILGUIDE_CHIME_HOST
#include "chime_test_sdk.h"
#else
#include "driver/i2s_std.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif
namespace v3audio {
enum State : uint32_t { Idle=0,Starting=1,Playing=2,Done=3,Cancelled=4,Error=5,Locked=6 };
static_assert(sizeof(std::atomic<uint32_t>)==4,"Diagnostic words must be 32 bit");
// The S3 SDK uses -mdisable-hardware-atomics. Keep its thread-safe atomic
// implementation; do not require lock freedom or bypass the SDK's safety flags.
// Access is task-only: no driver calls or waits occur inside an atomic operation.
// Atomic 32-bit words for fresh-ELF debugger readback. Terminal state is
// published after driver cleanup and result words. completed/failures are
// cumulative; error/cleanupError and frame counts describe the last attempt.
// error is a raw ESP error or ErrorCode; cleanupError preserves release errors.
// Locked means a failed delete retained the static driver owner: no new cues
// are accepted until reboot (no unsafe replacement allocation/retry).
struct Diagnostics {
  std::atomic<uint32_t> state{Idle},completed{0},error{0},cleanupError{0};
  std::atomic<uint32_t> toneFrames{0},zeroFrames{0};
  std::atomic<uint32_t> failures{0};
};
inline Diagnostics diagnostics;
inline std::atomic<uint32_t> cancelRequested{0};
class TxSink {
  i2s_chan_handle_t tx=nullptr;
  bool enabled=false;
public:
  uint32_t now() { return uint32_t(esp_timer_get_time()/1000); }
  uint32_t begin() {
    i2s_chan_config_t cfg=I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO,I2S_ROLE_MASTER);
    cfg.dma_desc_num=4; cfg.dma_frame_num=256; cfg.auto_clear_after_cb=true;
    esp_err_t error=i2s_new_channel(&cfg,&tx,nullptr);
    if(error!=ESP_OK) return uint32_t(error);
    i2s_std_config_t config{};
    config.clk_cfg=I2S_STD_CLK_DEFAULT_CONFIG(48000);
    config.slot_cfg=I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,I2S_SLOT_MODE_STEREO);
    config.gpio_cfg.mclk=I2S_GPIO_UNUSED;
    config.gpio_cfg.bclk=GPIO_NUM_48;
    config.gpio_cfg.ws=GPIO_NUM_38;
    config.gpio_cfg.dout=GPIO_NUM_47;
    config.gpio_cfg.din=I2S_GPIO_UNUSED;
    error=i2s_channel_init_std_mode(tx,&config);
    if(error!=ESP_OK) return uint32_t(error);
    error=i2s_channel_enable(tx);
    if(error==ESP_OK) { enabled=true; diagnostics.state=Playing; }
    return uint32_t(error);
  }
  WriteResult write(const uint8_t* bytes,size_t count,uint32_t wait) {
    size_t written=0;
    const auto error=i2s_channel_write(tx,bytes,count,&written,wait);
    return {written,uint32_t(error)};
  }
  void drain() { vTaskDelay(pdMS_TO_TICKS(26)+1); }
  uint32_t finish() {
    esp_err_t error=ESP_OK;
    if(enabled) {
      error=i2s_channel_disable(tx);
      if(error==ESP_OK) enabled=false;
    }
    if(tx) {
      const auto deleted=i2s_del_channel(tx);
      if(error==ESP_OK) error=deleted;
      if(deleted==ESP_OK) { tx=nullptr; enabled=false; }
    }
    return uint32_t(error);
  }
  bool retained() const { return tx!=nullptr; }
};
// Static lifetime; only worker owns the channel, never a caller/UI pointer.
inline TxSink sink;
inline void worker(void*) {
  const auto result=run(sink,[]{return cancelRequested.load()!=0;});
  const bool cancelled=result.cancelled || cancelRequested.load()!=0;
  diagnostics.error=result.error;
  diagnostics.cleanupError=result.cleanup;
  diagnostics.toneFrames=result.tone;
  diagnostics.zeroFrames=result.zero;
  if(result.error || result.cleanup) ++diagnostics.failures;
  if(!result.error && !result.cleanup && !cancelled) ++diagnostics.completed;
  // A failed delete keeps the static owner alive and permanently locks new cues.
  // Do not overwrite/leak an uncertain driver handle with a fresh allocation.
  diagnostics.state=sink.retained() ? Locked : result.error || result.cleanup ? Error : cancelled ? Cancelled : Done;
  vTaskDelete(nullptr);
}
inline void service(bool enabled,uint32_t sequence,bool valid=true) {
  static uint32_t consumed=0; // Main-loop-owned, baseline zero.
  const uint32_t state=diagnostics.state.load();
  const bool busy=state==Starting || state==Playing;
  // Sticky for this worker: ON never clears a mute cancellation.
  if(!enabled && busy) cancelRequested=1;
  if(sequence==consumed) return;
  consumed=sequence;
  if(!enabled || !valid || busy || state==Locked) return;
  cancelRequested=0;
  diagnostics.state=Starting;
  if(xTaskCreate(worker,"v3-chime",4096,nullptr,1,nullptr)!=pdPASS) {
    diagnostics.error=TaskError; ++diagnostics.failures; diagnostics.state=Error;
  }
}
} // namespace v3audio
