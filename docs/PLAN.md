# Delivery plan

Goal: dependable offline advice oracle before Frontier Ops, November 9–12, 2026. Preserve V1 as the fallback. The human connects hardware; the agent handles software once terminal access is available.

## 1. Pi handoff and toolchain
- Establish Pi network and SSH access.
- Inspect OS, architecture, disk space, tools, serial devices and permissions.
- Clone TrailGuide; install Git, Arduino CLI, Espressif board support and Adafruit libraries from official sources.
- Detect the real serial port; record exact working versions and commands.

## 2. V1 bring-up
- Wire one OLED with USB unplugged, check connections, reconnect.
- Compile, upload, and read serial diagnostics.
- Run V1 acceptance checks; fix display layout and input problems.
- Review starter advice before expanding toward forty cards.
- Save a tested V1 release and recovery instructions.

## 3. V2 after arrival (expected Nov. 1)
- Identify board/revision and run manufacturer display, touch and sensor examples.
- Extract shared content and shuffle logic.
- Implement round UI and tap-to-draw first.
- Add needle animation, shake detection and idle dimming.
- Run V2 acceptance checks with USB power.

## 4. Workshop readiness
- Select battery/case only after checking actual requirements.
- Measure runtime and charging.
- Freeze essential features several days before November 9.
- Retain source, tested firmware, recovery instructions, USB cable and V1 fallback.

## Current status
Starter V1 sketch with twelve cards, address detection, debounce and shuffled draws. No compile or hardware tests completed. Pi setup/access pending. Round display ordered. Power and enclosure deferred. No V2 firmware or simulator yet.
