#pragma once
#include <cstdint>
namespace v3app {
// RAM-only preference and monotonic settlement event; no audio dependencies.
// Main-loop-only ownership invalidates unaccepted events, never a playing task.
struct SoundCue {
 bool enabled=false;
 uint32_t sequence=0;
 void settled(const void *owner){if(enabled){++sequence;pendingOwner=owner;}}
 void cancel(const void *owner){if(pendingOwner==owner)pendingOwner=nullptr;}
 bool valid()const{return pendingOwner!=nullptr;}
 private:
 const void *pendingOwner=nullptr; // identity only; never dereferenced or sent to worker
};
}
