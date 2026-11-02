#pragma once
#include <lvgl.h>
#include <cmath>
#include <cstdlib>
#include <cstdint>

namespace v3app {
// One caller-owned RGB565 source + destination. Never an LVGL transformed layer.
// Caller allocator is separate from LVGL's unchanged 48 KiB object heap; no frame allocations.
class WindowReveal {
 public:
  static constexpr int width = 256, height = 140;
  static constexpr size_t pixels = width * height, bytes = pixels * sizeof(lv_color_t) * 2;
  struct Pose { float x, y, z, rx, ry, rz, light; };
  static Pose pose(uint32_t ms) {
    static const uint32_t times[] = {0, 600, 1680, 3780, 5040, 6000};
    static const Pose keys[] = {{3,25,-110,13,-9,-3,0},{3,25,-110,13,-9,-3,0},
      {2,21,-92,11,-7,-2,.22f},{-2,7,-30,4,3,1,.73f},
      {1,1,-3,1,-1,-.3f,.96f},{0,0,0,0,0,0,1}};
    if (ms >= 6000) return keys[5];
    unsigned i = 0; while (ms > times[i+1]) ++i;
    const float t = float(ms - times[i]) / float(times[i+1] - times[i]);
    const auto &a = keys[i]; const auto &b = keys[i+1];
    return {a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,
      a.rx+(b.rx-a.rx)*t,a.ry+(b.ry-a.ry)*t,a.rz+(b.rz-a.rz)*t,a.light+(b.light-a.light)*t};
  }
  using Allocate=void*(*)(size_t); using Release=void(*)(void*);
  WindowReveal(Allocate a,Release f):allocate(a),release(f){}
  WindowReveal(const WindowReveal &) = delete;
  WindowReveal &operator=(const WindowReveal &) = delete;
  ~WindowReveal() { clear(); }
  bool capture(lv_obj_t *panel) {
    clear();
    // LVGL snapshot/timer allocators assert on exhaustion. Preflight their small
    // temporary context + timer/style reserve while the single LVGL thread owns
    // the heap; do not turn a missing 53KiB LVGL layer into a heap-size increase.
    lv_mem_monitor_t memory; lv_mem_monitor(&memory);
    if (memory.free_biggest_size < lv_obj_get_disp(panel)->driver->draw_ctx_size + 2048U) return false;
    if (lv_snapshot_buf_size_needed(panel, LV_IMG_CF_TRUE_COLOR) != pixels * sizeof(lv_color_t)) return false;
    storage = static_cast<lv_color_t *>(allocate(bytes));
    if (!storage) return false;
    lv_img_dsc_t snapshot{};
    if (lv_snapshot_take_to_buf(panel, LV_IMG_CF_TRUE_COLOR, &snapshot, storage,
                               pixels * sizeof(lv_color_t)) != LV_RES_OK) { clear(); return false; }
    frame = snapshot; frame.data = reinterpret_cast<const uint8_t *>(storage + pixels);
    render(0);
    return true;
  }
  bool active() const { return storage != nullptr && frame.data != nullptr; }
  void clear() {
    if (storage) { lv_img_cache_invalidate_src(&frame); release(storage); storage = nullptr; }
    frame = {};
  }
  void draw(lv_draw_ctx_t *ctx, const lv_area_t &area) const {
    lv_draw_img_dsc_t dsc; lv_draw_img_dsc_init(&dsc);
    lv_draw_img(ctx, &dsc, &area, &frame);
  }
  void render(uint32_t elapsed) {
    const auto p = pose(elapsed);
    const float rad = .01745329252f;
    const float sx = std::sin(p.rx*rad), cx = std::cos(p.rx*rad);
    const float sy = std::sin(p.ry*rad), cy = std::cos(p.ry*rad);
    const float sz = std::sin(p.rz*rad), cz = std::cos(p.rz*rad);
    // CSS rotateX * rotateY * rotateZ, projected at 360px perspective.
    const float r00=cy*cz, r01=-cy*sz;
    const float r10=cx*sz+sx*sy*cz, r11=cx*cz-sx*sy*sz;
    const float r20=sx*sz-cx*sy*cz, r21=sx*cz+cx*sy*sz;
    const float ox=width/2.f, oy=height/2.f;
    const float g=-r20, h=-r21, i=360-p.z+r20*ox+r21*oy;
    const float a=360*r00+ox*g, b=360*r01+ox*h, c=360*(p.x-r00*ox-r01*oy)+ox*i;
    const float d=360*r10+oy*g, e=360*r11+oy*h, f=360*(p.y-r10*ox-r11*oy)+oy*i;
    const float inv[] = {e*i-f*h,c*h-b*i,b*f-c*e,f*g-d*i,a*i-c*g,c*d-a*f,d*h-e*g,b*g-a*h,a*e-b*d};
    int32_t m[9];
    for (unsigned k=0;k<9;++k) m[k]=int32_t(inv[k]/inv[8]*65536.f);
    auto *out=storage+pixels;
    const auto dark=lv_color_hex(0x02070d);
    const unsigned light=unsigned(p.light*256.f+.5f);
    for (int y=0;y<height;++y) {
      int32_t u=m[1]*y+m[2], v=m[4]*y+m[5], w=m[7]*y+m[8];
      for (int x=0;x<width;++x,u+=m[0],v+=m[3],w+=m[6]) {
        // Pixel-center nearest sampling settles onto the exact native raster;
        // truncation would shift almost every glyph one pixel at the handoff.
        const int px=w>0 && u+w/2>=0?(u+w/2)/w:-1;
        const int py=w>0 && v+w/2>=0?(v+w/2)/w:-1;
        auto color=dark;
        if (light && px>=0 && px<width && py>=0 && py<height) {
          color=storage[py*width+px];
          color.ch.red=color.ch.red*light/256;
          color.ch.green=color.ch.green*light/256;
          color.ch.blue=color.ch.blue*light/256;
        }
        *out++=color;
      }
    }
  }
  // Opaque, faceted face used both by the snapshot and final scrollable panel.
  static void face(lv_draw_ctx_t *ctx, const lv_area_t &area, int scrollY) {
    lv_draw_rect_dsc_t dsc; lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color=lv_color_hex(0x02070d);
    lv_draw_rect(ctx,&dsc,&area);
    for (int y=0;y<height;++y) {
      int left=y<10?(10-y):0;
      int right=y<10?width-11+y:width-1;
      if(y>height-11) right=width-1-(y-(height-11));
      lv_area_t line={lv_coord_t(area.x1+left),lv_coord_t(area.y1+y),lv_coord_t(area.x1+right),lv_coord_t(area.y1+y)};
      dsc.bg_color=lv_color_hex(0x124d8b);lv_draw_rect(ctx,&dsc,&line);
      int edge=52-y*2;
      if(edge>left){line.x2=area.x1+edge;dsc.bg_color=lv_color_hex(0x27689f);lv_draw_rect(ctx,&dsc,&line);}
      edge=width+height-30-y;
      if(edge<right){line.x1=area.x1+edge;line.x2=area.x1+right;dsc.bg_color=lv_color_hex(0x0e3b6a);lv_draw_rect(ctx,&dsc,&line);}
    }
    (void)scrollY;
  }
 private:
  Allocate allocate; Release release;
  lv_color_t *storage = nullptr;
  lv_img_dsc_t frame{};
};
}
