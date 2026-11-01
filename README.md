# TrailGuide

## The product vision

**Help your team find its next move with AI.**

TrailGuide is a pocket touchscreen companion for people leading AI workshops. Ask a useful question, notice what the team is telling you, and offer a small activity when it helps. Spend less time searching your notes and more time with the people in front of you.

Tap **Ask the Team** for a question or **Try a Move** for an activity. **Guide me** helps you listen and follow up. **Team Trail** remembers which questions and moves you used with each team, so you can return to the conversation with context.

### A glimpse of the experience

- **An AI answer sounds convincing.** Ask: "Which part would matter most if it were wrong?" Offer a move: check one consequential claim against an original source.
- **The discussion needs more voices.** Ask: "What do we each notice before seeing someone else's answer?" Offer quiet thinking time, then invite sharing with passing welcome.

Eight themes cover exploration, clarification, experiments, verification, reflection, getting unstuck, inclusion and decisions. Optional guidance supports teams working with Codex and Claude Code. The reusable Frontier Ops theme gives the device its character; the guidance works offline, without a live AI connection.

## Development status

The pitch above describes the intended end product. **V1 is our starting point and fallback**, proving the basic advice-card experience on an ESP32 and OLED with twelve button-driven cards. V2 now runs an initial read-only portrait ESP32-C6 trial with Ask/Try, Guide me and eight draft pairs; it is running on C6 with remotely verified wireless updating, while full human interaction acceptance remains pending. A separate round browser preview is retained for future V3. Production touchscreen behavior, recording/saving and Team Trail remain unimplemented. The expanded forty-card catalog is drafted for review.

| Version | Hardware | Interaction | Status |
| --- | --- | --- | --- |
| V1 | HiLetgo ESP-WROOM-32 + Hosyond SH1106 128x64 white OLED | Onboard BOOT button | v1-0.2.4 display and BOOT confirmed; full acceptance pending |
| V2 | ESP32-C6 1.47-inch capacitive touchscreen, 172x320 portrait, pin headers | Touch-first, portrait layout | v2-0.1.4 read-only trial; authenticated Wi-Fi updates and reconnection verified; full human acceptance pending |
| V3 | Waveshare ESP32-S3-Touch-LCD-1.85, 360x360 round touchscreen | Touch, optional shake, round layout | Future target; existing round browser preview retained |

## Start here

- [Two-mode facilitator coaching — Ask the Team / Try a Move](docs/FACILITATOR-MODES.md)
- [Downloadable read-only prototype source](prototype/index.html)
- [Prototype review walkthrough](docs/PROTOTYPE-REVIEW.md)
- [Round-screen UX/UI specification and prototype plan](docs/ROUND-UI-SPEC.md)

- [Card catalog — forty draft cards across eight themes](docs/CARD-CATALOG.md)
- [Agent Workflow — Codex and Claude Code facilitation](docs/AGENT-WORKFLOW.md)
- [Structured card content](content/cards.json)
- [Living product specification — themes, touchscreen and Team Trail](docs/PRODUCT-SPEC.md)
- [Delivery plan](docs/PLAN.md)
- [V1 specifications](docs/V1-SPEC.md)
- [V2 portrait ESP32-C6 specifications](docs/V2-SPEC.md)
- [V2 wireless updates — setup, verified tests and limits](docs/V2-WIFI-UPDATES.md)
- [Try TrailGuide V2 — portrait firmware, build and review](docs/V2-TRIAL.md)
- [V2 display/touch bring-up and original-image recovery](docs/V2-BRINGUP.md)
- [V3 round ESP32-S3 specifications](docs/V3-SPEC.md)
- [Raspberry Pi handoff](docs/PI-HANDOFF.md)
- [Wi-Fi update requirements](docs/WIFI-UPDATES.md)
- [Current Pi development results](docs/BRINGUP.md)
- [V1 firmware](oled_oracle/oled_oracle.ino)

The repository also includes a [forty-card draft catalog](docs/CARD-CATALOG.md), five cards in each of eight themes, with practical activities and optional AI prompts. The read-only browser preview and initial V2 hardware trial use eight draft pairs; neither is production acceptance. The other thirty-two catalog entries do not yet have paired guidance or touchscreen integration. The working V1 firmware still contains its original twelve cards.

The Pi is the development computer for building and uploading firmware. The ESP32 runs independently after flashing. Start with USB power; battery and enclosure selection are deferred.

## V1 wiring

Disconnect USB before wiring. Follow printed OLED labels, not physical pin order.

| OLED | ESP32 |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO21 |
| SCL | GPIO22 |

Use a USB data cable. Press BOOT after startup to draw advice. Do not hold BOOT during reset/power-on during normal operation: GPIO0 selects download mode.

## Software

Arduino C++, Espressif ESP32 core, Adafruit SH110X 2.1.14, Adafruit GFX and dependencies. The actual OLED requires the SH1106 driver, not SSD1306. V1 target: ESP32 Dev Module (`esp32:esp32:esp32`). Serial diagnostics: 115200 baud. The sketch probes 0x3C and 0x3D.

For Arduino IDE, add https://espressif.github.io/arduino-esp32/package_esp32_index.json in Additional Boards Manager URLs, install esp32 by Espressif Systems, install the Adafruit libraries, and open the sketch. Select the detected port and ESP32 Dev Module, Verify, then Upload. If upload remains at Connecting, hold BOOT until writing begins, then release.

For Pi setup, let the agent inspect and install the toolchain once terminal access is available; see the handoff document. No credentials belong in this repo.

References: [Espressif I2C](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html), [Adafruit SH110X](https://github.com/adafruit/Adafruit_SH110X).
