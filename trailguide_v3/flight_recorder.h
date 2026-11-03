#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
namespace v3app {
// Native little-endian ESP32/host wire layout; no padding or personal content.
struct FlightRecord {
 uint32_t poll_ms=0,sample_ms=0,errors=0;
 int16_t raw[6]={};
 uint16_t gates_before=0,gates_after=0,reject=0,resets=0;
 uint8_t phase_before=0,phase_after=0,status0=0,flags=0,clip=0,reset_reason=0,reserved[2]={};
};
static_assert(sizeof(FlightRecord)==40,"flight wire ABI");
static_assert(offsetof(FlightRecord,raw)==12 && offsetof(FlightRecord,flags)==35,"flight offsets");
// JTAG: motionDiagnostics.recorder.ram; records points to one PSRAM allocation.
struct FlightRam {
 uint32_t magic=0x31524654,version=1,record_bytes=40,capacity=2304;
 uint32_t session=0,state=0,count=0,started=0,ended=0,reason=0,crc32=0,error=0;
 FlightRecord *records=nullptr;
};
static_assert(offsetof(FlightRam,records)==48,"JTAG flight header ABI");
class FlightRecorder {
 public:
 using Allocate=void *(*)(size_t);using Release=void (*)(void *);
 static constexpr uint32_t DurationMs=45000,Capacity=2304,ChunkMax=32;
 FlightRam ram;
 explicit FlightRecorder(Allocate a=nullptr,Release r=nullptr):allocate_(a),release_(r){}
 ~FlightRecorder(){if(ram.records && release_)release_(ram.records);}
 FlightRecorder(const FlightRecorder&)=delete;FlightRecorder &operator=(const FlightRecorder&)=delete;
 void allocator(Allocate a,Release r){allocate_=a;release_=r;}
 bool active()const{return ram.state==1;}
 bool draining()const{return length_>cursor_ || dump_;}
 bool quiet()const{return active() || draining();}
 static uint32_t crc(const void *p,size_t n){uint32_t c=0xffffffffu;auto *b=static_cast<const uint8_t*>(p);while(n--){c^=*b++;for(unsigned k=0;k<8;++k)c=(c>>1)^(0xedb88320u & (0u-(c&1)));}return ~c;}
 bool start(uint32_t now,uint32_t session=0){
  if(active()){ram.error=1;return false;}
  cancel();
  if(!ram.records && allocate_)ram.records=static_cast<FlightRecord*>(allocate_(sizeof(FlightRecord)*Capacity));
  if(!ram.records){ram.error=2;return false;}
  if(session && session!=ram.session)ram.session=session;
  else {++ram.session;if(!ram.session)++ram.session;}
  ram.count=0;ram.started=now;ram.ended=0;ram.reason=0;ram.crc32=0;ram.error=0;ram.state=1;havePoll_=false;resetsSeen_=false;
  return true;
 }
 void tick(uint32_t now){if(active() && uint32_t(now-ram.started)>=DurationMs)freeze(now,1);}
 void append(const FlightRecord &r){
  if(!active())return;
  tick(r.poll_ms);if(!active())return;
  ram.records[ram.count++]=r;
  if(ram.count==Capacity)freeze(r.poll_ms,2);
 }
 template<class Meta,class Sample,class Snapshot> void observe(const Meta &m,const Sample &s,const Snapshot &before,const Snapshot &after,bool triggered){
  if(!active() || (havePoll_ && m.sequence==lastPoll_))return;
  FlightRecord r{};r.poll_ms=m.at_ms;r.sample_ms=s.at_ms;r.errors=m.errors;
  for(unsigned i=0;i<6;++i)r.raw[i]=m.raw[i];
  r.flags=m.flags | (s.valid?1:0) | (triggered?128:0);r.status0=m.status0;r.clip=m.clip;
  r.gates_before=uint16_t(before.key()&1023);r.gates_after=uint16_t(after.key()&1023);r.reject=uint16_t(before.gesture.reject);
  r.reset_reason=triggered?9:(after.gesture.resets!=before.gesture.resets?after.gesture.resetReason:0);
  r.phase_before=uint8_t(before.gesture.phase);r.phase_after=uint8_t(after.gesture.phase);
  for(unsigned i=0;i<8;++i){if((resetsSeen_ && after.resetCounts[i]!=lastResets_[i]) || before.resetCounts[i]!=after.resetCounts[i])r.resets|=uint16_t(1u<<i);lastResets_[i]=after.resetCounts[i];}
  if(!after.healthy)r.resets|=256;
  if(!after.eligible)r.resets|=512;
  lastPoll_=m.sequence;havePoll_=true;resetsSeen_=true;append(r);
 }
 void clear(){if(active()){ram.error=1;return;}cancel();ram.count=0;ram.state=0;ram.crc32=0;ram.reason=0;ram.error=0;}
 void cancel(){dump_=false;length_=cursor_=0;}
 // SINGLE command consumer lives in MotionDiagnostics. No RX here.
 bool command(const char *c,uint32_t now){
  // Validate every numeric token before starting, clearing or serializing.
  // scanf %u accepts signs and cannot safely reject overflowing input.
  const char *p=c;auto space=[](char x){return x==' ' || x=='\t';};
  while(space(*p))++p;
  const char *verb=p;
  while(*p && !space(*p))++p;
  const size_t length=size_t(p-verb);
  const bool rec=(length==3 && std::memcmp(verb,"REC",3)==0) || (length==6 && std::memcmp(verb,"RECORD",6)==0);
  const bool stat=length==6 && std::memcmp(verb,"STATUS",6)==0;
  const bool clearCommand=length==5 && std::memcmp(verb,"CLEAR",5)==0;
  const bool dump=length==4 && std::memcmp(verb,"DUMP",4)==0;
  if(!rec && !stat && !clearCommand && !dump)return false;
  auto refuse=[&](){ram.error=3;status(now,0);return true;};
  uint32_t args[4]={};unsigned fields=0;
  while(space(*p))++p;
  while(*p){
   if(fields==4 || *p<'0' || *p>'9')return refuse();
   uint32_t value=0;
   do{const uint32_t digit=uint32_t(*p-'0');if(value>(0xffffffffu-digit)/10u)return refuse();value=value*10u+digit;++p;}while(*p>='0' && *p<='9');
   if(*p && !space(*p))return refuse();
   args[fields++]=value;while(space(*p))++p;
  }
  if(!dump){
   if(fields>1)return refuse();
   const uint32_t nonce=fields?args[0]:0;
   if(rec){start(now,nonce);status(now,nonce);return true;}
   if(stat){status(now,nonce);return true;}
   clear();status(now,nonce);return true;
  }
  if(fields!=4)return refuse();
  const uint32_t session=args[0],offset=args[1],count=args[2],token=args[3];
  if(ram.state!=2 || session!=ram.session || count==0 || count>ChunkMax || offset>ram.count || count>ram.count-offset){ram.error=3;status(now,token);return true;}
  cancel();ram.error=0;offset_=offset;end_=offset+count;next_=offset;token_=token;chunkCrc_=crc(ram.records+offset,count*sizeof(FlightRecord));dump_=true;stage_=0;lastProgress_=now;return true;
 }
 template<class Serial> void service(Serial &s,uint32_t now){
  tick(now);
  // Frozen RAM survives disconnect; abort only the in-flight serialization.
  if(!bool(s)){cancel();return;}
  if(draining() && uint32_t(now-lastProgress_)>=5000){cancel();return;}
  if(cursor_>=length_ && dump_)nextLine(now);
  if(cursor_>=length_)return;
  int cap=s.availableForWrite();if(cap<=0)return;
  size_t n=length_-cursor_;if(n>64)n=64;if(n>size_t(cap))n=size_t(cap);
  size_t sent=s.write(reinterpret_cast<const uint8_t*>(line_+cursor_),n);if(sent>n)sent=n;cursor_+=sent;if(sent)lastProgress_=now;
 }
 private:
 Allocate allocate_;Release release_;bool havePoll_=false,resetsSeen_=false,dump_=false;
 uint32_t lastPoll_=0,lastResets_[8]={},offset_=0,end_=0,next_=0,token_=0,chunkCrc_=0,lastProgress_=0;
 unsigned stage_=0;char line_[256]={};size_t length_=0,cursor_=0;
 void freeze(uint32_t now,uint32_t why){ram.ended=now;ram.reason=why;ram.crc32=crc(ram.records,ram.count*sizeof(FlightRecord));ram.state=2;}
 void status(uint32_t now,uint32_t nonce){cancel();lastProgress_=now;length_=size_t(std::snprintf(line_,sizeof(line_),"\nTGFR1 STATUS token=%lu magic=31524654 version=1 session=%lu state=%lu count=%lu ready=%u size=40 capacity=2304 start=%lu end=%lu reason=%lu crc=%08lx error=%lu\n",(unsigned long)nonce,(unsigned long)ram.session,(unsigned long)ram.state,(unsigned long)ram.count,unsigned(ram.count>0),(unsigned long)ram.started,(unsigned long)ram.ended,(unsigned long)ram.reason,(unsigned long)ram.crc32,(unsigned long)ram.error));}
 void nextLine(uint32_t now){
  cursor_=0;lastProgress_=now;
  if(stage_==0){length_=size_t(std::snprintf(line_,sizeof(line_),"\nTGFR1 BEGIN session=%lu token=%lu offset=%lu count=%lu total=%lu size=40 magic=31524654 version=1 crc=%08lx fullcrc=%08lx\n",(unsigned long)ram.session,(unsigned long)token_,(unsigned long)offset_,(unsigned long)(end_-offset_),(unsigned long)ram.count,(unsigned long)chunkCrc_,(unsigned long)ram.crc32));stage_=1;}
  else if(next_<end_){int n=std::snprintf(line_,sizeof(line_),"TGFR1 DATA session=%lu token=%lu index=%lu hex=",(unsigned long)ram.session,(unsigned long)token_,(unsigned long)next_);const auto *p=reinterpret_cast<const uint8_t*>(ram.records+next_++);static constexpr char hex[]="0123456789abcdef";for(size_t i=0;i<sizeof(FlightRecord);++i){line_[n++]=hex[p[i]>>4];line_[n++]=hex[p[i]&15];}line_[n++]='\n';length_=size_t(n);}
  else{length_=size_t(std::snprintf(line_,sizeof(line_),"TGFR1 END session=%lu token=%lu offset=%lu count=%lu crc=%08lx\n",(unsigned long)ram.session,(unsigned long)token_,(unsigned long)offset_,(unsigned long)(end_-offset_),(unsigned long)chunkCrc_));dump_=false;}
 }
};
}
