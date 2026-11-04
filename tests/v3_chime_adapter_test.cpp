#define TRAILGUIDE_CHIME_HOST
#include "../trailguide_v3/chime.h"
#include <cstdio>
#include <cstdlib>
#include <thread>
#ifdef TRAILGUIDE_CHIME_EXPECT_SOFTWARE_ATOMICS
static_assert(!std::atomic<uint32_t>::is_always_lock_free,"Exercise the SDK-like atomic trait");
#endif
inline std::atomic<uint32_t> rendezvous{0};
void concurrent_test(uint32_t base) {
  using namespace v3audio;
  const int initialSpawns=sdk::spawns,initialWrites=sdk::writes,initialDrains=sdk::drains;
  const uint32_t initialCompleted=diagnostics.completed.load();
  service(true,base);
  sdk::onWrite=[] {
    rendezvous=1;
    while(rendezvous.load()!=2) std::this_thread::yield();
  };
  std::thread workerThread([] { sdk::execute(); });
  while(rendezvous.load()!=1) std::this_thread::yield();
  assert(diagnostics.state==Playing && cancelRequested==0);
  service(false,base); service(true,base);
  assert(cancelRequested==1); // ON must not release the old worker's latch.
  service(true,base+1); // Busy new edge is dropped, not delayed.
  assert(sdk::spawns==initialSpawns+1 && cancelRequested==1);
  rendezvous=2;
  workerThread.join(); sdk::onWrite=nullptr;
  assert(sdk::writes==initialWrites+1 && sdk::live==0 && sdk::enabled==0 && sdk::drains==initialDrains);
  assert(diagnostics.state==Cancelled && diagnostics.completed==initialCompleted);
  service(true,base+1); assert(sdk::spawns==initialSpawns+1 && cancelRequested==1);
  service(true,base+2); assert(sdk::spawns==initialSpawns+2 && cancelRequested==0);
  sdk::execute(); assert(diagnostics.state==Done && diagnostics.completed==initialCompleted+1 && sdk::live==0);
  puts("PASS real cross-thread OFF-ON sticky cancel, dropped edge and clean owner handoff");
}
void failure_test(int mode) {
  using namespace v3audio;
  sdk::fault=mode==8 ? 6 : mode;
  if(mode==8) sdk::onWrite=[] { service(false,1); };
  service(true,1);
  if(mode!=7) sdk::execute();
  sdk::onWrite=nullptr;
  const uint32_t expected=mode==7 ? uint32_t(TaskError) : uint32_t(300+(mode==8 ? 6 : mode));
  assert(diagnostics.completed==0 && diagnostics.failures==1);
  const bool locked=mode==5 || mode==6 || mode==8;
  assert(diagnostics.state==(locked ? Locked : Error));
  if(mode==5 || mode==6 || mode==8) assert(diagnostics.cleanupError==expected);
  else assert(diagnostics.error==expected);
  const int spawns=sdk::spawns;
  sdk::fault=0;
  service(true,2);
  if(locked) {
    assert(sdk::spawns==spawns && sdk::live==1 && !sdk::task);
    service(false,3); service(true,4);
    assert(sdk::spawns==spawns && diagnostics.state==Locked);
  } else {
    assert(sdk::spawns==spawns+1); sdk::execute();
    assert(diagnostics.state==Done && diagnostics.completed==1 && sdk::live==0 && sdk::enabled==0);
    assert(diagnostics.error==0 && diagnostics.cleanupError==0);
  }
  printf("PASS SDK fault %d honest failure, cleanup and owner gate\n",mode);
}
int main(int argc,char** argv) {
  if(argc>1) { failure_test(std::atoi(argv[1])); return 0; }
  using namespace v3audio;
  service(false,0); service(true,0); service(false,1); service(true,1);
  assert(sdk::spawns==0 && sdk::allocated==0);
  service(true,2);
  assert(sdk::spawns==1 && sdk::allocated==0 && diagnostics.state==Starting);
  sdk::execute();
  assert(diagnostics.state==Done && diagnostics.completed==1);
  assert(sdk::live==0 && sdk::enabled==0 && sdk::exits==1);
  assert(diagnostics.toneFrames==11520 && diagnostics.zeroFrames==1024);
  assert(sdk::output.size()==12544*4 && sdk::drains==1);
  service(true,2); assert(sdk::spawns==1);
  // Busy edges are consumed, never queued, including during cleanup.
  service(true,3); service(true,4);
  assert(sdk::spawns==2);
  sdk::onWrite=[] { service(true,5); };
  sdk::onDelete=[] { service(true,6); };
  sdk::execute(); sdk::onWrite=nullptr; sdk::onDelete=nullptr;
  service(true,6); assert(sdk::spawns==2 && sdk::task==nullptr);
  // Mute before worker starts does not initialize hardware.
  const int alloc=sdk::allocated;
  service(true,7); service(false,7); service(true,7); sdk::execute();
  assert(sdk::allocated==alloc && diagnostics.state==Cancelled);
  // Mute during writing aborts the rest; re-enable cannot undo cancellation.
  const int writes=sdk::writes,drains=sdk::drains;
  service(true,8);
  sdk::onWrite=[] { service(false,9); service(true,9); };
  sdk::execute(); sdk::onWrite=nullptr;
  assert(sdk::writes==writes+1 && sdk::drains==drains && sdk::live==0 && sdk::enabled==0);
  assert(diagnostics.state==Cancelled);
  service(true,9); assert(!sdk::task);
  service(true,10); sdk::execute();
  assert(diagnostics.state==Done && diagnostics.completed==3 && sdk::live==0);
  puts("PASS busy edges dropped, mute cancels queued/active, fresh edge resumes");
  puts("PASS quiet boot/off/on and asynchronous TX-only edge cue cleanup");
  concurrent_test(11);
  concurrent_test(0xfffffffeu); // Genuine edge through sequence wrap to zero.
}
