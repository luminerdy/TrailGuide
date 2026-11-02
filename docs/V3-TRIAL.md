# V3 — first read-only round-device trial

Status: **v3-0.1.1 DRAFT installed**. Actual S3 compilation, independent review/re-review and application-only USB update passed. Startup version, continuing UI-ready heartbeats, actual IMU identity/configuration and fresh sample-valid state were read back. Physical animation is reported to work well. Instrumented reveal allocation/frame time, remaining new controls/overflow and real shake acceptance remain unverified. This is the first card application, distinct from the earlier `v3-bringup-*` display/touch and IMU diagnostics. It is not production firmware or a validated learning intervention.

## Implemented trial

- Home opens on a gently wandering idea compass, without drawing a card. It says **Find a direction**, **Playful, not north** and **Shake for an idea**. This is decorative motion, not a measured heading. Intro shake defaults to Ask; touch **Ask the Team** / **Try a Move** still selects a mode and draws. Home returns to the compass.
- Compass motion uses independent pseudo-random state and a separate timer: it neither consumes card RNG nor marks the card reveal busy. Navigation and compass/root/owner deletion cancel it. Instant text bypasses the card reveal, not the decorative compass motion.
- The same eight canonical paired drafts used by V2 are generated from `content/cards.json`, including current Guide formatting and cautions. No editorial states or revisions change.
- Each mode has its own shuffled eight-card bag. A cycle contains every card once; the last card of one cycle cannot be the first of the next. This is a bounded draft-review pool, not production eligibility/filtering for the forty-card catalog.
- A blue answer-bearing surface and all its words approach with gentle perspective/tilt/drift, then settle still. Total time is six seconds, including a 600ms dark beat. **Reveal all** finishes that same card; it does not draw another.
- **Guide me** adds depth; **Back** returns to the same card without another reveal. **More text** advances longer copy by a bounded scroll step and returns to the top at the end; vertical swipe also works.
- Home's **Instant text** toggle bypasses the effect. It is memory-only and resets on restart. Touch **Another** remains available without a working IMU.
- Main text is 20px and Guide text 18px. Text, controls and focus bounds are tested inside the round display's radius-168 safe area; controls are at least 48px high. Main viewport is 256x140 at (52,112).
- No saving, Team Trail, team roster, recording/history, accounts, live AI, cloud, Wi-Fi, authenticated OTA, audio, microphone, RTC or SD features are initialized. V3 updates currently require its separately identified USB interface.

## Experimental shake behavior

A settled main card can accept a deliberate multi-peak shake followed by rest. Shake and Another use the same draw path. Gestures are dropped—not queued—in Guide, during reveal, while a pointer is down anywhere, or until inertial scrolling ends. A gesture never records use or confirms anything.

The owned IMU wrapper preserves the previously checked identity/configuration/error/scaling path but polls at **20ms**, rather than the diagnostic firmware's 250ms. Actual WHO_AM_I and revision must still be read back after installation; the suffix and screen-axis mapping are not inferred from the board name.

Thresholds are **experimental hypotheses, not physically tuned acceptance**: initial 600ms quiet; at least three separated acceleration-magnitude deviations above 0.75g within 1200ms, rotation below 45dps, then 500ms quiet; 2500ms cooldown and new rest before rearming. Quiet permits less than 0.16g magnitude deviation and less than 8dps rotation, allowing the observed gyro bias. Stale, duplicate, invalid, clipped and non-finite samples are excluded. These constants can miss natural handling patterns; sensor presence and synthetic traces do not establish successful or reliable real shakes.

Support the board securely and avoid stressing its USB lead. Do not shake harder to overcome an unaccepted threshold; use Another and report the behavior for tuning.

## Native rendering and safe fallback

The whole face and complete words are captured together, then projected using bounded RGB565 buffers. One **143,360-byte caller buffer uses PSRAM only**, outside the unchanged 48KiB LVGL object heap. There are no per-frame application allocations. Missing PSRAM/capture capacity yields instant complete scrollable text—not successful animation. The image cache is invalidated before storage is released.

Native nearest sampling does not reproduce browser blur, soft shadows or subpixel compositing. A host-rendered simulation/video is not evidence of device FPS, physical readability, target allocation success or smoothness.

## Build and checks

Use the pinned original-family archive and existing toolchain described in [V3-BRINGUP.md](V3-BRINGUP.md). The owned preparation helper preserves the original archive and fallback outputs, derives only the same four reviewed DMA fixes, and writes exclusively under ignored `build/v3-0.1.1/`.

```sh
umask 077
mkdir -p build/v3-0.1.0
python3 tools/prepare_v3.py > build/v3-0.1.1/preparation-output.json
ROOT="$PWD/build/v3-0.1.0"
FQBN='esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
FLAGS="-I$ROOT/config -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH=$ROOT/config/lv_conf.h"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$ROOT/libraries" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-path "$ROOT/compile-cache" --output-dir "$ROOT/firmware" \
  "$ROOT/prepared/trailguide_v3"
python3 tools/test_v3_app.py
python3 -m unittest discover -s tests -p '*_test.py' -v
```

Current fixed-source compilation reports **632,967 application bytes and 126,736 globals**. Static sizes do not measure runtime heap. The exported application is 633,120 bytes. Actual S3 headers/checksums/hashes and slot fit passed; bootloader/partitions are byte-identical to the previous diagnostic image. Fresh target identity and both original 16MB backup hashes were checked before replacing only app0 at `0x10000`. Source/package/runtime evidence stays private under `build/v3-0.1.1/`.

Host checks use actual bundled LVGL 8.3.10 with ASan/UBSan, plus explicitly synthetic gesture/fake-bus inputs. Coverage includes both modes/eight cards, shuffled bags, complete copy/overflow, six-second poses and settled raster handoff, independent deletion, allocation failure, pointer holds/drags, background contact, inertial-scroll suppression and timing rollover. The actual extracted heartbeat is also executed against real LVGL before initialization: early startup failures must not touch an uninitialized TLSF heap. Thirty-eight Python regressions pass separately from these C++ suites.

## Running-device evidence and remaining acceptance

Compass v3-0.1.1 passed an independent scoped review and actual S3 build. Only app0 at `0x10000` was written and hash-verified after original-target/package/backup checks; bootloader and partitions are unchanged. Exact new version, UI ready, real IMU identity/configuration/fresh samples and six continuing heartbeats were read back. Home LVGL free space stayed 41,372 bytes and PSRAM free space 8,382,632 bytes over that capture. No synthetic device input was sent. Actual compass appearance and physical intro shake remain for human review. Host LVGL ASan/UBSan reported 134 caller allocations, zero outstanding and 14,792-byte peak in the unchanged 49,152-byte LVGL heap. This is not device FPS or reliable-shake acceptance.

Earlier v3-0.1.0 startup identified UI ready, expander/touch ACKs and IMU `0x6b` / WHO_AM_I `0x05` / revision `0x7c`, with configuration readback passing. Five continuing heartbeats reported valid sensor samples, zero sampling read errors, stable Home-state LVGL free space of 41,788 bytes and PSRAM free space of 8,382,632 bytes. This is Home/idle health and available memory, not an exercised on-target raster allocation or frame-rate test. No synthetic UI input or physical shake was injected.

Review caught and blocked an uninitialized-TLSF heartbeat path and incomplete background/inertial-scroll suppression. Actual failing regressions were observed before both fixes; the corrected startup harness and real input/gesture paths passed and were independently re-reviewed before deployment.

Physical observation: the v3-0.1.0 answer animation was reported to work well; its implementation is unchanged in v3-0.1.1. This is visual acceptance of that effect, not every control, a measured frame rate or shake acceptance. A separate Home/idle observation ran for 6h56m with 4,998 valid heartbeats, no observed restarts/crash markers/sampling errors, and constant LVGL/PSRAM free space. It ended on an intentional USB disconnect for standalone testing; it is not a full eight-hour pass or motion stress test. V2 had 417 successful status checks without communication errors or observed uptime restarts in that window.

Remaining checks:

1. Preserve the accepted physical reveal. Quantitative frame time and instrumented allocator/stress evidence remain separate from visual feedback.
2. Check remaining round-screen controls: Reveal all, More text, Guide/Back and alternate mode. No repeat 100-tap or already-accepted visual exercise is required.
3. On the compass intro and with a card settled, assess a gentle deliberate shake/rest versus ordinary handling, scrolling and button contact. Tune only from actual observations; do not treat synthetic events as physical input.
4. Keep visual feel, allocation/frame time, gesture reliability, full-screen mapping, PSRAM health and sustained power/runtime separate. Do not reset an agreed soak or infer acceptance from silence.

The earlier diagnostic firmware and both full original-flash backups remain the recovery boundary. Never apply a C6 binary, pin map or partition table to V3.
