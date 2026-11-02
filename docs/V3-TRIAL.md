# V3 — read-only round-device trial

Status: **v3-0.1.2 DRAFT installed and boot/UI/IMU verified**. Home now uses a full-screen playful compass and an experimental **turn-over-and-back** trigger instead of shake. The earlier physical answer animation was reported to work well and its renderer is unchanged. New compass appearance, physical flip reliability and remaining controls still require human acceptance. This is not production recording firmware or a validated coaching intervention.

## Implemented experience

- Home shows a large wandering eight-point compass rose indefinitely, without drawing or consuming card RNG. It is decorative, not magnetic north. The hint is **Turn over & back, or tap**.
- A complete held turn-over/return on the intro draws an Ask question. Tapping the compass is the alternative and draws once on release, not while held. A small **Options** control exposes **Ask the Team**, **Try a Move**, **Instant text**, and **Back**. Opening/closing Options does not draw; Back returns to the compass.
- Compass tap hit-testing is restricted to the radius-168 safe circle, even though the decorative object's rectangle extends beyond it. Off-safe decorative space cannot draw. Background contact still suppresses gesture detection.
- The compass timer is separate from reveal busy state and uses independent pseudo-random state, a 160ms cadence and bounded old/new needle dirty regions. Navigation, independent deletion, root teardown and owner destruction cancel it. Host timing is not device FPS.
- The same eight canonical paired drafts and independent mode bags remain. No card wording, editorial state, model or revision changed. The proposed twelve-card refresh is not included.
- The complete blue answer face and all its words approach together over six seconds, including the 600ms dark beat. **Reveal all** finishes the same card; **Guide me** and **Back** remain instant. **More text** and vertical swipe expose complete copy.
- **Instant text** bypasses the card reveal, not decorative compass motion. It is memory-only. Touch works without an IMU.
- Main text is 20px and Guide text 18px; the intro hint is 16px. Controls are at least 48px high and tested against round safe-area bounds. Main viewport remains 256x140 at (52,112).
- Bubbles are a separate browser preview, not installed firmware. No saving, roster, Team Trail, recording/history, accounts, live AI, cloud, Wi-Fi/OTA, audio/microphone, RTC or SD is initialized. Updates use the independently identified V3 USB interface.

## Turn-over-and-back: experimental, not physically accepted

Support the board securely without stressing its USB lead. Hold it **screen-up and still**, turn it **screen-down and pause briefly**, then return it to approximately the **same screen-up position and pause**. Only the completed cycle draws; leaving it inverted does not repeat draws. The intro defaults to Ask; an eligible settled card draws again in its current mode. Touch remains the alternative.

The detector captures a stable native-axis unit-gravity reference, recognizes held opposition, then held return. It does not assume an axis/sign mapping or a magnetic heading. Heading-only rotation is not observable through gravity; this is not recognition of every possible turn from every starting pose.

Defaults remain hypotheses: 600ms initial/rearm rest; 180ms pose dwell with at least four fresh samples; 2500ms cooldown; 250ms maximum sample gap; 60ms freshness; 12-second active-cycle timeout. Held poses require gravity-like magnitude and gyro below 12dps, allowing bounded uncalibrated bias. Turning does not inherit the old 45dps blanket veto. Invalid, stale, clipped, non-finite and duplicate frames cancel/rearm the detector; cooldown and reference survive context cancellation so the inverted pose is not silently learned as the next origin.

The application loop runs faster than the configured 20ms sensor poll. `Ui::sample()` dispatches each healthy eligible timestamp only once, while immediately canceling on lost health or context even when the timestamp repeats. A real integration regression failed before this guard and passed afterward.

Options, Guide, reveal, pointer contact anywhere and inertial scrolling suppress gestures without queuing. A gesture never records use, saves content or confirms anything. The earlier v3-0.1.1 gentle-shake test did not trigger a card; the user chose this different gesture rather than further shake tuning. No successful physical flip is inferred from host tests.

## Rendering and memory

The accepted `WindowReveal` implementation is unchanged. One bounded **143,360-byte caller buffer uses PSRAM only**, outside the unchanged 49,152-byte LVGL object heap. No per-frame application allocations are introduced. Missing capture capacity falls back to instant complete text, not successful motion; cache/buffer lifetime safeguards remain.

Browser blur/compositing, native host rendering, device performance and human appearance remain separate evidence categories.

## Build and checks

Use the pinned original-family archive and toolchain in [V3-BRINGUP.md](V3-BRINGUP.md). Preparation retains the same four guarded DMA substitutions and writes only within the new increment. Accepted v3-0.1.1 files and the original recovery images remain unchanged.

```sh
umask 077
mkdir -p build/v3-0.1.2
python3 tools/prepare_v3.py > build/v3-0.1.2/preparation-output.json
ROOT="$PWD/build/v3-0.1.2"
FQBN='esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
FLAGS="-I$ROOT/config -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH=$ROOT/config/lv_conf.h"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$ROOT/libraries" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-path "$ROOT/compile-cache" --output-dir "$ROOT/firmware" \
  "$ROOT/prepared/trailguide_v3"
python3 tools/test_v3_app.py
```

Actual bundled LVGL 8.3.10 ASan/UBSan checks pass: full compass/dirty budget/idle heap/RNG; real pointer hold, release, drag and off-safe rejection; Options navigation; complete copy/reveal/lifetime/fallback; integrated synthetic flip with repeated application-loop samples; context and inertia suppression. Caller allocations: 135, outstanding: zero, host LVGL peak: 14,752 bytes. Core synthetic tests cover native axes/signs, mixed cadence, slow turns, partial/held cycles, ordinary handling, cancellation, cooldown, invalid frames and rollover.

All **38 Python regressions passed in an isolated current-source copy** with the pinned ZIP, original comparison drivers and baseline manifest. Legacy preparation tests can recreate fallback directories: do not run them against an accepted recovery tree merely to collect duplicate evidence. The isolated run initially lacked comparison drivers; supplying the exact missing fixtures resolved that test-environment error.

Independent review blocked an off-safe compass tap region. Actual pointer RED reproduced it; advanced circular hit-testing passed GREEN and targeted re-review. The fixed S3 build reports **634,199 program bytes and 126,776 globals**; exported app is **634,352 bytes**. Static sizes are not measured free heap or FPS.

## Installed evidence and remaining checks

Actual chip/checksum/hash/slot, prepared source hashes, byte-identical bootloader/partitions, fresh original-target identity and both original 16MB backup hashes passed validation. Only app0 at `0x10000` was written and data-hash verified. No V1/V2 hardware was accessed.

Readback identifies v3-0.1.2, UI ready, expander/touch ACKs and actual IMU `0x6b` / WHO_AM_I `0x05` / revision `0x7c`, configuration readback passing. Six continuing heartbeats reported valid samples and zero read errors, with Home LVGL free space **42,808 bytes** and PSRAM free space **8,382,632 bytes** stable. No synthetic device inputs were sent. USB monitoring can reset the device; the monitor was closed for human testing, not described as reset-free.

Private package/source/runtime evidence: `build/v3-0.1.2/`. Previous Home idle evidence and positively observed answer animation remain historical evidence, not acceptance of the new compass/flip. See [BRINGUP.md](BRINGUP.md).

Next physical check: one screen-up → screen-down → screen-up cycle, then ordinary handling and Options/scrolling versus accidental draws. Quantitative frame time, active-use power/memory stress, full-screen input mapping, PSRAM health and production storage/update features remain separate. Do not repeat an already-accepted visual effect or 100-tap exercise as a substitute.

Never apply C6 binaries, pins or partitions to V3. Preserve earlier diagnostics and both full original-flash images for recovery.
