# Code review — TrailGuide v3-0.1.21

## Executive assessment

**The design has sound foundations and substantial behavioral tests, but the current source is not an all-green release gate.** Keep the accepted device running; make the recommended corrections as a separately approved, tested increment rather than a broad rewrite.

The strongest aspects are small fixed-size navigation/sensing models, explicit resource ownership, real-LVGL framebuffer and pointer tests, negative sensor/parser cases, and careful separation of optional sound from UI threads. The main weaknesses are dense duplicated lifecycle code, development-machine-dependent verification, incomplete end-to-end integration boundaries, and several bounded correctness/robustness defects.

**Efficiency:** the recognizers are allocation-free and the reveal does not allocate every frame. This is structurally appropriate for the target. Actual renderer latency, loop/poll gaps, peak contiguous memory, power and battery endurance have not been measured by this review; no FPS or battery claim follows from passing host tests.

### Scope and provenance

- Reviewed the **accepted v3-0.1.21 working source**, based on commit `a96734776c63f7ee77a7dc4c5c42abb14a9ce507`. The integration was uncommitted when frozen; that commit alone is the preceding release, not the reviewed firmware payload.
- Frozen inventory: **150 repository files**. Review or generator/contract validation covers **120 in-scope firmware, test, tooling, content and browser-artifact files**, with no uncovered file in those directories. Generated outputs were checked against generators rather than independently re-authored.
- Current V3: all 17 production files reviewed; retained V1, V2, bring-up, generators, browser studies and tests were reviewed separately to avoid attributing legacy issues to the installed device.
- Scope excludes an exhaustive audit of vendor/SDK dependencies, private credentials, field trials and intrusive device testing. Existing dependency pins are not a security certification.
- Review snapshot fingerprint: `ddb9432376bba2399e20812bf913c981bf79cf76de719c81310f7c94b243cd60`.
- **No implementation fixes, device operations, commits or pushes were performed for this review.** The pending publication is paused; this document does not claim publication.

### Findings at a glance

There are **29 distinct review items** after merging the duplicate native-content validation finding: 16 confirmed behaviors/limitations, 10 risks and 3 improvement ideas. A confirmed tooling limitation is not necessarily a deployed runtime defect. Severity: 20 medium, 8 low and 1 informational; no high/critical item was identified in the inspected scope. This is not a guarantee that none exists.

## Recommended order of work

1. **Restore an honest current verification gate — TQ-01, TQ-04, TQ-05.** Replace the obsolete whole-header approval boundary with presentation-specific evidence, retain geometry/pixel constraints, and make every attempt finish with a bounded, explicit success/failure/timeout report. Do not simply update a hash to silence the failure.
2. **Harden local diagnostic evidence — VR-03.** Reject embedded NUL/unsupported control bytes before dispatching any command; verify malformed lines cannot clear or replace a frozen capture. Recorder default-OFF limits normal field exposure.
3. **Close hard-fault integration — VR-01.** Preserve the useful bounded NotReady tolerance while distinguishing known bus/raw/clipping faults at the driver-to-shake boundary. Retain accepted physical thresholds; this is not a sensitivity change.
4. **Close the optional sound ownership window — VR-02.** First reproduce the actual loop-order case with UI plus adapter. Ensure an unconsumed event belonging to a replaced card cannot start later; preserve mute/busy-drop/worker cleanup behavior. Sound is OFF by default.
5. **Make build mutation fail safely — TOOL-01/02/03.** Check every mutation destination against symlink escape, stage output transactionally, and reject content outside native integer/string contracts before writing.
6. **Improve the development loop — TQ-02/03/06/08/09.** Separate portable source checks from local release-preservation audits, make expensive cache rebuilding explicit, bind artifacts to their producer and record toolchain provenance.
7. **Measure before optimizing — VR-04/05/07.** Test object-heap pressure separately from raster-allocation fallback; measure projection/flush, recorder-freeze and loop gaps before changing rendering or allocation policy.
8. **Maintain readability deliberately — VR-06, ARCH-01.** Expand compressed lifecycle routines into reviewable steps before considering a small shared presentation-session helper. Keep target-specific drivers, geometry and memory policy separate. Defer optional legacy/browser fixes according to actual use.

## Verification results and limits

### Existing evidence independently checked during this review

- The current canonical native attempt records **66 successful child commands**, **533 archived framebuffer images** and **25 archived binaries**. These are different units, not 66 test cases or a coverage percentage.
- Archive hashes/CRCs and matching source fingerprints were checked. The native pipeline uses actual pinned LVGL with sanitizer-enabled current C++ and verified reused C objects; reuse provenance is explicit.
- Six content/browser generation checks and the retained V1 firmware/OTA host suites had passing evidence. The relevant generated artifacts also matched in-memory regeneration during review.
- Installation/readback and qualitative approval in both modes precede this review; no new physical acceptance is inferred.

### Fresh review checks

- One safe Python subset ran **49 cases: 46 passed, one failed, two skipped**. The failure is the obsolete compact-layout whole-header hash (TQ-01); the skips require native-produced protocol artifacts absent from the source-only snapshot.
- The parent independently reran the compact-layout check: **two tests, one failure, exit 1**. This confirms a failing approval boundary, not demonstrated visual damage.
- The parent retained and compiled a small synthetic probe against actual driver/detector/serial headers. It reproduced the hard-fault distinction gap (one event with current health forwarding versus zero with explicit fault forwarding) and an embedded-NUL `CLEAR` line clearing a frozen one-row capture. Probe success means the undesirable current behavior was confirmed, not fixed.
- Other reviewers ran focused pure/mocked checks. Their overlapping test counts are **not added together** as if they were disjoint coverage.
- The published-but-unserviced sound-event issue is source-order evidence, not a newly executed full-LVGL/audio composition test.

### Not verified

- **No completed passing full current isolated Python suite.** Its latest attempt was interrupted, with no completion report; 86 passes from the previous release must not be reused as a current result.
- Heavy/browser/target reruns were limited by available storage below the local 2 GiB reserve. This constrains verification; it does not establish firmware failure.
- No instrumented branch/line coverage, physical FPS, sunlight contrast, glove/wet touch, battery, audio loudness or long-duration field reliability measurements.
- The original intermittent flip/stuck-compass/blank-screen root cause remains **OPEN**. The successful global restart feature is not a causal diagnosis or proof of repair.

## What is well written

- Fixed-size models and independent shuffle/context state keep content selection separate from decorative animation and hardware details.
- IMU initialization checks identity and exact register readback; conversion, configured range and safety constants are coupled rather than independently guessed.
- Persistent motion recognizers use bounded state and latching instead of allocating or queuing new events on every frame.
- UI and renderer ownership is noncopyable; timer/image lifetimes and many independent-deletion paths have real regression tests.
- The sound backend uses an output-only worker, atomic handoff, finite operations and sticky cancellation rather than touching LVGL from the audio task.
- Diagnostic transport has bounded chunks, session/CRC validation and substantial malformed-input tests, despite the framing gap found here.
- Tests inspect real pixels, actual pointer-driver behavior, exact timing boundaries and resource-failure domains—not only strings or method return values.

## Detailed findings

Classification meanings: **confirmed** = source/probe-supported current behavior or limitation; **risk** = plausible conditional failure or missing evidence, not a reproduced field defect; **idea** = optional improvement. Priorities below express recommended work order, not permission to implement.

### VR-01 — Configured state is passed as shake health, so known read/clipping faults preserve partial intent

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/trailguide_v3.ino:79-87`; `trailguide_v3/imu.h:95-125`; `trailguide_v3/ui.h:32-43`; `trailguide_v3/coaching_ui.h:40-50`; `trailguide_v3/shake.h:33-46`.
- **Impact:** A short known bus error, all-zero/all-FF rejection or rail clip is treated like ordinary NotReady. Partial shake can resume and draw without fresh quiet acquisition. This is an integration gap, not evidence that invalid frames themselves contribute peaks, and not a diagnosis of the historical intermittent flip/blank failure.
- **Evidence / limits:** Fresh strict host probe compiled the actual Imu and Shake headers using the existing fake Bus. After 30 quiet polls and 35 burst polls, inject one short status read, then 100 burst polls without quiet. Actual loop health expression state()==Running yields one event. Forwarding hard fault bits 8|32|64 as unhealthy yields zero. At the fault both cases report state=6 (Running), flags=8, valid=0, errors=1. The same missing distinction exists in both UI owners. Driver metadata already distinguishes NotReady (16) from hard rejection (8/32/64). Parent independently reproduced the reported behavior with retained review-only probe source.
- **Recommendation:** Pass an explicit poll-health classification from the driver/last_poll metadata; retain the accepted bounded NotReady pause but reset shake on read-error/raw-invalid/clipped polls. Do not equate all sample.valid=false with a hard fault, and do not retune accepted thresholds.
- **Validation:** Add actual sketch-dispatch tests for partial shake plus a single status-read error, raw short read, all-zero/FF frame and each clipping domain. In both modes require zero draw until fresh quiet and a full new burst. Preserve the existing intermittent-NotReady positive.


### VR-02 — An already-published but not yet serviced settlement cue survives a same-loop replacement draw

- **Classification:** confirmed; **severity:** medium; **effort:** medium.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/trailguide_v3.ino:70-103`; `trailguide_v3/ui.h:108,149,165-170`; `trailguide_v3/coaching_ui.h:141,159,175-180`; `trailguide_v3/sound_cue.h:5-8`; `trailguide_v3/chime.h:94-105`.
- **Impact:** With Sound ON, a canceled old card can start its chime during the replacement card's reveal. The new card can then also chime at its own settlement. This violates pending-cue cancellation, although normal playback completion and already-playing-cue policy are separate matters.
- **Evidence / limits:** Source-level deterministic schedule: (1) Lvgl_Loop runs the old 6000ms timer, finish(true) calls settled(), sequence becomes 1. (2) The same main-loop iteration polls IMU and completes an eligible gesture; draw() renders the replacement and cancel() clears only pendingSettlement, not the already-published SoundCue event. (3) At line 103 service(true,1) sees a fresh unconsumed sequence and creates the old cue worker. No draw generation or cancellation token accompanies the sequence. This is a same-thread event-order bug, not an atomic data-race claim. A fresh actual-LVGL boundary reproduction was not run in this resource-constrained review. Parent qualification: this is a source-confirmed ordering window; reproduce the composed UI/adapter boundary before choosing a fix.
- **Recommendation:** Make the pending audio event generation-owned and cancelable until service accepts it; invalidate only the canceled draw's unconsumed event on replacement/navigation. Preserve monotonic event identity, muted consumption, busy-drop semantics and the existing worker's sticky OFF cancellation. Avoid decrementing a shared counter or simply clearing every new event.
- **Validation:** Compose actual LVGL, actual UI and host audio adapter: settle an old draw in Lvgl_Loop, complete flip or shake in that iteration before audio service, then assert no old task spawn. Cross both modes and navigation cancellation. Assert exactly one later cue at the new draw's settlement. Existing hidden-blackout tests exercise unpublished pendingSettlement, not this published/unconsumed window.


### VR-03 — Embedded NUL truncates a malformed serial command into a valid destructive command

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/motion_diagnostics.h:41-58`; `trailguide_v3/flight_recorder.h:69-104`.
- **Impact:** Malformed local serial input such as CLEAR followed by NUL and junk can clear frozen recorder evidence. REC followed by NUL can similarly be interpreted as a fresh recording command. This is offline diagnostic robustness, not a remote/network attack.
- **Evidence / limits:** Strict host probe compiled actual MotionDiagnostics/FlightRecorder. Create session 77 with one row, freeze at 120000ms, then feed bytes CLEAR\0junk\n through the real service parser. Output: before_state=2 count=1 after_state=0 count=0 consumed=11. service stores NUL as an ordinary byte; command() accepts the prefix C string and never sees trailing junk. Existing numeric-sign/overflow cases do not cover this framing defect. Parent independently reproduced the reported behavior with retained review-only probe source.
- **Recommendation:** Reject embedded NUL and unsupported control bytes for the entire line, discarding until newline as with overflow, or pass an explicit length to a parser that validates every byte. Preserve CRLF support without accepting an early string terminator as command completion.
- **Validation:** Feed NUL at each offset of CLEAR/REC/FREEZE/DUMP via the serial-facing service, including across its 16-byte input budgets. Preserve frozen header, rows and CRC on every malformed line; assert no new allocation/session/dump.


### VR-04 — Low-memory fallback protects raster/timers, but not UI object-tree rebuilds

- **Classification:** risk; **severity:** medium; **effort:** medium.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/ui.h:46-87,169,183-184`; `trailguide_v3/coaching_ui.h:53-94,179,213-214`; `trailguide_v3/ui.h:66-70`.
- **Impact:** Under actual LVGL object-heap exhaustion, the old root is deleted before the replacement is built; object creation and label/button creation are immediately dereferenced without a bounded fallback. A safe raster fallback is therefore not a general guarantee of complete-copy/control availability under object-memory exhaustion. Classic compass timer creation also lacks the free-block check used by CoachUi::startCompass. No exhaustion/crash was reproduced here.
- **Evidence / limits:** Both render() implementations delete the current root, then construct and style a new tree without checking the object creation result. WindowReveal::capture and hold() do preflight memory, while CoachUi::startCompass explicitly documents the pinned allocator asserting on exhaustion. Native sound tests wrap a null reveal timer and fail the external raster allocator; those are narrower failure domains than TLSF object allocation.
- **Recommendation:** Define a bounded object-memory failure policy: reserve for the maximum owned layout or retain/reuse a minimal preallocated accessible screen; validate creation at the appropriate allocator/assert boundary. Apply the established compass-timer guard consistently. Do not enlarge LVGL heap blindly or claim a null-return wrapper proves real allocator-exhaustion behavior.
- **Validation:** Use actual pinned LVGL with controlled heap pressure/fragmentation and assertion-aware isolation to exercise every screen rebuild, mode switch and compass timer path. Verify accessible content/controls or an explicit recoverable error, rather than only allocation return values.


### VR-05 — Recorder freeze computes the entire bitwise CRC synchronously on the UI/IMU loop

- **Classification:** risk; **severity:** medium; **effort:** small to medium.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/flight_recorder.h:35,46-51,93-97,126`; `trailguide_v3/trailguide_v3.ino:70-85`.
- **Impact:** At timeout/full/manual freeze, optional diagnostics can introduce a one-shot UI/IMU scheduling pause. Maximum capacity is 327680 bytes and 2621440 bit-loop iterations, plus byte processing. Its actual S3 duration and effect on the 60ms/120ms/250ms freshness gates are unmeasured; no claim of observed missed frames or watchdog reset is made.
- **Evidence / limits:** freeze() calls crc(records,count*sizeof(FlightRecord)) synchronously before setting state=2. recorder.tick is called before the IMU tick by MotionDiagnostics::service; append can also freeze in observe. Bounded serial chunks do not bound this computation. Arithmetic was executed with Python, not inferred from a timing benchmark.
- **Recommendation:** First measure full/normal-120s freeze latency. If material, incrementally update CRC when appending the fixed row bytes or amortize a post-freeze CRC with a distinct checksum-pending state. Preserve exact row order, existing CRC ABI and frozen evidence ownership.
- **Validation:** Compare incremental versus existing CRC for empty/single/full captures; ensure manual/timeout/full races retain reason/end/count. On an authorized later bench run measure worst loop gap and IMU poll gap with diagnostics off, recording, freeze and dump.


### VR-06 — Duplicated compressed lifecycle code makes fixes difficult to audit across owners

- **Classification:** idea; **severity:** low; **effort:** medium.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/ui.h:108-120,149-185`; `trailguide_v3/coaching_ui.h:139-219`.
- **Impact:** Blankout masks, reveal/hold/cue ownership, timer cancellation, contact resets and widget factories are independently implemented in dense single-line methods. Asymmetries already include Classic versus Coaching compass timer preflight and callback registration. This is review/maintenance debt, not evidence of a current use-after-free.
- **Evidence / limits:** The same lifecycle transitions are duplicated in the cited sections, often with multiple independent state changes on one physical line. Ui also registers body deletion twice (label() plus render()), which currently leads to idempotent repeated cancellation rather than a demonstrated fault.
- **Recommendation:** In a separately authorized increment, expand methods into explicit steps first and name cancellation versus same-card settlement. Consider a small owned presentation-session helper after behavior is locked by existing tests. Keep geometry, copy, timers and accepted detector policy byte/behavior stable; do not bundle a broad refactor into a motion correction.
- **Validation:** Retain both-owner deletion, blackout, focus-deadline and pending-cue matrices; compare actual native intermediate/settled rasters and real pointer behavior before/after any extraction.


### VR-07 — Measure projection cost before optimizing nominal 25Hz full-raster work

- **Classification:** idea; **severity:** low; **effort:** measurement small; optimization medium.
- **Scope:** Current V3 runtime.
- **Source:** `trailguide_v3/window_reveal.h:58-95`; `trailguide_v3/ui.h:169-170`; `trailguide_v3/coaching_ui.h:179-180`.
- **Impact:** At the requested 40ms timer cadence the raster loop visits 896000 pixels/second and can perform 1792000 integer coordinate divisions/second, apart from shading and LVGL/display work. Those are static nominal work estimates, not measured achieved FPS or processor utilization. Render work continues during qualified blackout and the initial zero-light interval.
- **Evidence / limits:** The fixed 256x140 destination is traversed for each render; coordinate division precedes the light test. Both UI tick callbacks render even when the root children are hidden for blackout. The source owns only one source/destination allocation per active reveal, so this is not a per-frame allocation leak.
- **Recommendation:** Measure renderer, LVGL refresh and flush separately. If needed, fast-fill the zero-light destination, skip projection during hidden blackout while preserving wall-clock reveal phase, or evaluate coordinate stepping. Preserve exact native raster settlement and blackout/restore behavior; do not replace the accepted effect with a fade.
- **Validation:** Collect p50/p95/max durations and loop/poll gaps later without retaining participant data. Native pixel equality should cover intermediate frames, blackout across settlement, clipping, complete text and final raster/live handoff.


### TQ-01 — Current Python gate fails because the compact UI freezes the entire obsolete header hash

- **Classification:** confirmed; **severity:** medium; **effort:** S for scoped approval-boundary correction; M for presentation-only extraction.
- **Scope:** Development verification / test quality.
- **Source:** `tests/v3_compact_layout_test.py:8-13`; `trailguide_v3/coaching_ui.h:31-49`; `README.md:51`; `docs/V3-TRIAL.md:9-19`.
- **Impact:**
- **Evidence / limits:** Executed the unchanged snapshot test twice. Expected febf7ac10fee1d5ec10e2c003eaaf603cf4deb082755b2d8ed365238742c1dd1; actual 831633d797b46c22f0789d647422ab6843f347cd743fc4312b465dc08d8e6626. Standalone invocation exited 1: 2 tests, 1 failure. Snapshot manifest matches the actual header. This is a stale approval/test boundary, not proof of a rendering regression.
- **Recommendation:** Retain approved presentation constraints using semantic/pixel/geometry checks or a presentation-only digest; document authorized policy-only changes. Do not blindly bless the new whole-header hash. Re-run the complete isolated current Python gate when space permits.


### TQ-02 — Main verification paths are not reproducible from the repository-owned source snapshot alone

- **Classification:** confirmed; **severity:** medium; **effort:** M.
- **Scope:** Development verification / test quality.
- **Source:** `tools/test_v3_app.py:22-23,125-134`; `tools/test_v3_hwcdc_budget.py:14-22,37-40`; `tools/test_v3_isolated.py:29-38,44-70`; `tools/test_v3_range_focused.py:25-35,44-47`; `tests/compass_coaching_test.py:16-19,133-136`.
- **Impact:**
- **Evidence / limits:** Full native runner requires a specific local Arduino SDK and private installed 1.9 IMU/Gesture baselines/inventory. Isolated Python requires many private installed root inventories, an archive, and TMPDIR. os.link crosses ROOT to TMPDIR without an EXDEV fallback. Browser paths and scratch conventions are fixed to Linux/Hermes. These dependencies are not in source manifest. No clean checkout execution was attempted.
- **Recommendation:** Publish a read-only prerequisite manifest/preflight and split portable source checks from local immutable-release audit. Supply redistributable pinned synthetic baselines or an explicitly provisioned CI artifact. Resolve browser/compiler paths, use sys.executable, support a same-filesystem temp root or copy fallback, and keep protected-root checks fail-closed.


### TQ-03 — Focused cache miss silently escalates to the full native suite and preparation

- **Classification:** confirmed; **severity:** medium; **effort:** M.
- **Scope:** Development verification / test quality.
- **Source:** `tools/v3_native_objects.py:45-52`; `tools/test_v3_coaching.py:12-15,35`; `tools/test_v3_range_focused.py:44-48,59`; `tools/test_v3_app.py:22-39,125-134`; `tests/v3_native_objects_test.py:42-50`.
- **Impact:**
- **Evidence / limits:** Any OSError/KeyError/ValueError/AssertionError in cache verification invokes tools/test_v3_app.py, which calls the SDK budget check, prepare(), compiles/runs the full suite, and requires historical private baselines. Focused runners advertise no target/hardware/network but do not expose this cost as a flag; no free-space guard is present on this escalation. Native cache tests explicitly enforce the fallback.
- **Recommendation:** Default focused runs to fail with a precise preflight diagnosis; require explicit --build-cache/--allow-full-suite. Separate building verified LVGL objects from executing unrelated tests. Apply resource checks before rebuilding.


### TQ-04 — Isolated-run timeout/interruption loses completion evidence and post-run immutable checks

- **Classification:** confirmed; **severity:** medium; **effort:** S-M.
- **Scope:** Development verification / test quality.
- **Source:** `tools/test_v3_isolated.py:78-84`; `tests/v3_storage_isolation_test.py:15-32`.
- **Impact:**
- **Evidence / limits:** subprocess.run(timeout=600) occurs before log/report writes and after-only inventory verification, with no try/finally for these steps. TimeoutExpired exits before all three. The TemporaryDirectory context does clean the sandbox; the cleanup test does not test timeout evidence publication. Parent context reports the latest interrupted isolated run left an empty log/no JSON; not independently used as a passing result.
- **Recommendation:** Stream child logs to an owned attempt file and write a final machine-readable timeout/interrupted/failed status plus post-run inventories in finally. Ensure child/browser process groups are reaped before deleting the sandbox. Add injected timeout and inventory-failure tests.


### TQ-05 — Some canonical native children have no timeout, allowing a detector regression to hang the gate

- **Classification:** confirmed; **severity:** medium; **effort:** S.
- **Scope:** Development verification / test quality.
- **Source:** `tools/test_v3_app.py:121-135`; `tests/v3_app_gesture_test.cpp:147-152`; `tools/replay_v3_recovery.py:53-55`.
- **Impact:**
- **Evidence / limits:** Five pure native child runs, the differential binary, and the Python protocol child omit timeout. Gesture test contains while(!cool.tick(cool.up)){} with no iteration cap; a failure to fire can hang instead of emitting a useful test failure. Replay subprocess also has no timeout. Other LVGL/chime runners do set bounds.
- **Recommendation:** Add uniform child wall-clock budgets and bounded expected-event loops that print the seed, sample index and detector diagnostic on exhaustion; include compile timeouts and runner-level cancellation cleanup.


### TQ-06 — Cache integrity is stronger than its build-provenance key

- **Classification:** risk; **severity:** medium; **effort:** M.
- **Scope:** Development verification / test quality.
- **Source:** `tools/v3_native_objects.py:11-25,27-42`; `tools/test_v3_app.py:29-39`; `tools/test_v2_ui.py:13-24`; `tools/test_v3_ui.py:21-26`; `tests/v3_native_objects_test.py:31-50`.
- **Impact:**
- **Evidence / limits:** Current key covers lv_conf.h, flags, source path and C source bytes; manifest hashes object files and library files. It does not record compiler identity/version/target, sanitizer runtime or transitive-header dependency hashes. Full runner reseals existing objects without recompiling them. Legacy V2/bringup caches are weaker mtime-based variants. No actual stale object was observed.
- **Recommendation:** Include compiler identity/target and the complete pinned library/header digest in the cache key; use dependency files or an archive digest tied to verified extracted contents, and atomic manifest publication. Test compiler/header/config changes independently.


### TQ-07 — No single test exercises raw-register shake to global routing to the real audio adapter in the sketch loop

- **Classification:** risk; **severity:** medium; **effort:** M.
- **Scope:** Development verification / test quality.
- **Source:** `tests/v3_app_motion_integration_test.cpp:31-36,59-77,88-116`; `tests/v3_shake_integration_native_test.cpp:14-16,33-56`; `tests/v3_sound_ui_test.cpp:37-60`; `tests/v3_chime_adapter_test.cpp:10-33,35-60`; `tools/test_v3_app.py:63-69`; `trailguide_v3/trailguide_v3.ino:73-103`.
- **Impact:**
- **Evidence / limits:** Actual sketch dispatch integration proves raw IMU conversion/flip/recorder with synthetic bus, but not the sustained-shake sequence across screen owners. Global shake tests inject already-converted ImuSample values directly. Sound UI verifies sequence counters, while adapter tests independently exercise service and worker faults. The generated sketch motion slice ends before the chime service call. This is compositional evidence, not a demonstrated bug.
- **Recommendation:** Add one economical real-loop synthetic raw shake case, including NotReady gaps, Coaching delegation, replacement during reveal, unread USB, Sound ON settlement, cancellation and exactly one adapter start. Keep device acceptance separate.


### TQ-08 — Artifact-dependent protocol tests can skip or consume stale fixtures without binding to their producer

- **Classification:** risk; **severity:** low; **effort:** S-M.
- **Scope:** Development verification / test quality.
- **Source:** `tests/v3_recorder_protocol_test.py:11-19,41-50`; `tools/test_v3_isolated.py:71-75`; `tools/test_v3_app.py:121-135`; `tests/v3_app_recorder_test.cpp:33,74`.
- **Impact:**
- **Evidence / limits:** Two cross-language checks skip when synthetic wire files are absent; present files have no producer source/build digest. Isolated runner optionally copies existing synthetic files. The archived canonical attempt ran all 11 protocol methods successfully after the writer, so this does not invalidate that attempt. Review scoped run correctly reported two skips.
- **Recommendation:** Make artifact production an explicit dependency in a cross-language integration tier and bind fixture metadata to current writer hash. Require zero skips for this release tier while keeping a separate pure-parser tier usable without native tools.


### TQ-09 — Recovery replay selects an existing binary without confirming its source/build identity

- **Classification:** risk; **severity:** low; **effort:** S-M.
- **Scope:** Development verification / test quality.
- **Source:** `tools/replay_v3_recovery.py:24-49,53-62`; `tests/v3_app_contract_test.py:80-92`.
- **Impact:**
- **Evidence / limits:** Replay checks selected row gates, monotonic at timestamps and retained origin, but invokes the fixed host executable without cache/build/source verification and records rows/exit/scope only. The contract test mocks subprocess and verifies the path, not binary provenance or negative parse cases. No stale replay execution was observed.
- **Recommendation:** Record capture hash, selected range, executable/source/config hashes and reject a stale binary; add malformed/empty/wrong-gate/duplicate/rollover cases with explicit errors and timeout. Continue labeling the seeded Recover window as partial replay, not physical acceptance.


### TOOL-01 — Intermediate symlinks bypass preparer output containment

- **Classification:** confirmed; **severity:** medium; **effort:** medium.
- **Scope:** Local developer tooling.
- **Source:** `tools/prepare_v3.py:22-39`; `tools/prepare_v3_bringup.py:22-35`.
- **Impact:** Only private is resolved and guarded. Subsequent recursive deletion targets include unchecked intermediate path components. A prepared symlink can redirect the final directory outside the allowed root. Resolving PRIVATE itself also lets an anchor symlink redefine the allowed tree unless a separately protected destination catches it.
- **Evidence / limits:** Scratch-only path resolution; no preparer or deletion executed. {'root_guard_accepts': True, 'destination_outside_allowed': True}
- **Recommendation:** Reject symlinked output ancestors; resolve and validate each mutation destination against an independent canonical anchor and protected roots; add isolated symlink guard tests. Use descriptor-relative no-follow operations if concurrent hostile mutation is in scope.


### TOOL-02 — Preparation failure leaves old completion manifest over partial outputs

- **Classification:** confirmed; **severity:** medium; **effort:** medium.
- **Scope:** Local developer tooling.
- **Source:** `tools/prepare_v3.py:30-39`; `tools/prepare_v3.py:43-93`; `tools/prepare_v3.py:117`; `tools/prepare_v3_bringup.py:26-35`; `tools/prepare_v3_bringup.py:39-79`; `tools/prepare_v3_bringup.py:94`.
- **Impact:** Existing output subtrees are deleted before archive extraction, source reads and content rendering complete. The previous preparation.json is neither invalidated nor transactionally coupled to those outputs. Exceptions leave a mixed/partial tree and stale success provenance.
- **Evidence / limits:** Static control-flow inspection. First rmtree precedes all own-source reads and renderer validation; manifest is written only at the end; no staging, rollback or invalidation exists
- **Recommendation:** Prevalidate all inputs, assemble a contained staging tree, and publish it with an explicit completion protocol. Preserve previous complete output; at minimum invalidate old completion evidence before mutation.


### TOOL-03 — Firmware card rendering accepts out-of-range revisions and unsupported controls

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** Local developer tooling.
- **Source:** `tools/build_prototype.py:12-13`; `tools/build_prototype.py:46-49`; `tools/build_v2_content.py:55-64`; `tools/build_v3_content.py:13-16`.
- **Impact:** Positive revision validation has no uint32 ceiling despite the generated uint32_t field. ASCII-only string validation permits NUL and other unsupported controls, then JSON serialization is used as a C++ literal encoder.
- **Evidence / limits:** In-memory mutations of canonical input passed to build_v2_content.render. {'revision_4294967296_accepted_and_emitted': True, 'nul_title_accepted_and_unicode_escape_emitted': True}
- **Recommendation:** Bound native revisions to 1..0xffffffff and use a shared native printable-character/C++ string policy, with explicitly supported newline handling. Add overflow and control-character cases.
- **Validation:** Add mutation cases for uint32 revision boundaries and forbidden controls; require a clear rejection before any output write. This merges the independent P-05 observation.


### TOOL-04 — Serial constructor failures leak the opened descriptor

- **Classification:** confirmed; **severity:** low; **effort:** small.
- **Scope:** Local developer tooling.
- **Source:** `tools/v3_flight_recorder.py:94-104`; `tools/v3_flight_recorder.py:241-261`.
- **Impact:** Port construction occurs before main's finally scope. Constructor does not close its descriptor on setup failure. CLI process exit limits current operational impact.
- **Evidence / limits:** Fully mocked open/flock/termios/close; no actual port opened. {'tcgetattr_raised': True, 'close_calls': 0}
- **Recommendation:** Close fd and re-raise on constructor setup failure; add failure-injection tests for each setup operation.


### LEG-01 — V2 auxiliary label lifetimes are not covered by teardown invalidation

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** optional legacy V2 maintenance; not present V3.
- **Source:** `trailguide_v2/ui.h:31-34`; `trailguide_v2/ui.h:72,163-178,184-193`; `trailguide_v2/ui.h:139-143`; `tests/v2_ui_test.cpp:311-326,379-391`.
- **Impact:** After external Home-screen cleanup or direct status-label deletion, the next readiness transition can write a dangling LVGL pointer. Independent deletion of the second-button caption during a reveal can reach label APIs with null at finish. Normal owned render resets connectionLabel before the next loop, so ordinary Home/Card navigation is not asserted to fail.
- **Evidence / limits:** Confirmed by source ownership/call-path inspection; no new runtime or sanitizer reproduction. No normal installed-device failure claimed.
- **Recommendation:** If V2 is reopened, invalidate the status-label pointer on DELETE and make revealCaption tolerate a missing caption; add focused real-LVGL tests for Home cleanup plus readiness change and caption-only deletion plus timer completion. Prefer a contained V2 fix, not changes to accepted V3.
- **Validation:** Confirmed by source ownership/call-path inspection; no new runtime or sanitizer reproduction. No normal installed-device failure claimed.


### LEG-02 — V2 model count and generated card count can diverge on a future pool edit

- **Classification:** risk; **severity:** low; **effort:** small.
- **Scope:** optional legacy V2 future content-selection changes; not current mismatch.
- **Source:** `trailguide_v2/model.h:12-14`; `trailguide_v2/ui.h:79`; `tools/build_v2_content.py:50-66`.
- **Impact:** A future shorter generated selection could leave the model indexing beyond the array; a longer selection could leave cards unreachable. Current generator check confirms eight pairs, so no present bounds failure.
- **Evidence / limits:** Source analysis and successful current generator check.
- **Recommendation:** At a future V2 edit, add static_assert(Model::count == cardCount) at the UI/content integration boundary, retaining the pure model separation. Do not manually alter generated content.
- **Validation:** Source analysis and successful current generator check.


### LEG-03 — Measure V2 runtime contiguous-heap and loop latency before enabling more work

- **Classification:** risk; **severity:** low; **effort:** small to medium.
- **Scope:** optional legacy V2 runtime qualification; V3 retains separate PSRAM allocator.
- **Source:** `trailguide_v2/window_reveal.h:12-13,31-46,57-93`; `trailguide_v2/trailguide_v2.ino:23,140,143-147`; `trailguide_v3/trailguide_v3.ino:16-19`.
- **Impact:** V2 needs a contiguous 106240-byte raster while its 13760-byte partial draw buffer, separate 48 KiB LVGL arena and networking also exist. Fallback is safe, but aggregate free heap at startup does not establish successful repeated capture, fragmentation resistance or target frame latency. No current exhaustion/stall is established.
- **Evidence / limits:** Allocation sizes calculated from source; no target measurements performed.
- **Recommendation:** Only if continuing V2, collect target largest-free-block and loop/frame-time high-water marks across reveal, reconnect and status traffic; preserve instant complete-copy fallback and avoid increasing the LVGL heap or copying S3 PSRAM assumptions. Verify partial-client WebServer behavior from the exact pinned implementation before making responsiveness claims.
- **Validation:** Allocation sizes calculated from source; no target measurements performed.


### LEG-04 — Legacy bring-up does not carry current V3 serial backpressure hardening

- **Classification:** risk; **severity:** low; **effort:** small.
- **Scope:** optional reuse of historical S3 bring-up; not current V3.
- **Source:** `trailguide_v3_bringup/trailguide_v3_bringup.ino:21-23,77-87`; `trailguide_v3/trailguide_v3.ino:32-36`.
- **Impact:** An unread/full USB diagnostic transport is a qualification risk if the old bring-up sketch is reused; current application already contains the explicit mitigation. The absence of while(!Serial) alone is not proof of backpressure-safe writes. No new hardware hang reproduced.
- **Evidence / limits:** Source difference against the current documented in-tree mitigation, not a new SDK or hardware experiment.
- **Recommendation:** Keep bring-up historical; if deliberately reused, apply the same pinned-core positive-budget policy and test unread-monitor progress. Do not deploy the old sketch as a repair for accepted V3.
- **Validation:** Source difference against the current documented in-tree mitigation, not a new SDK or hardware experiment.


### ARCH-01 — Share narrow contracts/tests before consolidating target implementations

- **Classification:** idea; **severity:** informational; **effort:** medium; defer until a real second-target change justifies it.
- **Scope:** cross-target maintainability; optional future work touching current V3 requires its own gates.
- **Source:** `trailguide_v2/window_reveal.h:12-25,31-93`; `trailguide_v3/window_reveal.h:12-27,32-94`; `trailguide_v3_bringup/imu.h:48-50,78,97-98`; `trailguide_v3/imu.h:10-13,62-64,93,124`; `docs/V2-TRIAL.md:26,36-43`; `docs/V3-SPEC.md:1-3`.
- **Impact:** Copied authored algorithms can drift, but a single generic firmware/driver/UI would mix distinct geometry, sampling/range, heap, tick, board and ABI contracts. Generated content is deliberately excluded from duplication concerns. No current dependency collision was established.
- **Evidence / limits:** Full owned-source read plus targeted current-V3 comparison; preparation scripts inspected but not run.
- **Recommendation:** First share test vectors/contracts for pose/settlement, cancellation, range-scaling and OTA lifecycle, while retaining target-specific allocators, face geometry, LVGL configs, drivers and application ownership. Extract only demonstrated stable pure math or bounded bus helpers in a separately authorized change; preserve pinning and DMA completion adapter semantics.
- **Validation:** Full owned-source read plus targeted current-V3 comparison; preparation scripts inspected but not run.


### P-01 — Full dial replacement discards keyboard focus and does not announce the new view

- **Classification:** confirmed; **severity:** medium; **effort:** small-to-medium.
- **Scope:** Browser navigation accessibility; not deployed-device behavior.
- **Source:** `prototype/template.html:77-80,105-106,119-123`; `prototype/compass_coaching_template.html:50-53,74-76,84-85`; `tests/compass_coaching_test.py:301-304,446-449`.
- **Impact:** A focused Next, Another, Paths, Guide or Back node is removed even when a logically identical control remains. Neither renderer restores focus, and dial copy/view changes are not live regions. Keyboard and assistive-technology users lose their stable navigation position. Exact subsequent browser/AT focus behavior is not measured here.
- **Evidence / limits:** Source inspection: Buttons invoke navigation/render; every render replaces all dial children. Recreates buttons and focusable copy region, without preserving focus or selecting a destination. Tests explicitly focus a target before Enter; they do not verify uninterrupted keyboard navigation after replacement. No fresh browser or assistive-technology execution was performed.
- **Recommendation:** Reuse keyed controls or restore the same logical control when still available; on view changes choose a meaningful heading/copy/primary control and announce concise context, not every character. Preserve normal Tab order and avoid focus stealing on external fixture changes.
- **Validation:** Start with Tab only, activate navigation and repeated paging with Enter/Space, assert document.activeElement after each render, test Back context and one screen-reader announcement per view. Do not call focus from the test before every action.


### P-02 — Legacy Guide me does not expose all canonical guidance or activity

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** Legacy prototype/index.html content completeness only.
- **Source:** `prototype/template.html:66-75`; `content/cards.json:735-741`; `tools/build_v2_content.py:35-46`; `docs/PROTOTYPE-REVIEW.md:33,41`.
- **Impact:** The legacy study can review an invitation and short action but cannot review the complete activity or new optional help. Byte-current generation and pagination tests do not prove field-level parity. Safety/participation instructions located only in activity are absent from that browser route.
- **Evidence / limits:** Source inspection: guidePages includes title/action, invitation, notice and follow-ups only; activity, conditional_follow_ups and optional_ai_help are never consumed. tg-028 carries optional AI help and a conditional follow-up that are embedded but never rendered by this legacy guide. Native guide formatter includes conditional follow-ups, optional AI help and full activity. Review contract describes Try action/activity and opening its related activity. No fresh browser or assistive-technology execution was performed.
- **Recommendation:** Either extend the legacy guide with authored activity, optional help and condition-plus-question sections, or explicitly label the artifact as a reduced historical subset and stop using it to approve complete guidance. Keep conditional prompts gated in wording; do not flatten them into an unconditional question.
- **Validation:** For each selected card/face, concatenate all reachable guide pages and assert inclusion of required canonical scopes, including activity and tg-028 condition plus question; keep typography normalization explicit.


### P-03 — Legacy pointerleave clears a qualified release before a later click

- **Classification:** risk; **severity:** medium; **effort:** small.
- **Scope:** Legacy prototype/index.html touch compatibility sequence.
- **Source:** `prototype/template.html:80,163-168`; `prototype/compass_coaching_template.html:135-139`; `tests/prototype_test.py:304-324,354-365`.
- **Impact:** If a browser sends pointerup -> pointerleave -> compatibility click for a stationary touch, the legacy handler rejects a valid tap. The code path is clear; occurrence on a particular browser/device was not reproduced under the resource restrictions.
- **Evidence / limits:** Source inspection: pointerup qualifies release; pointerleave unconditionally clears it; non-keyboard click requires release to equal the button. Newer study explicitly preserves qualified releases across post-pointerup pointerleave, with an explanatory touch-order comment. Legacy checks cover mouse and constructed pointer/click sequences, but omit pointerleave between qualified pointerup and click. No fresh browser or assistive-technology execution was performed.
- **Recommendation:** Apply the newer preserve-qualified-release rule to the legacy study if it remains supported. Keep live-drag cancellation and stale-target protections; do not remove all click qualification.
- **Validation:** Add the exact event-order regression plus real touch taps, canceled drags and double contacts on supported browsers after resource headroom is restored.


### P-04 — Timing-study tests regenerate the accepted artifact before exercising it

- **Classification:** confirmed; **severity:** medium; **effort:** small.
- **Scope:** Test integrity and read-only maintainability.
- **Source:** `tests/v3_experience_test.py:26-32`; `tests/v3_experience_test.py:145-153`; `tests/compass_coaching_test.py:494-500`.
- **Impact:** Browser cases run alone can repair a stale/missing delivered timing-study HTML and pass against newly rebuilt bytes. The build test does check freshness first, so the full suite may detect staleness; the defect is hidden mutation and inability to trust browser-only delivered-artifact results, not inevitable full-suite false passing.
- **Evidence / limits:** Source inspection: A build test runs the generator without --check against its default tracked output. Every browser-test setup rebuilds the default generated page before loading it. Newer delivered-artifact test compares generated bytes and runs --check without repair. No fresh browser or assistive-technology execution was performed.
- **Recommendation:** Render synthetic/browser fixtures to scratch paths; verify the committed delivered artifact with --check and load it unchanged for a separate smoke test. Never repair source artifacts in setUp.
- **Validation:** Record artifact bytes and modification time before/after tests; deliberately stale a disposable artifact and ensure delivered-artifact checking fails rather than repairing it.

## Coverage and regression priorities

The test review maps 15 requirement/module areas to actual tests: card selection, flip recognition, shake recognition, global routing, reveal ownership, Coaching context, layout/overflow/compass, IMU conversion/health, serial diagnostics, recorder/protocol, sound, sketch-loop integration, preparation/cache/replay, browser/content generation, and retained targets. There is no measured coverage percentage.

The highest-value additional cases are:

- Driver raw data → sketch dispatch → both UI owners → real host audio adapter, including NotReady versus explicit hard faults and same-loop replacement.
- Every unsupported serial control byte across input-budget boundaries, with frozen rows/header/CRC unchanged.
- Actual object-heap exhaustion/fragmentation, not only a failed caller-buffer or wrapped null timer.
- Test-runner timeout/cancel/error completion, protected-root invariants and expensive cache escalation.
- Preparer symlink destinations and mid-stage exceptions; native-content integer/control boundaries.
- Browser keyboard focus across rerender and genuine touch event ordering, distinct from current native behavior.

## Efficiency and maintainability plan

1. Record bounded p50/p95/max renderer, LVGL-refresh and display-flush durations separately, plus real loop/IMU poll gaps.
2. Measure diagnostics OFF, recording, freezing and dumping. The full recorder CRC is synchronous; its actual device latency is unknown.
3. Track largest free object-heap block and caller/PSRAM minima during mode switches, repeated draws and cancellation. Do not confuse total free bytes with a usable contiguous allocation.
4. Only then evaluate skipping invisible projection work or incremental CRC. Retain exact approved intermediate/settled images, complete-copy access and wall-clock timing.
5. Format and name lifecycle steps first; extract small demonstrated common contracts later. Do not redesign working hardware support or merge distinct targets simply to reduce line count.
6. Establish a source-only portable check tier, a native integration tier, an optional local preservation audit and a separately authorized device acceptance tier. Each should state prerequisites, resource budget, produced artifacts and exact completion status.

## Additional tooling ideas

These are supplementary recommendations, not additional confirmed runtime defects or additions to the deduplicated 29-item count.

- **Atomic generated artifact and JSONL publication** — Avoid final-looking truncated derived files. Effort: small; classification: risk.
- **Bound offline capture/raw file reads and check hex length before decoding** — Limit accidental local resource exhaustion; no remote exploit claim. Effort: small; classification: risk.
- **Share V3 preparer planning/extraction and stream archive hashing** — Reduce duplicated safety logic and whole-archive allocations. Effort: medium; classification: idea.
- **Test or explicitly reject whitespace checkout paths in compiler flags** — Predictable supported host paths. Effort: small; classification: risk.
- **Keep trusted-LAN V2 scope explicit; optionally reuse image/readback gates for legacy V1** — Consistent local device safety without introducing V3 networking. Effort: medium; classification: idea.
- **Record installed toolchain versions/hashes in manifests** — Reproducibility; no unsupported CVE or upgrade recommendation. Effort: small; classification: idea.
- **Consistent UTF-8, readable CLI formatting and concise I/O/validation errors** — Maintainability and diagnostics. Effort: small; classification: idea.

## Decision boundary

These are documented findings and recommendations only. The accepted appearance, six-second reveal/five-second hold, global start-fresh gesture, Classic availability/boot default, Coaching content and optional output-only sound remain unchanged. No diagnosis of motivation, learner ability, consent or learning outcomes is implied by device behavior.

The companion UI review addresses discoverability, hierarchy, complete-copy reading and field-use validation separately. Discuss and approve a small remediation sequence before implementing or replacing the current device build.
