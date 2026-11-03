# V3 — read-only round-device trial

## Current installed checkpoint

**v3-0.1.8 DRAFT is installed and startup/UI/real-IMU verified.** It integrates the approved twelve-card content with the bounded RAM recorder, retaining the earlier recovery/logging corrections. Compass appearance and touch were reported working on the unchanged v3-0.1.5 layout; a complete physical flip on the corrected detector remains unaccepted. Twelve draft opening/cue/help sets are updated as endorsed; [V3-CARD-CONTENT.md](V3-CARD-CONTENT.md) details the two narrowly scoped base-activity clarifications. Reveal composition, fonts, geometry, sensor/gesture behavior and optional peripherals are unchanged.

The coordinated v3-0.1.5 trace contained 315 fresh, valid, resting, eligible records stuck in Recover, with current gravity nearly opposite the retained origin (alignment about -0.989). Old recovery rejected this position indefinitely. The corrected code retains the old reference until a fresh stable recovery hold qualifies, then adopts that pose without drawing. Same-origin recovery remains 600ms; a changed/opposed reference uses the existing 1200ms rebase dwell. Ready/Turning/Inverted and their thresholds are unchanged. Subsequent drawing still requires a whole opposition/return cycle.

**Separate logging correction retained:** v3-0.1.4 diagnostic activation left HWCDC timeout at zero. Actual device backtrace proved the main loop stalled inside a normal heartbeat write when the reader closed. v3-0.1.5 and later use a positive finite 1ms TX budget at setup and diagnostic activation. Do not restore a zero timeout. Capacity-sized writes are bounded, not universally wait-free; large ordinary heartbeat lines may be truncated. Keep raw fragments and do not invent complete records.

## RAM recorder — actual target verification

The explicit REC/RECORD command now stores every actual poll attempt in 2304×40-byte records (92,160 bytes) allocated only in PSRAM, outside LVGL. No per-poll USB output/allocation occurs. Raw axes/STATUS/wait/error/clipping masks, phases, UI gates/reset causes and triggers are retained; invalid raw values never become valid samples. Default is OFF; recording freezes after 45seconds or capacity and remains available without a reader. DUMP returns bounded CRC/session/token-checked chunks and is retryable; malformed signed/overflow numeric commands cannot replace frozen evidence. No card IDs/text, participants, files or network calls are added.

Actual S3 validation passed current/prepared source hashes, target/slot/image checksum/hash, compatible bootloader/partitions, fresh original-board identity and original backups before application-only write/hash verification. Exact version/UI/real IMU read back. RECORD confirmed a nonzero fresh session/count, then the reader was closed 46seconds. Fetch/independent verification recovered all 1,969 records / 78,760 bytes with full CRC32 d799012b and no reset between record/fetch. All baseline rows were valid, eligible and Ready, with no clipping/read errors/resets; poll gaps 20–36ms. No human movement was requested: this does not establish a physical flip.

The earlier v3-0.1.7 recorder build used 640,967 program bytes/128,176 globals; app 641,120 bytes. Real LVGL ASan/UBSan, actual SDK positive-budget regression, 12,000-step published-driver/detector equivalence and 42 isolated Python checks passed. Review's unsigned-parser blocker was reproduced RED, corrected with strict checked decimal uint32 parsing, passed full tests and targeted independent review. Installed v1.6 artifacts remain unchanged.

Use the current client after freshly identifying the port:

```sh
python3 tools/v3_flight_recorder.py record --port "$PORT"
# READY gives SESSION; perform one uninterrupted movement within the 45-second window.
python3 tools/v3_flight_recorder.py fetch --port "$PORT" --session "$SESSION" --wait 60 --output PRIVATE_NEW_CAPTURE.json
python3 tools/v3_flight_recorder.py verify PRIVATE_NEW_CAPTURE.json --jsonl PRIVATE_NEW_DECODED.jsonl
```

Use `--wait 60` (separate arguments) for the fetch timeout. Keep raw captures private and use new output paths. Reader may close; do not reset/power-cycle before retrieval. This is diagnostic RAM, not product history. Actual baseline and capture/export logs: `build/v3-0.1.7/evidence/`; The approved twelve-card integration is installed in v3-0.1.8 after merged tests/review/build/write/readback.

## Current merged target verification

v3-0.1.8 passed all four generator freshness checks, 45 isolated Python checks and actual LVGL/SDK/recorder/differential suites. All 12 main openings are present in the compiled app and standalone/prepared headers match. The full content surface covers 48 Ask/Try/main/Guide contexts and 179 complete-copy pages at unchanged fonts/geometry. Independent content and merged-integration reviews passed.

Actual S3 build 645,507 program bytes/128,184 globals; exported app 645,648 bytes. Target/slot/image checksum/hash/source/originals/fresh same-board and compatible component gates passed before app-only write/hash verification. Exact startup/UI/actual IMU readback passed.

An explicitly synthetic return-state injection on the actual 1.8 device selected tg-004; reading the live ROM pointer matched its approved question exactly. Actual raster/reveal timer, then released raster/settled-hold timer, then restored-control state completed on the same card. A full 1,904-record/76,160-byte CRC f09b5d32 export retained exactly one synthetic trigger and zero read errors/clips. This is a target execution test, not a human flip or visual-pixel acceptance. Evidence was exported before intentional cleanup reset; Home/index-1/compass timer were verified and readers/debuggers closed. Current package/evidence: `build/v3-0.1.8/`. Physical flip still needs a complete actual-motion capture.

## Additional target bench evidence

An explicitly synthetic detector-state injection on the actual v3-0.1.6 device bypassed the physical inversion stage, then normal actual-IMU/Ui::sample execution completed the return and selected one card. Readbacks showed a live PSRAM raster/reveal timer, then released raster and active settled-hold timer, then cleared focus/restored-control state on the same card. This establishes actual target execution of the draw/reveal/hold path, not a successful physical flip or visual pixel acceptance. An intentional cleanup reset returned Home/Ready. Private evidence: `build/v3-motion-investigation/target-draw-bench/target-result.private.json`.

The latest physical report remains three complete sequences without a card. An opt-in in-memory recorder is being prepared to retain all poll attempts and cancellation/clipping reasons without relying on live USB output; no further motion threshold change is justified yet. The approved twelve-card draft is now installed as v3-0.1.8; its public scopes remain draft.

## Implemented experience

- Home is only the full-face wandering eight-point compass, centered at (180,180), without instructions, labels or buttons. Decoration stays within radius 168. It never auto-draws or consumes card RNG; it is not magnetic north.
- A complete held turn-over/return draws Ask from Home. Tap the compass to open Options without drawing; Ask the Team / Try a Move / Instant text / Back remain available. Back returns to the plain compass. Safe-circle advanced hit-testing rejects off-safe taps; background contact still suppresses motion.
- The compass has its own 160ms timer/RNG and bounded old/new needle invalidation. Navigation/deletion/destruction cancel owned timers. Host frame samples are not device FPS.
- Twelve existing canonical draft pairs form independent shuffled mode bags. Exact endorsed Ask/cue/help wording is installed; twelve revisions advance once, IDs/titles/themes remain stable and all scopes stay draft. V2/browser pools remain eight, with no V2 hardware update.
- Each new card shows the unchanged whole blue answer face over six seconds, including its dark beat. Header/footer/controls stay hidden during reveal, then for five seconds after settlement. Tap during reveal completes the same card and starts the hold; tap during hold exposes controls. Drag does neither.
- The settled hold is not reveal-busy: another complete flip can draw the next shuffled current-mode card and cancel the old deadline. Instant text/capture fallback start the hold immediately; optional-timer allocation failure leaves controls accessible. Guide/Back stay instant and preserve identity; swipe/More text retain complete copy.
- Main text stays 20px, Guide 18px, controls at least 48px high. Main viewport stays 256x140 at (52,112). Plain compass has no text.
- Bubbles remain browser-preview-only. No saving, roster, Team Trail/history, accounts, live AI, cloud, networking/OTA, audio/microphone, RTC or SD is initialized. V3 updates use the independently identified USB interface.

## Gesture contract and physical check

Support the board without stressing its USB lead. Hold it screen-up and still, turn screen-down briefly, then return to approximately the same screen-up position and pause. Do not exchange messages between the two movements: the active cycle has a 12-second timeout. Leaving it inverted never repeats draws. Touch is sufficient without an IMU.

For the first controlled corrected-recovery check, tap the compass and choose Back to cancel any previous cycle, hold screen-up still for two seconds, turn screen-down for one second, then screen-up for two seconds. This is a test setup, not a new permanent operating ritual. Report whether a card appeared. Broader handling reliability remains separate.

The detector uses native unit gravity, not assumed axis/sign mapping or magnetic heading. Heading-only rotation is unobservable through gravity; this does not recognize every turn from every starting pose. Initial/same-origin rest 600ms, changed-reference recovery 1200ms, held pose 180ms with at least four fresh samples, cooldown 2500ms, maximum fresh gap 250ms, freshness 60ms and active cycle 12seconds remain experimental. Held poses require gravity-like magnitude and gyro below 12dps. Active cycles never rebase. Invalid/stale/clipped/nonfinite/duplicate frames or lost eligibility cannot accumulate dwell. Rearming itself never draws; a return alone is not a completed cycle after canceled-state reacquisition.

The fast application loop dispatches each healthy eligible sensor timestamp once; lost health/context cancels immediately even on a repeated timestamp. Options, Guide, reveal, pointer contact and inertial scrolling suppress gestures without queuing. A gesture never records use or saves anything.

## Rendering and diagnostics

The accepted `WindowReveal` is unchanged: one bounded 143,360-byte caller buffer uses PSRAM only, outside the 49,152-byte LVGL heap. No per-frame caller allocation is introduced. Missing capture capacity yields complete instant text, not successful animation. Browser studies, native host rendering, actual device timing and human acceptance remain separate.

Default-OFF `MOTION` starts a bounded 45-second raw-motion/state diagnostic session. It includes validity/age/gaps, native acceleration/gyro, UI gates, detector phase/reference/dwell and trigger/cumulative drop counters, not card IDs/copy or participant information. Fixed pending storage and capacity-sized partial writes preserve line integrity; old buffered records may precede a new session. Observation stops at timeout, while pending lines/summary may await a reader. Keep raw traces private. Do not treat an incomplete summary or synthetic replay as a successful physical cycle.

## Build and software evidence

Use pinned original-family archive/toolchain in [V3-BRINGUP.md](V3-BRINGUP.md). Preparation retains the same four guarded DMA substitutions and isolates the new output. Installed fallbacks and two full originals remain untouched.

```sh
umask 077
mkdir -p build/v3-0.1.8
python3 tools/prepare_v3.py > build/v3-0.1.8/preparation-output.json
ROOT="$PWD/build/v3-0.1.8"
FQBN='esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
FLAGS="-I$ROOT/config -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH=$ROOT/config/lv_conf.h"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$ROOT/libraries" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-path "$ROOT/compile-cache" --output-dir "$ROOT/firmware" \
  "$ROOT/prepared/trailguide_v3"
python3 tools/test_v3_app.py
```

Full actual LVGL 8.3.10 ASan/UBSan passes existing UI/model/IMU/gesture/input/scroll/reveal/lifetime checks and new recovery integration. Old detector fails the synthetic rearm assertion and private 315-row replay; corrected actual Ui/sample/Gesture rearms without drawing, followed by synthetic one-card full cycles and reveal-busy ignore/nonqueue checks. Existing false-positive assertions remain. The pinned verbatim SDK-write zero-budget negative control and positive-budget setup/activation tests pass. All 38 Python regressions passed in a fixture-seeded isolated copy; never run legacy preparers over immutable accepted builds. All 5,516 installed v3-0.1.5 files and exact inventory stayed byte-identical. Independent release review found no blockers.

Actual S3 build: 638,671 program bytes and 127,640 globals; exported app 638,816 bytes. Chip/slot/checksum/hash/current-prepared sources, matching bootloader/partitions, original backups and fresh same-board S3/16MB identity passed. Only app0 at 0x10000 was written and hash-verified. Exact version/UI/real IMU identity 0x6b/WHO05/revision7c/configuration and healthy first heartbeat were read back. Later oversized ordinary heartbeat fragments were not promoted to complete records.

With the serial reader closed, actual JTAG halt/read/resume snapshots across ten seconds showed compass ticks 400→463 and fresh sensor/UI timestamp 64623→74668ms, with motion available and phaseReady at both ends; TX budget remained1ms. No reset, flash or target-memory assignment was requested by that debugger. CPUs were resumed, all monitors closed. This is runtime progression evidence, not a completed physical flip or prolonged stress test.

Current private package/evidence: `build/v3-0.1.6/`; gesture capture:`build/v3-motion-investigation/coordinated-test/`; logging fault stack:`build/v3-motion-investigation/freeze-observation/`. Earlier v3-0.1.2 flip success, later failed trials and browser animation samples retain their separate scope. See [BRINGUP.md](BRINGUP.md).

Remaining: corrected physical flip/card-only timing acceptance, broader handling/edge mapping, quantitative frame time, active-use power/memory/PSRAM stress, and production storage/update features. Do not repeat already accepted compass appearance or the100-tap exercise as substitutes. Never apply C6 binaries/pins/partitions to V3. Source publication still awaits GitHub authentication renewal.
