#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace trailguide {
// Borrowed immutable copy; the canonical deck outlives the UI. No fixed-size buffer.
class RevealText {
 public:
  static constexpr uint32_t settleMs = 160;
  static constexpr uint32_t revealMs = 2800;
  static constexpr uint32_t completeMs = settleMs + revealMs;
  explicit RevealText(const char *text = "") : text_(text), bytes_(std::strlen(text)) {}
  const char *text() const { return text_; }
  size_t prefixBytes(uint32_t elapsed) const {
    if (elapsed < settleMs) return 0;
    if (elapsed >= completeMs) return bytes_;
    const auto progress = elapsed - settleMs;
    // Divide first so even a size_t-sized synthetic input cannot overflow.
    size_t bytes = (bytes_ / revealMs) * progress + (bytes_ % revealMs) * progress / revealMs;
    while (bytes && (static_cast<unsigned char>(text_[bytes]) & 0xc0) == 0x80) --bytes;
    return bytes;
  }
 private:
  const char *text_;
  size_t bytes_;
};
}
