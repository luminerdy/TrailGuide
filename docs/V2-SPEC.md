# V2 — ESP32-C6 portrait touchscreen

Status: current touchscreen development target. Board description is supplied; USB enumeration is observed. Exact PCB/revision, chip/flash identity, display/touch operation, partition layout and usable memory are not yet hardware-verified. No TrailGuide firmware has been built or flashed for this target.

## Version assignment

- V1: classic ESP32 plus SH1106 OLED; preserve as the working fallback.
- V2: ESP32-C6 1.47-inch capacitive touchscreen, nominal 172x320 portrait display, pin-header variant.
- V3: the planned round ESP32-S3 1.85-inch touchscreen; see [V3-SPEC.md](V3-SPEC.md).

This supersedes the earlier use of V2 for the round board. The existing round browser preview remains a V3 design artifact, not a portrait-board test.

## Expected hardware, pending identification

The supplied description appears to match the Waveshare ESP32-C6-Touch-LCD-1.47 family with pin headers. Confirm the PCB marking/revision and exact variant before selecting drivers or examples.

- Reported ESP32-C6 architecture and onboard 8MB flash.
- Reported 1.47-inch IPS capacitive touch display, 172x320 pixels.
- Reported AXS5106L touch controller.
- Manufacturer family documentation identifies a JD9853 display controller; confirm the actual board's resources/schematic and downloaded example configuration before using it.
- Do not copy V1 wiring, GPIO assignments, partitions or binaries, or assume the V3 board's memory/peripherals apply to V2.

Reference: [manufacturer hardware documentation](https://docs.waveshare.com/ESP32-C6-Touch-LCD-1.47). Example names, dependencies and pin maps must be checked against the actual C6 hardware, not a similarly sized non-touch board or an unrelated S3 example.

## First bring-up gates

1. Re-enumerate USB and identify the intended physical board/serial port independently of V1. A USB interface alone is not a chip or flash-capacity probe.
2. Inspect exact board/revision, available toolchain and manufacturer sources. Keep V1 connected safely but excluded from any C6 upload command.
3. Before an authorized write, identify chip/flash/partition/security state and retain the original firmware if readable, plus a documented recovery procedure. These operations may reset the C6; enumeration itself does not authorize resetting/flashing either board.
4. Compile and run the correct manufacturer's display and touch examples, preserving evidence of each stage separately. Test orientation, visible pixels, touch mapping and edges on the actual board.
5. Measure usable RAM, flash and drawing performance with pinned dependencies. Do not assume the browser preview proves LVGL, framebuffer or board support.
6. Build a separate V2 target with compatible authenticated OTA, reviewed partitions and USB recovery. Keep advice functional without Wi-Fi. Do not reuse a V1 application binary.

## Product and portrait UI

Shared requirements remain in [PRODUCT-SPEC.md](PRODUCT-SPEC.md), especially P-16 Ask/Try behavior and new P-18 portrait adaptation. [FACILITATOR-MODES.md](FACILITATOR-MODES.md) defines content/review/recording semantics for all touchscreen targets.

Use the same stable cards, themes and explicit tracking model. The portrait view needs its own layout and readable text/page budget; do not shrink or crop the 360x360 circle to fit 172x320. Start with stacked labeled controls, one Guide me entry, short guidance pages and a read-only eight-card portrait preview before enabling storage or claiming UI acceptance. Exact dimensions/fonts remain a design task pending orientation and hardware review.

Touch is the essential input. Motion/compass animation is optional and requires separately verified hardware and input safeguards. Do not assume a sensor or shake behavior merely because it was specified for V3.

## Acceptance

- Actual C6 board, revision, port, flash/partitions and working dependency versions are recorded without private identifiers.
- Display and touch examples run with visibly correct orientation and input mapping.
- Portrait text, buttons and complete-copy navigation are reviewed on the actual 172x320 screen, including edge taps and longest representative copy.
- Eight paired draft cards support read-only Ask/Try/Guide me review without fake recording or hidden gesture dependencies.
- Content eligibility, pools, explicit team/mode confirmation, deduplication, Undo, save/event migration and durable tracking pass the shared contracts before tracking is enabled.
- Offline operation, reboot, soak, OTA retention and USB recovery are independently verified for V2; prior V1 results are not substituted.
- V1 firmware/recovery is preserved and V3 retains a separate hardware/layout contract.

Power starts with USB. Battery, charging and enclosure readiness require actual-board verification and measurement; do not infer them from product-family marketing.
