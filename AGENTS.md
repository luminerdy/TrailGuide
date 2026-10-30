# TrailGuide contributor and agent instructions

## Purpose

Develop a dependable offline pocket facilitator for AI workshops. Frontier Ops is a reusable visual and content theme, not a specific event. A development host builds and flashes firmware; the ESP32 runs independently.

## Start here

Read README.md, docs/PRODUCT-SPEC.md, docs/PLAN.md, docs/BRINGUP.md, docs/V1-SPEC.md and docs/WIFI-UPDATES.md before continuing. Read docs/V2-SPEC.md for touchscreen work. Recheck Git status, available tools, actual hardware and serial devices; prior observations are not live state.

## Hardware and architecture

- V1 uses classic ESP-WROOM-32, BOOT on GPIO0 and an SH1106 128x64 I2C OLED at SDA21/SCL22, powered at 3V3. The earlier SSD1306 assumption was incorrect.
- Disconnect power before wiring. Do not hold BOOT during normal reset or power-on.
- Identify the actual board and serial port before flashing; never guess a port.
- Preserve V1 as a fallback. V2 is a separate Waveshare ESP32-S3-Touch-LCD-1.85 target; inspect its exact revision before implementing support.
- Preserve compatible OTA partitions and authenticated local updates. Advice and Team Trail must work without internet access.

## Build and verification

Pinned V1 toolchain and build/upload commands are in docs/BRINGUP.md. Run both host suites from the repository root:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests/stubs tests/firmware_test.cpp -o "${TMPDIR:-/tmp}/trailguide-firmware-test"
"${TMPDIR:-/tmp}/trailguide-firmware-test"
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/ota_test.cpp -o "${TMPDIR:-/tmp}/trailguide-ota-test"
"${TMPDIR:-/tmp}/trailguide-ota-test"
```

Host tests cannot verify visible pixels, mechanical input, physical power behavior or flash durability. Distinguish planned, implemented, host-tested, compiled, flashed, serial-verified and human-observed results. Do not reset or reflash hardware during a continuous soak test.

## Privacy and publication

Keep repository documentation generic: no personal names, personal machine paths, chat/session details, specific event references or calendar dates. Frontier Ops may remain as a reusable theme. Technical versions, durations and generic workshop examples are appropriate. Use Git history for chronology.

Never publish actual network names, private addresses, passwords, keys, credentials, build binaries, private serial logs or real Team Trail data. Keep credentials in ignored local files and use placeholders in shared commands. Do not alter Git history or author identity without explicit authorization.

## Development and continuity

- Treat docs/PRODUCT-SPEC.md as canonical requirements, docs/PLAN.md as delivery sequencing and V1/V2 specs as hardware contracts.
- Update affected requirements and acceptance checks alongside implementation. Preserve stable requirement and content IDs.
- Record technical execution evidence and remaining acceptance in docs/BRINGUP.md without personal/session narratives.
- Inspect the current branch, configured origin and scoped diff before committing. Respect permission boundaries.
- Publish only intended changes and verify the exact remote target after pushing. Keep planned behavior separate from shipped functionality.
