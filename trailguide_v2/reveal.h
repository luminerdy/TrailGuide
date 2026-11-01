#pragma once
#include <cstdint>

namespace trailguide {
// Keep the complete immutable answer; only its visibility changes, never its text.
class RevealText {
 public:
  static constexpr uint32_t settleMs = 160;
  static constexpr uint32_t revealMs = 2800;
  static constexpr uint32_t completeMs = settleMs + revealMs;
  explicit RevealText(const char *text = "") : text_(text) {}
  const char *text() const { return text_; }
  uint8_t opacity(uint32_t elapsed) const {
    if (elapsed <= settleMs) return 0;
    if (elapsed >= completeMs) return 255;
    const uint32_t progress = (elapsed - settleMs) * 255U / revealMs;
    // Integer smoothstep: the entire answer gently emerges and settles clear.
    return static_cast<uint8_t>(progress * progress * (765U - 2U * progress) / 65025U);
  }
 private:
  const char *text_;
};
}
