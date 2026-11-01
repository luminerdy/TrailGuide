# TrailGuide V2 — first portrait hardware trial

Status: `v2-0.1.0 DRAFT`, compiled, USB-flashed and serial-verified on the C6. This is the first read-only TrailGuide application, replacing the manufacturer demo. It is not a production release. The manufacturer demo was reported working by a human; TrailGuide readability and physical interaction acceptance are separate and remain open.

## Available now

- Home with **Ask the Team** and **Try a Move**. Reboot opens Home without drawing a card.
- Eight canonical draft pairs, one per theme, generated from `content/cards.json` without changing editorial states or existing card revisions.
- Main Ask question or Try action, visible theme/mode, one **Guide me** entry and **Another**.
- Guide me shows complete mode-specific Say/Offer, Activity where applicable, Notice and Follow up text. Swipe vertically to read longer text; Back returns to the same card.
- Another cycles the eight cards in their fixed review order, without an adjacent repeat. This deliberately supports repeatable hardware review; it is not the future production shuffled/filterable pool.
- Portrait 172x320 layout, 20px main copy, 18px guidance, fixed 48–56px-high navigation buttons, and vertical overflow scrolling. Smaller 14/16px text is used for metadata and button captions; actual-device readability still requires human review.
- Offline operation without network configuration or a development-host connection after boot. Power still needs USB; independent-power and soak acceptance have not been completed.

No theme picker, linked-face shortcut, team roster, saving, recording, Team Trail, Wi-Fi or OTA listener is implemented in this trial. No disabled feature pretends to have succeeded. The existing two-slot 8MB partition layout is preserved, but authenticated updates are still a future acceptance gate. All displayed content remains explicitly DRAFT review material.

## Hardware and recovery boundaries

The driver/pin configuration is based on the working C6 manufacturer example, not V1 or V3. Source is in `trailguide_v2/`; the compile-time target guard rejects a non-C6 build. V1 source, credentials, firmware and USB interface are unchanged. Factory full-flash backups and the working manufacturer demo binaries remain local/ignored; see [V2-BRINGUP.md](V2-BRINGUP.md).

The manufacturer's original panel initialization is prepared into an ignored build header from the checksum-verified archive. The public repository does not duplicate that vendor function. The manufacturer touch driver remains a local dependency; complete driver robustness and production input safeguards remain separate work.

## Build

Use Arduino CLI 1.3.1, Espressif core 3.3.2 and the manufacturer bundle's GFX 1.5.9 / LVGL 8.4.0 / AXS5106L library. Obtain the exact archive described in V2-BRINGUP.md and retain the extracted libraries under the ignored vendor directory. Run from the repository root:

```sh
python3 tools/build_v2_content.py
python3 tools/prepare_v2_panel.py
ROOT="$PWD/build/v2-0.1.0"
SKETCH="$PWD/trailguide_v2"
LIBS="$PWD/build/v2-c6-bringup/vendor/Arduino/libraries"
FQBN='esp32:esp32:esp32c6:CDCOnBoot=cdc,FlashSize=8M,PartitionScheme=default_8MB,FlashMode=qio,FlashFreq=80'
FLAGS="-I$SKETCH -I$ROOT/generated -DLV_CONF_INCLUDE_SIMPLE"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$LIBS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --output-dir "$ROOT/firmware" "$SKETCH"
```

The project-specific LVGL configuration is used by both C and C++; global libraries and V1 settings are not modified. The compiler reported 615,100 application bytes and 80,612 bytes of globals. These static figures are not total runtime memory measurements.

## Upload and diagnostics

Only after independently identifying the C6 port and closing monitors:

```sh
arduino-cli upload --fqbn "$FQBN" --port PORT \
  --input-dir "$ROOT/firmware" "$SKETCH"
```

The executed upload verified hashes. A C6-only controlled reset produced `TrailGuide v2-0.1.0 DRAFT`, touch startup data, `READY: 172x320 Home; 8 DRAFT pairs; offline` and a measured startup free-heap value of 364,092 bytes. That single reading is not a leak, stress or soak test.

At 115200 baud, send uppercase `T` over USB to run synthetic UI diagnostics. The executed on-device diagnostic passed both modes, eight cards, sixteen guidance faces, full-text comparisons, button bounds, Back and Another, then returned to Home. This invokes software click events; it does not physically touch the panel or establish pixel readability. Keep raw logs private.

## Automated checks

```sh
python3 tools/build_v2_content.py --check
python3 -m unittest discover -s tests -p '*_test.py' -v
g++ -std=c++17 -Wall -Wextra -Werror tests/v2_model_test.cpp -o "$TMPDIR/trailguide-v2-model-test"
"$TMPDIR/trailguide-v2-model-test"
python3 tools/test_v2_ui.py
```

The last command builds the real bundled LVGL on the host with AddressSanitizer and UndefinedBehaviorSanitizer. It tests actual UI callbacks and layout for both modes, all eight pairs, full guidance scrolling and screen/button bounds. It does not replace physical touch/display acceptance. Continue running both V1 C++ suites as documented in BRINGUP.md.

## Human trial

1. Confirm Home is readable, with both mode buttons fully visible.
2. Ask the Team → Guide me → swipe to the final follow-up → Back → Another. Back should preserve the card; Another should change it once.
3. Home → Try a Move → Guide me. Confirm different activity-oriented guidance and complete text.
4. Check longest questions, every card, edge taps and finger-held/swipe behavior. Report unreadable text, clipping, unwanted activations or missed actions.
5. After interaction acceptance, verify restart, independent power, runtime memory and soak separately. Do not interrupt an agreed soak with uploads or resets.

Next: refine the portrait layout from hands-on feedback, implement production eligibility/pools and authenticated OTA, then test durable recording before enabling Team Trail. See [V2-SPEC.md](V2-SPEC.md), [FACILITATOR-MODES.md](FACILITATOR-MODES.md) and [PLAN.md](PLAN.md).
