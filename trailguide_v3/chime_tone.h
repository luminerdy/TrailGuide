#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace v3audio {
// Fixed 500 Hz cycle at 48 kHz. Peak 1536/32768; no runtime synthesis/allocation.
inline constexpr int16_t sine[96] = {0,100,200,300,398,494,588,680,768,854,936,1012,1086,1154,1218,1278,1330,1378,1420,1454,1484,1506,1522,1532,1536,1532,1522,1506,1484,1454,1420,1378,1330,1278,1218,1154,1086,1012,936,854,768,680,588,494,398,300,200,100,0,-100,-200,-300,-398,-494,-588,-680,-768,-854,-936,-1012,-1086,-1154,-1218,-1278,-1330,-1378,-1420,-1454,-1484,-1506,-1522,-1532,-1536,-1532,-1522,-1506,-1484,-1454,-1420,-1378,-1330,-1278,-1218,-1154,-1086,-1012,-936,-854,-768,-680,-588,-494,-398,-300,-200,-100};
inline constexpr uint32_t toneFrames=11520, flushFrames=1024;
inline int16_t sample(uint32_t frame) {
  if(frame>=toneFrames) return 0;
  const uint32_t fade=std::min<uint32_t>(480u,std::min<uint32_t>(frame,toneFrames-1-frame));
  // Linear decay plus 10 ms attack/release: a soft chime, not a flat beep.
  const int64_t envelope=int64_t(fade)*(toneFrames-1-frame);
  return int16_t(int64_t(sine[frame%96])*envelope/(480*(toneFrames-1)));
}
enum ErrorCode : uint32_t { TaskError=0x10000, DeadlineError, NoProgressError, AlignmentError, OversizeError };
struct WriteResult { size_t bytes; uint32_t error; };
struct Result { uint32_t error=0,cleanup=0,tone=0,zero=0; bool cancelled=false; };
template<class Sink,class Cancel> inline Result run(Sink& sink,Cancel cancel) {
  Result result;
  const uint32_t started=sink.now();
  result.cancelled=cancel();
  if(!result.cancelled) result.error=sink.begin();
  uint8_t chunk[256*4];
  if(!result.error && !result.cancelled) {
    for(uint32_t frame=0;frame<toneFrames+flushFrames && !result.error && !result.cancelled;) {
      const uint32_t count=std::min<uint32_t>(256u,toneFrames+flushFrames-frame);
      for(uint32_t i=0;i<count;++i) {
        const uint16_t s=uint16_t(sample(frame+i));
        chunk[4*i]=chunk[4*i+2]=uint8_t(s);
        chunk[4*i+1]=chunk[4*i+3]=uint8_t(s>>8);
      }
      size_t offset=0;
      while(offset<count*4 && !result.error) {
        if(cancel()) { result.cancelled=true; break; }
        // Reserve one 20 ms write and 26 ms DMA drain inside 1000 ms.
        if(uint32_t(sink.now()-started)>=950) { result.error=DeadlineError; break; }
        const auto write=sink.write(chunk+offset,count*4-offset,20);
        if(write.bytes>count*4-offset) { result.error=OversizeError; break; }
        offset+=write.bytes;
        if(frame<toneFrames) result.tone+=uint32_t(write.bytes/4);
        else result.zero+=uint32_t(write.bytes/4);
        if(write.bytes%4) result.error=AlignmentError;
        else if(write.error) result.error=write.error;
        else if(!write.bytes) result.error=NoProgressError;
      }
      frame+=count;
    }
    if(cancel()) result.cancelled=true;
    if(!result.error && !result.cancelled) {
      if(uint32_t(sink.now()-started)>=974) result.error=DeadlineError;
      else sink.drain();
    }
  }
  result.cleanup=sink.finish();
  if(!result.error && uint32_t(sink.now()-started)>=1000) result.error=DeadlineError;
  return result;
}
} // namespace v3audio
