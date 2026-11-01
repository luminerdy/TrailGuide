# Development and verification record

Technical evidence only. Keep this document reusable: no personal identifiers, machine-specific home paths, chat narratives, calendar dates or references to a particular workshop. Frontier Ops remains a reusable theme. Versions and test durations describe technical behavior, not event scheduling.

## V2 guidance refinement — v2-0.1.1

- Hands-on review identified redundant repetition at the start of Guide me. Ask now opens with What to listen for and follow-ups, without repeating its main question. Try opens with How to run it, then What to notice and follow-ups; a distinct suggested invitation is last, and exact action/activity duplicates are omitted. Card title and Back context remain unchanged.
- Regression tests first failed for the missing progressive formatter, then passed across all eight pairs, including per-card generated-guide checks and duplicate-invitation suppression. All fourteen Python tests and the actual-LVGL ASan/UBSan navigation/scrolling suite passed. Canonical card text, IDs, revisions and review states are unchanged.
- v2-0.1.1 compiled with 615,004 application bytes and 80,612 globals. C6-only USB upload verified four image hashes. Serial startup confirmed v2-0.1.1, Home and 364,092 free-heap bytes; the synthetic on-device diagnostic passed and returned to Home. V1 and the two-slot partition layout were unchanged.
- Updated physical readability/touch acceptance and independent-power/soak results remain open. A disconnected USB interval was not treated as a failed upload or as battery acceptance. The older round browser artifact remains unchanged and needs its guidance-order alignment separately.

## Earlier V2 first read-only TrailGuide application

- Basic manufacturer-demo operation was human-reported as working before replacement. No detailed edge/soak acceptance is inferred from that report.
- v2-0.1.0 DRAFT is compiled, C6-only USB-flashed with verified hashes and serial-verified. It opens Home and provides Ask/Try, Guide me, vertical full-copy scrolling and ordered Another using eight canonical draft pairs. No editorial state/revision was changed.
- Actual LVGL host tests passed with ASan/UBSan across both modes, all eight cards and sixteen guidance faces, button bounds and scrolling. Pure model tests passed 1,000 advances without adjacent repeats and preserved card context across Guide/Back. New generator tests first failed for missing implementation, then passed. All thirteen Python tests, both V1 C++ suites and generated-content/round-preview freshness checks passed.
- The on-device synthetic diagnostic passed both modes, all eight cards/sixteen guidance faces, complete-copy comparisons, bounds, Back and Another, returning to Home. Startup measured 364,092 free-heap bytes; static compilation reported 615,100 application bytes and 80,612 globals. Neither is a sustained memory/soak test.
- Human TrailGuide readability, actual touch/drag/edge behavior, reset/independent power and soak remain open. Theme filtering, linked-face shortcuts, team selection, recording/saving, Team Trail and authenticated OTA remain unimplemented. This is not production release acceptance.
- Independent review of the full code diff found no blocking security or logic defects. Non-blocking follow-ups are stronger host-object cache invalidation and per-record generated-content assertions; existing real-LVGL, generator and on-device checks passed as described above.
- Source/build/review details: [V2-TRIAL.md](V2-TRIAL.md). V1 was untouched. Original backups and working demo binaries remain private/ignored; the 8MB two-slot layout is unchanged from the verified demo.

## Earlier V2 C6 first USB bring-up

- C6 silicon revision 0.2 and 8MB flash were probed. Secure boot and flash encryption were reported disabled. The original full flash was preserved in two matching 8,388,608-byte reads before any write.
- The correct manufacturer LVGL 8.4.0 display/touch example was built with Arduino core 3.3.2 and bundled GFX 1.5.9, uploaded only to C6, and hash-verified. A controlled C6 reset produced the example banner, LVGL version, touch-driver initialization data and `Setup done` over serial.
- Physical display/readability, touch mapping, exact PCB identification and runtime performance are not yet accepted. The temporary demo has no TrailGuide content, storage or OTA implementation; reserved OTA slots alone are not functional OTA.
- V1 was not reset, flashed or opened for serial monitoring during this work. Details, package-label caveat, source checksum, pins, partitions, reproducible build and private-image recovery procedure are in [V2-BRINGUP.md](V2-BRINGUP.md).
- This checkpoint supersedes the pending probe/backup/demo items in the earlier designation-only record below; it does not turn that earlier documentation update into a hardware test.

## Earlier target designation update

- V1 remains the classic ESP32/SH1106 working fallback.
- V2 is now the supplied ESP32-C6 1.47-inch 172x320 portrait capacitive-touch board with pin headers. V3 is the future round ESP32-S3-Touch-LCD-1.85 target.
- USB inspection sees an Espressif USB JTAG/serial interface at /dev/ttyACM0 and the original CP2102 interface at /dev/ttyUSB0. These are enumeration observations, not C6 chip/revision/flash probes or display/touch verification. Re-identify both before any upload; ports may change.
- The family description matches manufacturer documentation, but exact PCB/revision, drivers, flash/partitions and original-firmware recovery remain pending. No reset, serial probing or flashing was performed for this update.
- V2-SPEC.md now defines the portrait target; the prior round contract is preserved in V3-SPEC.md. Existing circular HTML/content/tests are unchanged and remain the V3/shared-flow preview. Earlier records using V2 for the round board are superseded by this designation.
- Documentation checks passed: generic wording, local links, eighteen stable requirement IDs and correct V2/V3 target mapping. The unchanged round preview passed its freshness check and all eleven existing Python tests passed. Source, content and device firmware are unchanged; this is not C6 build/display/touch acceptance.
- Next V2 work is safe identification/recovery preparation, correct manufacturer display/touch bring-up and a separate portrait UI adaptation. Do not shrink the circular coordinates or copy binaries across targets.

## Current checkpoint

- V1 hardware previously identified as ESP32-D0WD-V3 revision 3.1, 40 MHz crystal, 4 MB flash, CP2102 USB UART. Re-identify the actual board/port before flashing.
- Firmware v1-0.2.4 uses the actual Hosyond 1.3-inch SH1106 128x64 white OLED, SDA21/SCL22 at 3V3. The earlier SSD1306 driver assumption was incorrect.
- v1-0.2.4 was ESP32-compiled, USB-flashed and serial-verified: correct version, SH1106 at 0x3C and OTA readiness.
- Human-observed checks confirm readable sampled cards, one new card per BOOT press, no repeated advance while held, and fifty mechanical presses. Full-deck readability, thirty-minute soak result and independent-power acceptance remain unconfirmed.
- Host suites passed with C++17 and warnings as errors. These do not verify physical pixels or power behavior.
- The expanded content catalog is authored as forty draft cards across eight themes, with structured JSON and a readable catalog. Content tests pass; facilitator review and hardware layout checks remain pending.
- The first read-only V2 TrailGuide trial is now implemented; production theme-filter behavior, real team storage and Team Trail remain unimplemented. The round browser preview is retained for V3/shared design. Neither artifact stores interventions. V1 source and flashed firmware are unchanged. See [V2-TRIAL.md](V2-TRIAL.md), [PROTOTYPE-REVIEW.md](PROTOTYPE-REVIEW.md), [PRODUCT-SPEC.md](PRODUCT-SPEC.md) and [PLAN.md](PLAN.md).

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

## Round UI and two-mode specification refinement

Refined the documentation contract with a single Guide me entry, Asked this / Offered this move labels, the existing `question` field name, explicit proposed draft/reviewed/retired scopes and eligibility on draws/linked faces/saves/history reuse. Recorded historical snapshots stay distinguishable from eligible content for new use.

Added ROUND-UI-SPEC.md with proposed circle-safe coordinates, typography/touch budgets, three-choice paged menus, overflow behavior, explicit return context and an eight-card read-only prototype plan. All thirteen proposed rectangle envelopes were mathematically checked inside a radius-168 circle on an assumed 360x360 canvas. This is geometry evidence only: no font rendering, touch accuracy, readability, runtime performance or hardware clearance was established. Proposed thresholds and dimensions remain review inputs.

A separate read-only UX critique supported short pages, one help entry, mode-specific recording and eight representative pairs. It also prompted explicit Home-without-auto-draw behavior and a circular input mask, not just visual clipping. The critique did not run geometry or hardware tests. Independent geometry checking and existing-suite results are reported separately.

Final documentation checks passed: thirteen circle-contained rectangle envelopes, seventeen unique requirement IDs, parseable illustrative JSON, relative links, generic public wording and documentation-only scope. Both Python content tests and both rebuilt C++17 suites passed. None of these checks establishes actual font rendering, touch accuracy or new runtime behavior.

During the preceding specification-only step, no content JSON, firmware or storage schema was changed and no prototype was built. The subsequent prototype build is recorded below. No device was reset/flashed; full catalog enrichment, production eligibility/pools and mode-aware storage still follow facilitator review.

## Initial read-only prototype build and QA

Eight representative cards now carry additive draft Ask/Notice/Follow-up and Try invitation/Notice/Follow-up fields. Each affected revision advanced once; base wording, all forty stable IDs, eight themes and existing agent variants are unchanged. The content test first failed for missing paired fields, then passed after authoring and catalog synchronization. All scopes remain draft; structural validation is not editorial approval.

A portable prototype/template and standard-library builder now exist. The initial parent verification ran eight Python tests, including actual Chromium navigation, canonical-copy fit and pointer safeguards; they passed. Both existing C++17 suites also passed, and V1 firmware remained unchanged. The parent separately opened the actual file in a browser and exercised Ask, Guide me, paging, Back and disabled tracking controls with real pointer clicks. This is browser evidence only, not facilitator or hardware acceptance.

The initial review found three regression issues: reopening Team inside its picker overwrote return context; a 320-pixel viewport clipped the left of the fixed-size circle; duplicate theme metadata passed the unique-theme count check. A separate fix context added failing regressions and corrected all three. The parent reran all eleven Python tests successfully and independently rechecked double-open/Back and actual 320-pixel layout in the browser: card/mode/draw count were preserved and the circle started at x=0 in a horizontally scrollable region while remaining 360 pixels wide. Browser-helper screenshots were intermittently unavailable, but that does not replace functional checks or justify claiming a screen was inspected when it was not.

The independent full-code review found no security concerns and one blocking outside-and-back mouse-drag defect. A separate fix added a real Chromium CDP regression: it failed before the fix with draw count 2 instead of 1, then passed after leaving the dial canceled both pending press/release. Normal mouse taps and Enter-key activation also passed. The parent reran all eleven Python tests successfully after this correction. A separate narrowly scoped re-review approved the pointerleave fix with no security or logic findings; it did not independently rerun tests. The parent also reran the builder freshness check and both C++17 suites. Arbitrary custom theme-label width remains a review consideration; the canonical labels are covered by browser fit checks.

Implementation adjustments for this review artifact: compact menu context sits at y=111 rather than the initial y=116; the full title has a separate Guide page; overflow uses an explicit Full text: Guide me route rather than scroll-only access. Review these choices with the facilitator before treating them as final interaction decisions. Recording, saving and history remain unavailable, and synthetic fixtures are not real cards or team records.

## Documentation validation

The product specification defines stable requirement IDs; the current document includes P-01 through P-18. Related document links and whitespace checks passed when the specification was added, alongside both rebuilt host suites. Planned V2 behavior is not verified by those V1 tests. Keep generic public docs separate from private operational context and preserve Git history unless a rewrite is explicitly authorized.

## Two-mode facilitation specification

Added FACILITATOR-MODES.md with Ask the Team / Try a Move, core Guide me support, paired-content authoring, navigation rules, mode-aware saves/use events, legacy migration, acceptance checks and a development-host implementation sequence. Added stable requirement P-16 and reconciled home navigation, core scope, delivery sequencing and V2 requirements. README links to the handoff.

GitHub readback verified the exact published contents of the handoff, product spec, plan, V2 spec and README. This is documentation verification only. No firmware, content JSON, test code or device state was changed; no host suites, compile, upload or hardware checks were run for this documentation update. Paired facilitator content, prototype, mode-aware storage and V2 implementation remain pending.
