#pragma once
#include <cstdint>
namespace v3app {
// RAM-only UI preference and monotonic settlement event; no audio dependencies.
struct SoundCue {
 bool enabled=false;
 uint32_t sequence=0;
 void settled(){if(enabled)++sequence;}
};
}
