# V3 — read-only round-device trial

## Current installed checkpoint

**v3-0.1.14 DRAFT is installed, application-flash verified and exact-version startup/UI/real-IMU liveness checked.** Tap the classic compass, then **Coaching** for the approved narrower 192x112 blue face. Text, outward icons, spacing, text-only Options and the coaching flow are unchanged. Coaching **Options → Classic** returns to the original compass/cards. Physical feedback liked the preceding icons and spacing; physical acceptance of this side trim and broader coaching touch/natural-opener checks remain pending. Earlier 1.10/1.11 human gesture acceptance below is historical, not a new 1.14 acceptance claim.

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

## Narrower coaching blue face — v3-0.1.14

The approved native side-trim comparison is installed: 8px removed from each side, taking the visible blue face from 208x112 to 192x112. The 168px text wrap, 18px body, 12px captions, icon positions, generous hit targets and plain Options are unchanged. The exact 256x140 snapshot, six-second reveal and additional five-second hold remain intact. All six openers fit, all 55 canonical coaching faces and complete-copy overflow pass native checks, and the animated/live handoff remains exact. Full current-source LVGL ASan/UBSan and 80 isolated Python checks pass; source/output/deployment review, S3/package checks, app-only flash/explicit comparison and exact-version initialized-UI/real-IMU readback pass. Installed 1.6–1.13 inventories remain unchanged; this is presentation-only, not new physical gesture acceptance.

## Compact coaching presentation — v3-0.1.13

The approved smaller native layout and subsequent plain Options request are installed, app-flash verified and exact-version startup/real-IMU checked. The flow is unchanged. Main coaching text is 18px, perimeter captions and opening Options are 12px; icons move 4px outward and blue face is 208x112 inside the unchanged 256x140 snapshot panel. Tap targets remain at least 48px, including the 100x48 invisible Options area. All six canonical openers fit in native checks. Actual full native tests and 80 isolated checks pass; physical compact readability/touch remains pending. Classic, content, models, sensor/gesture and timing are preserved.

## Native six-path coaching — v3-0.1.12

See the [current coaching checkpoint and entry instructions](HANDOFF.md#previous-native-coaching-checkpoint--v3-0112) and [coaching content, interaction and browser boundaries](COMPASS-COACHING-PREVIEW.md). Classic remains the boot default; use its Options **Coaching** entry. This is an offline draft, not a recording release. No new shake detector or motion tuning is introduced.

## Qualified-turnover blackout — v3-0.1.11

The existing qualified Inverted milestone now hides the current compass/card to true full-frame black. It stays black during return qualification; only the completed return draws one new card and starts the unchanged 6000ms blue-face reveal and 5000ms settled hold. There is no shake, extra pause, recognition/range/dwell/rest/cooldown change, RNG use on the half-cycle, or rerender/reset just to hide the scene.

The existing root draws black while its existing direct children are HIDDEN, removing invisible controls from hit-testing. A 32-bit prior-visibility mask uses no new heap allocation, snapshot or timer. Owned layouts have at most 6 direct children; unsupported foreign roots over 32 decline optional blackout safely. Focus expiry updates the desired restored chrome without exposing children. Health/context/timeout cancellation restores prior identity; a real pointer press restores immediately and swallows that same contact's click. Navigation and deletion cancel safely. Arbitrary foreign child reordering/appending while black is unsupported. This does not make ordinary new draws or unrelated UI allocations globally allocation-free.

Actual native ASan/UBSan tests cover compass/card, both modes/bag boundaries, preconfirmation black, exactly-one completion, cancellation, repeated timestamps, focus expiry, real pointer input, exhausted LVGL heap and root/child/owner deletion. All 53 isolated Python and full native/content/SDK/differential checks pass, followed by independent 14-file review. Thirteen native 360x360 framebuffer captures and hashes were inspected; all five inversion/preconfirmation frames have every pixel black, and cancel-before/restored frame bytes match. These are host renderer observations, not device FPS or physical cancellation acceptance.

Actual S3 build 646,191 program bytes/128,192 globals; app 646,336 bytes. App-only write and explicit flash verification pass after original-board/security/16MB/backup/source/image/compatible component gates. Exact 1.11 ROM/UI/Home, clear blackout state, default-OFF recorder, live compass/sample counters and configured healthy IMU passed direct runtime/memory checks. A legacy MOTION streaming probe produced no usable rows and was not accepted as evidence; the independent exact-ELF checks passed without modifying recognition or sensor code.

The subsequent natural movement produced the requested visible sequence: blank, card animation, then controls. The 120-second recorder was manually frozen after 112.156seconds; 4940 actual records/197600 bytes passed full device CRC with exactly one trigger, zero clipped rows and no read errors. Read-only JTAG was used only after freeze and after the user had observed controls. Final actual state is the selected card with blackout, reveal and settled-focus flags clear; it was left running without another reset. Raw data and firmware/range provenance remain private. This confirms one observed visual sequence; earlier multiple Card/Compass successes refer to the preserved 1.10 gesture checkpoint, not a measured 1.11 reliability total.

## Wide-range correction and successful natural turnover — v3-0.1.10

The earlier complete failed capture showed gyro-X raw rails cancelling health and restarting recovery during both legs. One checked range selection now derives CTRL3 `0x77`, full scale +/-2048 dps, 16 LSB/dps conversion and 2044 dps safety bounds (the same 32704-count headroom fraction as before). Applicable pinned QMI8658A/C primary datasheets agree on this setting; installed suffix is not assumed. Raw endpoint/nonfinite/invalid rejection remains. Acceleration 4g, ODR enum 7, 20ms cadence, 12dps rest, dwell/cooldown/state/eligibility, model/RNG, cards, renderer and recorder behavior are unchanged. Only the gyro register/scaling and its two matching safety predicates changed, plus version/log identity.

All 53 isolated Python checks, complete native LVGL/content/SDK suite and independent 18-file review pass. RED/GREEN signed conversions, synthetic 600dps complete-cycle and actual UI integration tests pass; all-axis safety/nonfinite/raw-rail and previous cancellation/timing negatives remain. Differential tests cover 12000 equal-raw steps with narrowly permitted register/scaling differences and 12000 independently encoded equal-physical steps with matching state/predicates and 12 events. These are synthetic host checks, not the human recording.

Actual S3 compile 645,639 program bytes/128,184 globals; app 645,792 bytes. Fresh original-board/security/16MB/backup/partition/image/source checks, app-only write and explicit flash verification pass. Exact 1.10 ROM version/UI/Home, completed sensor configuration/readback, advancing compass/IMU counters and plausible stationary physical units pass. Installed 1.6/1.7/1.8/1.9 outputs remain immutable.

The subsequent natural turnover/back produced a card and exactly one trigger in 3610 actual poll records/144400 bytes, frozen at 83.068seconds. Full device CRC matches the retrieved bytes; zero clipped rows and no read errors. Driver/detector phases include Turning and Inverted before the trigger. No synthetic state or debugger was used during movement; read-only JTAG retrieved already-frozen RAM afterward. The firmware/range identity is preserved with this capture: capacity 8192 alone does not identify raw gyro scaling. Earlier clipped values were not reconstructed or rescaled as if they were new measurements.

This verifies one end-to-end natural gesture on the corrected setting, not sustained reliability, every touch path, timing/readability acceptance, power endurance or learning outcomes. The running card was left in place without another reset. Long serial dump reliability remains a separate instrumentation limitation; only complete CRC-validated retrieval is accepted. Keep the simple turn-over-and-back interaction; no shake combination is introduced.

## Two-minute recording checkpoint — v3-0.1.9 (prior)

REC is default OFF and now allocates/reuses 8192x40 bytes in PSRAM, freezes at 120seconds/capacity or `FREEZE <session> <token>`. Exact unsigned fields and matching nonzero session are required; retrying a matching frozen session preserves its ended time/reason/CRC/rows. Updated client supports legacy 2304/new 8192 capacities and manual reason 3; old unmodified clients reject 8192 rather than silently misread it. Legacy MOTION remains 45seconds. No sensor range, polling, detector thresholds/gates, card content, fonts or renderer changed in this release.

```sh
python3 tools/v3_flight_recorder.py record --port "$PORT"
# Use the actual READY session; complete the whole natural movement without messaging between poses.
python3 tools/v3_flight_recorder.py freeze --port "$PORT" --session "$SESSION"
python3 tools/v3_flight_recorder.py fetch --port "$PORT" --session "$SESSION" --wait 0 --output PRIVATE_NEW_CAPTURE.json
python3 tools/v3_flight_recorder.py verify PRIVATE_NEW_CAPTURE.json
```

Keep data private; export before reset and use new destinations. Serial long-transfer timeouts/interleaved heartbeat text were observed; never accept partial output. Parent target tests used the exact ELF to retrieve already-frozen RAM through read-only JTAG and verified full CRC/header stability. No debugger was attached during the later human movement.

Actual 53 isolated Python checks plus full native LVGL/content, SDK positive 1ms and driver/detector differential suites pass. Independent review passed after a RED/GREEN unsigned-metadata fix. Actual compile 645,639 program/128,184 globals; app 645,792 bytes. App-only write/hash and explicit flash verify passed after fresh original-board, security, backup/source/image/compatible component gates. Exact runtime 1.9/UI/Home/healthy IMU and advancing compass/sample counters pass. A synthetic target return proved larger recorder plus reveal allocation coexistence and normal reveal/hold/restoration; 3725 records/149000 bytes/manual reason 3/fullCRC contained exactly one synthetic trigger. Evidence was exported before intentional cleanup reset. These are not physical pixel or flip acceptance.

The subsequent full natural-turnover capture contains 3101 rows/124040 bytes across 70.862seconds, manual reason 3; whole CRC passes. Host FREEZE ACK round trip was 0.070seconds (includes protocol/CRC/USB, not isolated CRC CPU timing). Gyro-X hit its configured raw rails 8 times; invalid-health cancellation put the detector into Recover on both turn legs, with no trigger. Read-error count did not increase and touch gates did not block the cycle. Physical result remains no card. This identifies measurement saturation in this attempt; a wider supported range and matched scaling/clip guards must be tested separately. Do not treat clipped peaks as recoverable data or claim that the range change is already installed.

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
mkdir -p build/v3-0.1.11
python3 tools/prepare_v3.py > build/v3-0.1.11/preparation-output.json
ROOT="$PWD/build/v3-0.1.11"
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
