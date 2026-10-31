# Development and verification record

Technical evidence only. Keep this document reusable: no personal identifiers, machine-specific home paths, chat narratives, calendar dates or references to a particular workshop. Frontier Ops remains a reusable theme. Versions and test durations describe technical behavior, not event scheduling.

## Current checkpoint

- V1 hardware previously identified as ESP32-D0WD-V3 revision 3.1, 40 MHz crystal, 4 MB flash, CP2102 USB UART. Re-identify the actual board/port before flashing.
- Firmware v1-0.2.4 uses the actual Hosyond 1.3-inch SH1106 128x64 white OLED, SDA21/SCL22 at 3V3. The earlier SSD1306 driver assumption was incorrect.
- v1-0.2.4 was ESP32-compiled, USB-flashed and serial-verified: correct version, SH1106 at 0x3C and OTA readiness.
- Human-observed checks confirm readable sampled cards, one new card per BOOT press, no repeated advance while held, and fifty mechanical presses. Full-deck readability, thirty-minute soak result and independent-power acceptance remain unconfirmed.
- Host suites passed with C++17 and warnings as errors. These do not verify physical pixels or power behavior.
- The expanded content catalog is authored as forty draft cards across eight themes, with structured JSON and a readable catalog. Content tests pass; facilitator review and hardware layout checks remain pending.
- V2 touchscreen firmware, theme-filter runtime behavior, team storage and Team Trail remain planned, not implemented. V1 source and flashed firmware are unchanged. See [PRODUCT-SPEC.md](PRODUCT-SPEC.md) and [PLAN.md](PLAN.md).

## Verified V1 toolchain

- Arduino CLI 1.3.1.
- Espressif Arduino core 3.3.2.
- Adafruit SH110X 2.1.14, GFX 1.13.0 and BusIO 1.17.4.
- Python esptool 4.11.0.
- Target: esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default.
- Default 4 MB partition layout includes OTA metadata and two 0x140000-byte application slots at 0x10000 and 0x150000.

Install BusIO, GFX and SH110X in dependency order with pinned versions and --no-deps to avoid unintentional upgrades. A slow core download previously required `arduino-cli config set network.connection_timeout 600s`.

## Build and upload

Run from the repository root. Replace PORT with the actual detected serial port. These commands describe the procedure; rerunning them requires checking hardware and preserving compatible credentials/partitions.

```sh
arduino-cli board list
arduino-cli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --output-dir build/v1-0.2.4-sh1106 oled_oracle
arduino-cli upload --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --port PORT --input-dir build/v1-0.2.4-sh1106 oled_oracle
python3 tools/ota_upload.py ESP32_ADDRESS build/v1-0.2.4-sh1106/oled_oracle.ino.bin
```

The verified SH1106 build used 995,259 application bytes and 50,932 bytes globals. Compiled outputs contain credentials and must remain local/ignored. Wi-Fi/OTA secrets belong only in ignored, mode-600 oled_oracle/secrets.h. See [WIFI-UPDATES.md](WIFI-UPDATES.md).

## Host verification

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests/stubs tests/firmware_test.cpp -o "${TMPDIR:-/tmp}/trailguide-firmware-test"
"${TMPDIR:-/tmp}/trailguide-firmware-test"
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/ota_test.cpp -o "${TMPDIR:-/tmp}/trailguide-ota-test"
"${TMPDIR:-/tmp}/trailguide-ota-test"
```

Passing coverage: missing OLED/allocation failure, both display addresses, 1,000 deck cycles, debounce/hold/rollover, layout limits, offline advice, network retries, OTA listener reconnect and callbacks. The SH1106 regression assertion failed against the earlier SSD1306 implementation and passed after correction.

## Hardware verification evidence

- SH1106 USB upload hashes verified; serial startup confirmed v1-0.2.4, display address and OTA readiness. Human display/button observations are summarized above.
- Earlier serial-only builds verified fifty electrical GPIO0 cycles through the UART bridge with exactly fifty cards, no adjacent repeats or resets, and one draw while held. Electrical cycles are separate from mechanical-button checks.
- Wrong OTA password was rejected while normal firmware remained usable.
- Two consecutive direct-IP wireless updates succeeded from v1-0.2.1 through v1-0.2.2 to v1-0.2.3, with serial version/readiness confirmation.
- A sender-interrupted authenticated transfer at 10% reported OTA error 4. Normal firmware remained bootable and a later upload succeeded. This verifies interrupted-transfer recovery, not automatic rollback or power-loss safety.
- A temporary bench build disconnected Wi-Fi once after 20 seconds. Advice continued offline; production retry restored Wi-Fi/OTA. Normal v1-0.2.3 was restored afterward, before the SH1106 correction.
- OTA code/credentials are unchanged in v1-0.2.4. Prior OTA tests are not a claim of newly repeated OTA tests on every build.
- USB errors and an initially garbled serial read were observed; causes were not established. A controlled RTS reset with DTR deasserted produced readable 115200 8N1 startup. I2C acknowledgement alone does not verify visible pixels.

## Recovery

Keep local ignored build directories and source snapshots. Known SH1106 build: build/v1-0.2.4-sh1106. Earlier verified normal build: build/v1-0.2.3; it predates the corrected display driver.

Previously recorded application SHA256 for build/v1-0.2.3:
`3564e24ea0741ed7df65ff36fac86f9a61b56465bd5ee1afb78da1eceb43543c`.

Original flash backup: build/recovery/pre-trailguide-4mb.bin. Previously recorded SHA256:
`d911e73b462b36998a21300ed3be4423e6cf3fc7ba0052f400828e9712bfbc5e`.

Use USB recovery with the known build's original partition settings; prefer the SH1106 build for this OLED. Do not send merged images or partition-table changes over OTA. Private verification logs remain under ignored build/verification/ and are not repository documentation.

## Remaining acceptance

- Confirm every card's readability, independently of click count.
- Obtain the thirty-minute continuous-operation result; do not infer it from a timer/reminder.
- Verify reset, power-cycle, USB reconnect and USB-independent power operation.
- Router/AP disappearance and return, mDNS upload, and power loss during transfer remain untested. Direct-IP OTA is verified; no automatic rollback is claimed.
- Inspect V2 hardware before implementing its drivers or persistence layer.

## Content expansion validation

The content contract test was run before the expanded catalog existed and failed for the expected missing-catalog condition. After adding structured JSON and its readable Markdown view, it passed: forty unique card IDs/titles, eight themes with five cards each, required guidance fields, recognized situation tags, and verbatim preservation of the twelve original V1 actions. Both C++17 host suites were rebuilt with warnings as errors and passed. No firmware source was changed and no device was reset or flashed. All forty entries remain editorial drafts, not facilitator-approved or hardware-layout-verified content.

## Agent workflow content validation

Ten existing cards now carry draft CLI-agent variants with bounded prompts, human checks and separate Codex/Claude Code notes. The base actions, stable IDs, eight themes and forty-card count are unchanged. The ten adapted entries advanced to revision 2. A cross-theme collection and readable guide are authored; collection UI, variant tracking and tool-note display are not implemented.

The new content test first failed for the expected missing-collection condition, then passed after adding metadata and synchronized catalog/guide content. Official project-instruction documentation was inspected; guidance asks facilitators to verify installed versions, settings and active instructions rather than assuming identical tool behavior. No firmware was changed, reset or flashed.

## Documentation validation

The living product specification has fifteen stable P-IDs. Related document links and whitespace checks passed when the specification was added, alongside both rebuilt host suites. Planned V2 behavior is not verified by those V1 tests. Keep generic public docs separate from private operational context and preserve Git history unless a rewrite is explicitly authorized.

## Two-mode facilitation specification

Added FACILITATOR-MODES.md with Ask the Team / Try a Move, core Guide me support, paired-content authoring, navigation rules, mode-aware saves/use events, legacy migration, acceptance checks and a development-host implementation sequence. Added stable requirement P-16 and reconciled home navigation, core scope, delivery sequencing and V2 requirements. README links to the handoff.

GitHub readback verified the exact published contents of the handoff, product spec, plan, V2 spec and README. This is documentation verification only. No firmware, content JSON, test code or device state was changed; no host suites, compile, upload or hardware checks were run for this documentation update. Paired facilitator content, prototype, mode-aware storage and V2 implementation remain pending.
