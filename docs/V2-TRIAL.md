# TrailGuide V2 — first portrait hardware trial

Status: `v2-0.1.6 DRAFT`, compiled and wirelessly installed on the C6, with running version, changed application slot and continuing OTA readiness verified remotely. This increment adds touch-triggered gradual main-card reveal; it uses no motion sensor. The initial OTA bootstrap used USB. This remains a read-only trial, not a production release. Physical pacing, readability and touch acceptance remain separate and open.

## Available now

- Home with **Ask the Team** and **Try a Move**. Reboot opens Home without drawing a card.
- Eight canonical draft pairs, one per theme, generated from `content/cards.json` without changing editorial states or existing card revisions.
- Main Ask question or Try action, visible theme/mode, one **Guide me** entry and **Another**.
- Fresh Ask/Try entry and Another show a short settling beat, then gradually reveal the original main text. The nominal timing is 160ms settling plus 2800ms reveal with regularly serviced LVGL timers; real-device pacing still needs human review. During the effect, the existing Another button reads **Reveal all** and immediately completes that same card; it then returns to Another. Guide me stays available and instant; Back shows the same main card in full without restarting the effect. There is no main-text tap shortcut or new settings menu.
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
ROOT="$PWD/build/v2-0.1.6"
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

The unchanged manufacturer panel header is still prepared in the initial trial's generated directory and reused by this patch build. The project-specific LVGL configuration is used by both C and C++; global libraries and V1 settings are not modified. The v2-0.1.6 compiler reported 1,416,560 application bytes and 113,364 bytes of globals. These static figures are not total runtime memory measurements. Its partition image is byte-identical to the preceding build.

## Upload and diagnostics

Only after independently identifying the C6 port and closing monitors:

```sh
arduino-cli upload --fqbn "$FQBN" --port PORT \
  --input-dir "$ROOT/firmware" "$SKETCH"
```

The OTA-enabled bootstrap v2-0.1.2 was USB-flashed with verified hashes and then updated over the network to v2-0.1.3 and v2-0.1.4. Coaching-copy v2-0.1.5 and gradual-reveal v2-0.1.6 were also installed wirelessly. The latest update changed the active slot from 0x340000 to 0x10000; expected running version, OTA readiness and continuing runtime were read back. No USB upload or serial reset was used for this increment. Earlier free-heap readings from USB-only firmware are not substituted for a Wi-Fi runtime budget; sustained memory/soak checks remain open.

At 115200 baud, send uppercase `T` over USB to run synthetic UI diagnostics. An earlier on-device diagnostic passed both modes, eight cards, sixteen guides, full-copy comparisons, bounds and navigation. The revised diagnostic first activates Reveal all before comparing full main text; it compiled in v2-0.1.6 but was not rerun over USB during this wireless update. Synthetic events do not physically touch the panel or establish pixel readability. Keep raw logs private.

## Automated checks

```sh
python3 tools/build_v2_content.py --check
python3 -m unittest discover -s tests -p '*_test.py' -v
g++ -std=c++17 -Wall -Wextra -Werror tests/v2_model_test.cpp -o "$TMPDIR/trailguide-v2-model-test"
"$TMPDIR/trailguide-v2-model-test"
python3 tools/test_v2_ui.py
```

The last command builds the real bundled LVGL on the host with AddressSanitizer and UndefinedBehaviorSanitizer. It tests both modes/eight pairs, full-copy scrolling, button/caption bounds, settling/intermediate/completed reveal, same-card skip, instant Guide/Back, UTF-8 prefixes, cancellation, OTA restoration and teardown. The full Python suite passed twenty-eight tests; five V1/V2 C++ host suites also passed. These checks do not replace physical touch/display acceptance.

## Human trial

1. Confirm Home is readable, with both mode buttons fully visible.
2. Tap Ask the Team and watch the words appear. During another fresh draw, tap Reveal all: the same question should become complete immediately, and the button should return to Another. Another then changes the card once.
3. Open Guide me during a reveal: guidance should be complete immediately. Swipe to the final follow-up, then Back; the same main card should appear in full, without another animation.
4. Home → Try a Move → Guide me. Confirm the main action reveals gradually and the different activity-oriented guidance remains instant and complete.
5. Check longest questions, every card, edge taps and finger-held/swipe behavior. Report irritating pacing, unreadable text, clipping, unwanted activations or missed actions.
6. After interaction acceptance, verify restart, independent power, runtime memory and soak separately. Do not interrupt an agreed soak with uploads or resets.

Next: refine the portrait layout from hands-on feedback, complete sustained Wi-Fi/power checks and production eligibility/pools, then test durable recording before enabling Team Trail. See [V2-SPEC.md](V2-SPEC.md), [FACILITATOR-MODES.md](FACILITATOR-MODES.md) and [PLAN.md](PLAN.md).
