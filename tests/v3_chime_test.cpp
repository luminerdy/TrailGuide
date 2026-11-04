#include <cassert>
#include <cstdint>
#include <cstdio>
#include "../trailguide_v3/chime_tone.h"
#include <vector>
struct Sink {
  uint32_t clock=0; int begins=0,finishes=0,drains=0;
  std::vector<uint8_t> data;
  uint32_t now() { return clock; }
  uint32_t begin() { ++begins; return 0; }
  v3audio::WriteResult write(const uint8_t* p,size_t n,uint32_t timeout) {
    assert(timeout==20 && n<=1024); n=std::min<size_t>(n,100);
    data.insert(data.end(),p,p+n); return {n,0};
  }
  void drain() { ++drains; clock+=26; }
  uint32_t finish() { ++finishes; return 0; }
};
void stream_test() {
  Sink sink;
  auto result=v3audio::run(sink,[] { return false; });
  assert(result.error==0 && result.cleanup==0 && !result.cancelled);
  assert(result.tone==11520 && result.zero==1024);
  assert(sink.begins==1 && sink.finishes==1 && sink.drains==1);
  assert(sink.data.size()==12544*4);
  for(uint32_t i=0;i<12544;++i) {
    const auto s=uint16_t(v3audio::sample(i));
    assert(sink.data[4*i]==uint8_t(s) && sink.data[4*i+1]==uint8_t(s>>8));
    assert(sink.data[4*i]==sink.data[4*i+2] && sink.data[4*i+1]==sink.data[4*i+3]);
  }
  puts("PASS partial writes, stereo little endian, zero flush and drain");
}
struct FaultSink : Sink {
  int mode=0,writes=0; uint32_t beginError=0,finishError=0,step=0;
  uint32_t begin() { ++begins; clock+=step; return beginError; }
  uint32_t finish() { ++finishes; return finishError; }
  v3audio::WriteResult write(const uint8_t* p,size_t n,uint32_t ms) {
    ++writes; clock+=step;
    if(mode==1) return {0,0};
    if(mode==2) return {3,0};
    if(mode==3) return {n+4,0};
    if(mode==4) return {4,321};
    return Sink::write(p,n,ms);
  }
};
void fault_test() {
  { FaultSink s; auto r=v3audio::run(s,[]{return true;});
    assert(r.cancelled && s.begins==0 && s.writes==0 && s.finishes==1 && !r.error); }
  { FaultSink s; auto r=v3audio::run(s,[&]{return s.writes>=2;});
    assert(r.cancelled && s.writes==2 && s.drains==0 && s.finishes==1); }
  for(int mode=1;mode<=4;++mode) {
    FaultSink s; s.mode=mode;
    auto r=v3audio::run(s,[]{return false;});
    assert(r.error!=0 && s.writes==1 && s.finishes==1 && s.drains==0);
    if(mode==4) assert(r.error==321 && r.tone==1);
  }
  { FaultSink s; s.beginError=322; s.finishError=323;
    auto r=v3audio::run(s,[]{return false;});
    assert(r.error==322 && r.cleanup==323 && s.writes==0 && s.finishes==1); }
  { FaultSink s; s.finishError=323; auto r=v3audio::run(s,[]{return false;});
    assert(r.cleanup==323); }
  for(uint32_t start : {0u,0xfffffff0u}) {
    FaultSink s; s.clock=start; s.step=20;
    auto r=v3audio::run(s,[]{return false;});
    assert(r.error!=0 && uint32_t(s.clock-start)<=1000 && s.finishes==1 && s.drains==0);
    Sink happy; happy.clock=start;
    assert(v3audio::run(happy,[]{return false;}).error==0);
  }
  { FaultSink s; s.step=980; auto r=v3audio::run(s,[]{return false;});
    assert(r.error!=0 && s.writes==0 && s.finishes==1); }
  puts("PASS cancellation, bounded wrap-safe deadline, all stream failure cleanup");
}
int main() {
  fault_test();
  stream_test();
  using namespace v3audio;
  assert(sample(0)==0 && sample(11519)==0 && sample(11520)==0);
  int peak=0; int64_t early=0,late=0;
  for(uint32_t i=0;i<11520;++i) {
    const int s=sample(i); assert(s>=-1536 && s<=1536);
    if(s>peak) peak=s;
    if(i>=480 && i<1440) early+=int64_t(s)*s;
    if(i>=8640 && i<9600) late+=int64_t(s)*s;
  }
  assert(peak>1400 && late<early/4);
  assert(sample(504)>0 && sample(552)<0); // 96-frame = 500 Hz period
  puts("PASS gentle 240ms 500Hz bounded decaying waveform");
}
