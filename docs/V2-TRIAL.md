# TrailGuide V2 — first portrait hardware trial

Status: `v2-0.1.8 DRAFT`, compiled and wirelessly installed on C6. Expected running version, changed application slot and continuing OTA readiness/uptime were read back. The whole blue answer-face and complete words approach with gentle perspective/tilt/drift over six seconds, then settle still. No motion sensor is used. This remains a read-only trial; physical motion/allocation/frame rate, readability and touch acceptance remain open.

## Available now

- Home with **Ask the Team** and **Try a Move**. Reboot opens Home without drawing a card.
- Eight canonical draft pairs, one per theme, generated from `content/cards.json` without changing editorial states or existing card revisions.
- Main Ask question or Try action, visible theme/mode, one **Guide me** entry and **Another**.
- Fresh Ask/Try entry and Another use a 6000ms whole-surface emergence, including 600ms dark settling. The real native 20px type label and blue facets are captured together; fixed-point perspective moves the complete face/words rather than typing or fading only text. The final scrollable text matches the settled native raster. Reveal all finishes that same card; Guide/Back stay instant. No new shortcut/menu is added. The bounded 106,240-byte source/output buffer is outside the unchanged 48KiB LVGL object heap; failed capture/allocation safely shows instant complete text instead. This fallback is not proof that motion runs on the target.
- Guide me adds depth without repeating the main question/action. Under observation headings, body copy starts with the cue rather than repeating Listen for, Look for or Notice; the remaining wording and cautions are preserved. Ask starts with What to listen for, then You could ask. Try starts with How to run it, then What to notice and You could ask; a distinct If helpful, offer invitation is last and omitted when it duplicates the action/activity. Swipe vertically to read longer text; Back returns to the same card.
- Another cycles the eight cards in their fixed review order, without an adjacent repeat. This deliberately supports repeatable hardware review; it is not the future production shuffled/filterable pool.
- Portrait 172x320 layout, 20px main copy, 18px guidance, fixed 48–56px-high navigation buttons, and vertical overflow scrolling. Smaller 14/16px text is used for metadata and button captions; actual-device readability still requires human review.
- Offline operation without network configuration or a development-host connection after boot. Power still needs USB; independent-power and soak acceptance have not been completed.

No theme picker, linked-face shortcut, team roster, saving, recording or Team Trail is implemented in this trial. No disabled feature pretends to have succeeded. Authenticated local Wi-Fi/OTA is now implemented on the existing two-slot 8MB layout, with offline/reconnect and two wireless updates verified; see [V2-WIFI-UPDATES.md](V2-WIFI-UPDATES.md). This is not internet/cloud updating or a durable-data-retention test. All displayed content remains explicitly DRAFT review material.

## Hardware and recovery boundaries

The driver/pin configuration is based on the working C6 manufacturer example, not V1 or V3. Source is in `trailguide_v2/`; the compile-time target guard rejects a non-C6 build. V1 source, credentials, firmware and USB interface are unchanged. Factory full-flash backups and the working manufacturer demo binaries remain local/ignored; see [V2-BRINGUP.md](V2-BRINGUP.md).

The manufacturer's original panel initialization is prepared into an ignored build header from the checksum-verified archive. The public repository does not duplicate that vendor function. The manufacturer touch driver remains a local dependency; complete driver robustness and production input safeguards remain separate work.

## Build

Use Arduino CLI 1.3.1, Espressif core 3.3.2 and the manufacturer bundle's GFX 1.5.9 / LVGL 8.4.0 / AXS5106L library. Obtain the exact archive described in V2-BRINGUP.md and retain the extracted libraries under the ignored vendor directory. Run from the repository root:

```sh
python3 tools/build_v2_content.py
python3 tools/prepare_v2_panel.py
ROOT="$PWD/build/v2-0.1.8"
PANEL="$PWD/build/v2-0.1.0/generated"
SKETCH="$PWD/trailguide_v2"
LIBS="$PWD/build/v2-c6-bringup/vendor/Arduino/libraries"
FQBN='esp32:esp32:esp32c6:CDCOnBoot=cdc,FlashSize=8M,PartitionScheme=default_8MB,FlashMode=qio,FlashFreq=80'
FLAGS="-I$SKETCH -I$PANEL -DLV_CONF_INCLUDE_SIMPLE"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$LIBS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --output-dir "$ROOT/firmware" "$SKETCH"
```

The unchanged manufacturer panel header is reused. The project LVGL configuration enables snapshots and is supplied to C/C++; global libraries and V1 settings are unchanged. The v2-0.1.8 compiler reported 1,427,064 application bytes and 113,372 globals; the application image is 1,427,168 bytes. Static figures do not measure runtime heap. Its partition image is byte-identical to v2-0.1.7.

## Upload and diagnostics

Only after independently identifying the C6 port and closing monitors:

```sh
arduino-cli upload --fqbn "$FQBN" --port PORT \
  --input-dir "$ROOT/firmware" "$SKETCH"
```

The OTA-enabled bootstrap v2-0.1.2 used USB; subsequent updates are wireless. One authenticated transfer installed v2-0.1.8, switching0x340000 to 0x10000. Exact running version, OTA readiness and subsequent increasing uptime were read back. No C6 USB upload/reset was used. Earlier known-good applications and full original recovery images remain private; actual target heap/frame time and sustained power/network checks remain open.

At 115200 baud, uppercase T runs synthetic USB UI diagnostics. The diagnostic handles both successful animation allocation and instant fallback without accidentally advancing Another. Its actual function passed against host LVGL in both branches; it compiled but was not rerun over C6 USB during this OTA. Synthetic events are not physical touch or readability acceptance; keep raw logs private.

## Automated checks

```sh
python3 tools/build_v2_content.py --check
python3 -m unittest discover -s tests -p '*_test.py' -v
g++ -std=c++17 -Wall -Wextra -Werror tests/v2_model_test.cpp -o "$TMPDIR/trailguide-v2-model-test"
"$TMPDIR/trailguide-v2-model-test"
python3 tools/test_v2_ui.py
```

The last command runs real bundled LVGL 8.4 under ASan/UBSan. Coverage includes both modes/eight pairs, exact six-second poses, coherent face/word movement, native final handoff, full-copy scrolling/20px type, fixed controls, real input-driver hold/release/drag, same-card skip, instant navigation, allocation failure, rollover, OTA and independent teardown. It recorded 422 caller-buffer allocations with none remaining and 100-cycle heap stability. Thirty-four Python regressions passed. Native-frame images/video are host-rendered evidence, not device photographs or target frame-time measurements.

## Human trial

1. Confirm Home is readable, with both mode buttons fully visible.
2. Tap Ask the Team and confirm the blue face and words move into view over six seconds, not instantly. During another fresh draw, tap Reveal all: the same question should become complete immediately, and the button should return to Another. Another then changes the card once.
3. Open Guide me during a reveal: guidance should be complete immediately. Swipe to the final follow-up, then Back; the same main card should appear in full, without another animation.
4. Home → Try a Move → Guide me. Confirm the main action reveals gradually and the different activity-oriented guidance remains instant and complete.
5. Check longest questions, every card, edge taps and finger-held/swipe behavior. Report irritating pacing, unreadable text, clipping, unwanted activations or missed actions.
6. After interaction acceptance, verify restart, independent power, runtime memory and soak separately. Do not interrupt an agreed soak with uploads or resets.

Next: refine the portrait layout from hands-on feedback, complete sustained Wi-Fi/power checks and production eligibility/pools, then test durable recording before enabling Team Trail. See [V2-SPEC.md](V2-SPEC.md), [FACILITATOR-MODES.md](FACILITATOR-MODES.md) and [PLAN.md](PLAN.md).
