#pragma once
#include <stdint.h>
#include <stddef.h>
#define V3_LAST_POLL_METADATA 1

// Owned driver; 20ms polling retained. Wider gyro range is not physical acceptance.
namespace v3 {
// QMI8658A Rev A / QMI8658C Rev 0.6 agree on CTRL2 aFS; suffix unidentified.
// One checked selection binds register and physical-unit conversion.
static constexpr uint8_t AccRangeSelection=3; // 011: +/-16 g
static_assert(AccRangeSelection<=3, "supported QMI8658 accelerometer range");
static constexpr uint8_t AccCtrl2=uint8_t((AccRangeSelection<<4)|7); // unchanged aODR
static constexpr float AccFullScaleG=float(2u<<AccRangeSelection);
static constexpr float AccSafetyG=AccFullScaleG*(3.99f/4.f); // retained fractional headroom
// QMI8658A Rev A / QMI8658C Rev 0.8 common CTRL3 gFS bits; suffix unidentified.
// One selection derives register, conversion and fractional rail headroom.
static constexpr uint8_t GyroRangeSelection=7;
static constexpr uint8_t GyroCtrl3=uint8_t((GyroRangeSelection<<4)|7); // unchanged gODR
static constexpr float GyroFullScaleDps=float(16u<<GyroRangeSelection);
static constexpr float GyroSafetyDps=GyroFullScaleDps*(511.f/512.f);
enum class ImuState { Off, Probing, Missing, Identity, Configuring, ConfigError, Running };
struct ImuCandidate { bool read_ok=false; uint8_t id=0; };
// Read-only last actual poll. Flags: status-read=2 raw-read=4 error=8
// not-ready=16 raw-invalid=32 clipped=64; valid remains sample-owned.
struct ImuPoll {
 uint32_t sequence=0,at_ms=0,errors=0;
 int16_t raw[6]={};uint8_t status0=0,flags=0,clip=0;
};
struct ImuSample {
  bool valid=false;
  uint32_t at_ms=0;
  float acc_g[3]={}, gyr_dps[3]={};
};
// Owned bounded driver; original-family Gyro_QMI8658.{h,cpp} supplies register
// addresses, CTRL1/6/7 semantics, ODR/range enums and little-endian scaling.
// WHO_AM_I=0x05 is corroborated by the locally pinned manufacturer FastIMU
// header (see private build provenance). 0x01 is REVISION, NOT WHO_AM_I.
// Original-family init has NO soft reset. Keep that path: do not guess a reset
// command from conflicting supplementary drivers (0xff versus 0xb0 at 0x60).
// Wire is the existing shared expander bus: never call begin() here.
template<class Bus> class Imu {
 public:
  explicit Imu(Bus &bus): bus_(bus) {}
  void begin(uint32_t now) {
    state_=ImuState::Probing; probe_=0; selected_=false; step_=0;
    candidates_[0]={}; candidates_[1]={}; address_=0; revision_=0;
    sample_={}; metadata_={}; last_poll_=now; errors_=0; poll_status_="IMU: waiting";
  }
  void tick(uint32_t now) {
    if(state_==ImuState::Running) { poll(now); return; }
    if(state_==ImuState::Probing) {
      auto &c=candidates_[probe_];
      c.read_ok=read(0x6a+probe_,0,&c.id,1);
      if(c.read_ok && c.id==0x05 && !selected_) { address_=0x6a+probe_; selected_=true; }
      if(++probe_==2) {
        state_=selected_ ? ImuState::Configuring :
          (candidates_[0].read_ok || candidates_[1].read_ok ? ImuState::Identity : ImuState::Missing);
      }
      return;
    }
    if(state_!=ImuState::Configuring) return;
    if(step_==0) {
      uint8_t ctrl1;
      if(!read(address_,1,&revision_,1) || !read(address_,2,&ctrl1,1)) { state_=ImuState::ConfigError; return; }
      ctrl1_=(ctrl1 & 0xfe) | 0x40;
      ++step_; return;
    }
    // Disable sensors before setting range/rate, then enable as original family.
    // Selected acc/gyro ranges, normal ODR enum 7 (~60 Hz); no filtering/motion engine.
    const uint8_t regs[]={8,2,3,4,6,7,8};
    const uint8_t values[]={0,ctrl1_,AccCtrl2,GyroCtrl3,0,0,0x43};
    const uint8_t i=step_-1;
    uint8_t observed;
    if(!write(regs[i],values[i]) || !read(address_,regs[i],&observed,1) || observed!=values[i]) {
      state_=ImuState::ConfigError; return;
    }
    if(++step_==8) state_=ImuState::Running;
  }
  ImuState state() const { return state_; }
  const ImuCandidate &candidate(unsigned i) const { return candidates_[i<2 ? i : 0]; }
  uint8_t address() const { return address_; }
  uint8_t revision() const { return revision_; }
  const ImuSample &sample() const { return sample_; }
  const ImuPoll &last_poll() const { return metadata_; }
  uint32_t read_errors() const { return errors_; }
  const char *status() const {
    switch(state_) {
      case ImuState::Off: return "IMU: off";
      case ImuState::Probing: return "IMU: starting";
      case ImuState::Missing: return "IMU: missing";
      case ImuState::Identity: return "IMU: ID error";
      case ImuState::Configuring: return "IMU: config";
      case ImuState::ConfigError: return "IMU: cfg error";
      case ImuState::Running: return sample_.valid ? "IMU: sample" : poll_status_;
    }
    return "IMU: off";
  }
 private:
  void poll(uint32_t now) {
    if(uint32_t(now-last_poll_)<20) return;
    last_poll_=now;
    const uint32_t sequence=metadata_.sequence+1;metadata_={};metadata_.sequence=sequence;metadata_.at_ms=now;metadata_.errors=errors_;
    sample_.valid=false; // Never publish stale or partially read numbers.
    poll_status_="IMU: waiting";
    uint8_t status=0, raw[12];
    if(!read(address_,0x2e,&status,1)) { ++errors_; metadata_.errors=errors_;metadata_.flags=8; poll_status_="IMU: read err"; return; }
    metadata_.status0=status;metadata_.flags=2;
    // STATUS0 bits 0/1 corroborated by the pinned local supplementary driver.
    if((status & 3)!=3) {metadata_.flags|=16;return;}
    if(!read(address_,0x35,raw,sizeof(raw))) { ++errors_; metadata_.errors=errors_;metadata_.flags|=8; poll_status_="IMU: read err"; return; }
    metadata_.flags|=4;
    // Diagnostic-only decode does not change the publication loop or bus calls.
    for(unsigned i=0;i<6;++i){
      const uint16_t bits=uint16_t(raw[i*2]) | (uint16_t(raw[i*2+1])<<8);
      const int32_t value=bits>=0x8000 ? int32_t(bits)-65536 : bits;
      metadata_.raw[i]=int16_t(value);
      if(value==-32768 || value==32767)metadata_.clip|=uint8_t(1u<<i);
    }
    poll_status_="IMU: invalid";
    bool zero=true, ff=true;
    for(uint8_t b:raw) { zero=zero && b==0; ff=ff && b==255; }
    if(zero || ff) {metadata_.flags|=32;return;}
    if(metadata_.clip)metadata_.flags|=64;
    ImuSample next;
    for(unsigned axis=0;axis<6;++axis) {
      const uint16_t bits=uint16_t(raw[axis*2]) | (uint16_t(raw[axis*2+1])<<8);
      // Explicit sign conversion avoids negative shifts / implementation-defined casts.
      const int32_t signed_raw=bits>=0x8000 ? int32_t(bits)-65536 : bits;
      if(signed_raw==-32768 || signed_raw==32767) return; // clipped, not trustworthy
      if(axis<3) next.acc_g[axis]=signed_raw*(AccFullScaleG/32768.f);
      else next.gyr_dps[axis-3]=signed_raw*(GyroFullScaleDps/32768.f);
    }
    next.valid=true; next.at_ms=now; sample_=next;
  }
  bool write(uint8_t reg,uint8_t value) {
    bus_.beginTransmission(address_);
    const bool queued=bus_.write(reg)==1 && bus_.write(value)==1;
    const bool sent=bus_.endTransmission()==0;
    return queued && sent;
  }
  bool read(uint8_t address,uint8_t reg,uint8_t *out,size_t n) {
    bus_.beginTransmission(address);
    const bool queued=bus_.write(reg)==1;
    const bool sent=bus_.endTransmission(false)==0;
    if(!queued || !sent) return false;
    if(bus_.requestFrom(address,n)!=n) {
      for(size_t i=0;i<n && bus_.available();++i) bus_.read();
      return false;
    }
    for(size_t i=0;i<n;++i) { if(!bus_.available()) return false; const int b=bus_.read(); if(b<0) return false; out[i]=uint8_t(b); }
    return true;
  }
  Bus &bus_;
  ImuState state_=ImuState::Off;
  ImuCandidate candidates_[2];
  uint8_t probe_=0, address_=0, revision_=0, ctrl1_=0, step_=0;
  bool selected_=false;
  ImuSample sample_;ImuPoll metadata_;
  uint32_t last_poll_=0, errors_=0;
  const char *poll_status_="IMU: waiting";
};
} // namespace v3
