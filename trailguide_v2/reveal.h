#pragma once
#include <cstdint>

namespace trailguide {
// Approved physical-window duration, including the initial ten-percent dark beat.
// Copy is always the complete immutable native label; it is never typed/faded.
struct RevealTiming {
  static constexpr uint32_t settleMs = 600;
  static constexpr uint32_t revealMs = 5400;
  static constexpr uint32_t completeMs = settleMs + revealMs;
};
}
