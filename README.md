# TrailGuide

[Project specification](docs/PROJECT-SPEC.md) · [Classic flow](docs/CLASSIC-FLOW.md) · [Coaching flow](docs/COACHING-FLOW.md)

## The product vision

**Make something meaningful. Learn how to keep going.**

TrailGuide combines **project-led mentoring, reusable coaching materials and an optional playful device**. Its purpose is to help mentors support individuals and teams across different uses of AI—and help learners continue without depending on that mentor.

Start with something a learner wants to **make, investigate or improve**. Find the next useful piece, choose how AI can help, try it, inspect what happened and adjust. The finished project need not contain AI: researching, planning, creating, analyzing, coding, debugging and collaborating can all be part of learning through building.

Build on the [AI Learning Lab](https://github.com/luminerdy/AILearningLab) loop **Define → Direct → Check → Adjust** and its flexible AI-role map, not a mandatory ladder or a competing curriculum. Carry the context and preferred help style into the next conversation with a [learner-owned project/coaching brief](docs/PROJECT-COACH.md).

### A glimpse of mentoring

- **A project needs a starting point.** Ask: "What is the next useful piece?" Choose whether an explanation, brainstorming partner, researcher, builder or another approach would help.
- **Something now works.** Ask: "What happened, and what do you understand differently?" Keep one next step and enough context to continue independently.
- **A team needs a move.** Offer one question or small experiment. People can adapt it, choose something else, observe or pass.

Checking claims, respecting privacy and choosing an appropriate level of AI help belong throughout this work. They are not the whole curriculum. More AI use, faster output or confidence scores are not the goal.

### The optional device: a little curiosity in your hand

Tap **Ask the Team** for a question or **Try a Move** for an activity; **Guide me** supports listening and follow-up. Eight themes offer exploration, clarification, experiments, verification, reflection, getting unstuck, inclusion and decisions. Optional agent-workflow material covers Codex and Claude Code. Frontier Ops remains a reusable theme, and curated advice works offline without live AI.

For **V3's playful experience**, **turn over and back for an idea, then watch the whole answer emerge together**. Randomness offers a possible next move, not a command or a diagnosis. Touch can do the same job; Reveal all and Instant text keep the playful pause optional. Planned **Team Trail** remembers deliberately recorded questions/moves—not inferred learning or screen views.

The device is a fun building project and possible shared learning aid, not a prerequisite for mentoring, a container for every material or evidence of better learning. Try the separate [V3 shake/reveal browser study](prototype/v3-experience.html); its shake is simulated, not sensor-tested.

## Development status

The pitch above describes the intended direction, not a production release or integrated AI-coaching service. The optional device Coaching trial is described separately below. Draft [mentoring practice](docs/COACHING-DIRECTION.md) and a [portable AI-coaching starter](docs/PROJECT-COACH.md) can be reviewed without hardware; they are not a validated curriculum or an integrated AI service. The separate V3 experience study explores shuffled curated ideas and gradual reveal in a browser only.

**V1 is the hardware starting point and fallback**, with twelve OLED/button cards. V2 retains its installed v2-0.1.8 portrait trial. Current [V3 v3-0.1.18-coaching-compass](docs/V3-TRIAL.md) preserves Classic as the boot default and adds the Coaching compass entry. Selecting **Coaching** now enters a wordless decorative compass with the full gold/blue arrow and six labelled path icons. The model silently selects or retains its opener; entry does not show the question. The invisible **136x64 center** opens Options. Decorative wandering uses an independent LCG and never selects a path or consumes a card draw. **Another opener** or a qualified turn over and back on Opening displays the retained blue-card reveal (six seconds plus five seconds of reading hold). **Opening** from a path returns to the compass without changing the saved opener; Options **Back** restores the compass or opener-card variant it came from.

**Sound OFF/ON** is available in both modes' Options, shared within the boot and reset OFF on reboot. Boot and toggles are silent. With Sound ON, an accepted new card/opener can chime when readable (normally at six-second settlement); the additional five-second hold is unchanged. Initial Coaching entry, restoration and navigation stay silent; busy cues are not queued, and OFF cancels active playback without OFF→ON reviving it. The 240ms 500Hz decaying cue uses nominal PCM peak 1536. There is no microphone, storage, networking or OTA added.

The compiled S3 application is **689568 bytes**. Four candidate native suites and two independent frozen-baseline suites pass strict compilation and actual LVGL ASan/UBSan checks. The approved Coaching Home and six selected path frames match exactly; all 68 Coaching and 110 Classic comparison frames match the frozen baseline. These are recorded software/build results, not new tests run for this documentation or physical acceptance. The integrated candidate also passes the complete canonical native suite and all 84 isolated Python checks without skips; 512 real native-rendered frames were captured and hash-verified. The forty-card catalog, twelve-card Classic pool and 55 Coaching entries remain unchanged. The quieter button appearance was physically approved on the earlier checkpoint and is preserved. The new Coaching Home preview is approved; its on-device appearance/touch check is still pending. Sound menu/mute/chime, narrower-face readability, broader touch/natural-opener behavior and sustained reliability retain separate gates. The original physical flip/blank failure remains unresolved: touch recovery was reported on the earlier checkpoint, not proof of a gesture fix. No tracking, saving, live AI or V3 OTA is implemented.

| Version | Hardware | Interaction | Status |
| --- | --- | --- | --- |
| V1 | HiLetgo ESP-WROOM-32 + Hosyond SH1106 128x64 white OLED | Onboard BOOT button | v1-0.2.4 display and BOOT confirmed; full acceptance pending |
| V2 | ESP32-C6 1.47-inch capacitive touchscreen, 172x320 portrait, pin headers | Touch-first, portrait layout | v2-0.1.8 read-only trial; tap-triggered gradual reveal; authenticated Wi-Fi updates verified; full human acceptance pending |
| V3 | Waveshare ESP32-S3-Touch-LCD-1.85 Rev2.1, 360x360 round | Classic compass/cards plus optional compact six-path icon coaching; unchanged turn-over/back reveal | v3-0.1.18-coaching-compass DRAFT installed; Coaching compass entry, preserved Classic and default-OFF peak-1536 chime; on-device Home/touch and other physical gates pending |

## Mentoring first; hardware alongside

Develop the mentor's listening, invitations, useful help and reflection alongside the learner's project. Use the next real milestone to decide what to research or build. Offer accessible alternatives, passing and a non-AI route; independence includes knowing when to seek human help. Research and human review remain necessary before claiming effectiveness. See the [mentoring direction](docs/COACHING-DIRECTION.md) and [primary delivery track](docs/PLAN.md#primary-track--project-led-mentoring-and-independence-p-19).

## Start here

- [Project specification — current scope, architecture and acceptance](docs/PROJECT-SPEC.md)
- [Classic — Mermaid prompt-deck flow](docs/CLASSIC-FLOW.md)
- [Coaching — Mermaid conversation-guide flow](docs/COACHING-FLOW.md)
- [Current project and coaching handoff](docs/HANDOFF.md)
- [Project-led mentoring direction, examples and review gates](docs/COACHING-DIRECTION.md)
- [Compass coaching — browser-only preview, design and checks](docs/COMPASS-COACHING-PREVIEW.md)
- [Downloadable six-path compass coaching preview](prototype/compass-coaching.html)
- [Learner-owned project brief and optional AI-coaching starter](docs/PROJECT-COACH.md)
- [V3 shake/reveal experience study and hardware boundaries](docs/V3-EXPERIENCE.md)
- [Downloadable simulated shake/reveal study](prototype/v3-experience.html)

- [Two-mode facilitator coaching — Ask the Team / Try a Move](docs/FACILITATOR-MODES.md)
- [Downloadable read-only prototype source](prototype/index.html)
- [Prototype review walkthrough](docs/PROTOTYPE-REVIEW.md)
- [Round-screen UX/UI specification and prototype plan](docs/ROUND-UI-SPEC.md)

- [Card catalog — forty draft cards across eight themes](docs/CARD-CATALOG.md)
- [Agent Workflow — Codex and Claude Code facilitation](docs/AGENT-WORKFLOW.md)
- [Structured card content](content/cards.json)
- [Living product specification — mentoring purpose and optional device requirements](docs/PRODUCT-SPEC.md)
- [Delivery plan](docs/PLAN.md)
- [V1 specifications](docs/V1-SPEC.md)
- [V2 portrait ESP32-C6 specifications](docs/V2-SPEC.md)
- [V2 wireless updates — setup, verified tests and limits](docs/V2-WIFI-UPDATES.md)
- [Try TrailGuide V2 — portrait firmware, build and review](docs/V2-TRIAL.md)
- [V2 display/touch bring-up and original-image recovery](docs/V2-BRINGUP.md)
- [V3 round ESP32-S3 specifications](docs/V3-SPEC.md)
- [V3 initial identification, original-image backup and host-build evidence](docs/V3-BRINGUP.md)
- [Raspberry Pi handoff](docs/PI-HANDOFF.md)
- [Wi-Fi update requirements](docs/WIFI-UPDATES.md)
- [Current Pi development results](docs/BRINGUP.md)
- [V1 firmware](oled_oracle/oled_oracle.ino)

The repository includes a [forty-card draft catalog](docs/CARD-CATALOG.md), five cards in each of eight themes. Twelve existing IDs now have paired draft guidance and form the V3 set; see the [exact openings and scoped revisions](docs/V3-CARD-CONTENT.md). The other twenty-eight entries remain unchanged and unpaired. The earlier browser/V2 source selections stay eight; V2 content generation is refreshed for future builds only, not installed. V1 retains its original twelve OLED cards.

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
