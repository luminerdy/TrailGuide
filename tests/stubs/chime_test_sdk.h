#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
using esp_err_t=int;
using i2s_chan_handle_t=void*;
constexpr int GPIO_NUM_48=48,GPIO_NUM_38=38,GPIO_NUM_47=47;
constexpr int ESP_OK=0,I2S_NUM_AUTO=-1,I2S_ROLE_MASTER=1,I2S_GPIO_UNUSED=-1;
constexpr int I2S_DATA_BIT_WIDTH_16BIT=16,I2S_SLOT_MODE_STEREO=2,pdPASS=1;
struct i2s_chan_config_t { int id,role,dma_desc_num,dma_frame_num; bool auto_clear_after_cb; };
struct Clock { int sample_rate_hz; };
struct Slots { int data_bit_width,slot_mode; };
struct Pins { int mclk=0,bclk=0,ws=0,dout=0,din=0; };
struct i2s_std_config_t { Clock clk_cfg; Slots slot_cfg; Pins gpio_cfg; };
#define I2S_CHANNEL_DEFAULT_CONFIG(id,role) {id,role,6,240,false}
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) {rate}
#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits,mode) {bits,mode}
#define pdMS_TO_TICKS(ms) (ms)
namespace sdk {
inline int fault=0,allocated=0,live=0,enabled=0,disabled=0,deleted=0,spawns=0,exits=0,drains=0,writes=0;
inline uint32_t time=0;
inline void (*task)(void*)=nullptr;
inline void (*onWrite)()=nullptr;
inline void (*onDelete)()=nullptr;
inline std::vector<uint8_t> output;
inline void execute() { auto fn=task; assert(fn); task=nullptr; fn(nullptr); }
}
inline int64_t esp_timer_get_time() { return int64_t(sdk::time)*1000; }
inline esp_err_t i2s_new_channel(const i2s_chan_config_t* cfg,i2s_chan_handle_t* tx,i2s_chan_handle_t* rx) {
  assert(rx==nullptr && cfg->id==I2S_NUM_AUTO && cfg->role==I2S_ROLE_MASTER);
  assert(cfg->dma_desc_num==4 && cfg->dma_frame_num==256 && cfg->auto_clear_after_cb);
  if(sdk::fault==1) return 301;
  assert(sdk::live==0); *tx=reinterpret_cast<void*>(uintptr_t(1)); ++sdk::allocated; ++sdk::live; return 0;
}
inline esp_err_t i2s_channel_init_std_mode(i2s_chan_handle_t h,const i2s_std_config_t* c) {
  assert(h && c->clk_cfg.sample_rate_hz==48000 && c->slot_cfg.data_bit_width==16 && c->slot_cfg.slot_mode==2);
  assert(c->gpio_cfg.mclk==-1 && c->gpio_cfg.din==-1 && c->gpio_cfg.bclk==48 && c->gpio_cfg.ws==38 && c->gpio_cfg.dout==47);
  return sdk::fault==2?302:0;
}
inline esp_err_t i2s_channel_enable(i2s_chan_handle_t h) {
  assert(h); if(sdk::fault==3)return 303; ++sdk::enabled; return 0;
}
inline esp_err_t i2s_channel_write(i2s_chan_handle_t h,const void* p,size_t n,size_t* written,uint32_t ms) {
  assert(h && sdk::enabled && ms==20 && n<=1024); ++sdk::writes;
  if(sdk::onWrite) sdk::onWrite();
  *written=sdk::fault==4?16:n;
  const auto* b=static_cast<const uint8_t*>(p); sdk::output.insert(sdk::output.end(),b,b+*written);
  return sdk::fault==4?304:0;
}
inline esp_err_t i2s_channel_disable(i2s_chan_handle_t h) {
  assert(h); ++sdk::disabled; if(sdk::fault==5) return 305;
  --sdk::enabled; return 0;
}
inline esp_err_t i2s_del_channel(i2s_chan_handle_t h) {
  assert(h); ++sdk::deleted; if(sdk::onDelete) sdk::onDelete();
  if(sdk::fault==6 || (sdk::fault==5 && sdk::enabled))return 306;
  --sdk::live; return 0;
}
inline int xTaskCreate(void (*fn)(void*),const char*,uint32_t stack,void* arg,int priority,void* handle) {
  assert(stack==4096 && priority==1 && arg==nullptr && handle==nullptr);
  ++sdk::spawns; if(sdk::fault==7)return 0;
  assert(!sdk::task); sdk::task=fn; return pdPASS;
}
inline void vTaskDelay(uint32_t ticks) { assert(ticks>=26); sdk::time+=ticks; ++sdk::drains; }
inline void vTaskDelete(void* p) { assert(p==nullptr); ++sdk::exits; }
