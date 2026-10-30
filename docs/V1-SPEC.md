# V1 — ESP32 + OLED

## Purpose
Provide canned questions and small experiments that help workshop teams learn AI while working on their use cases. No live AI connection or network is required.

## Hardware
- Existing HiLetgo ESP-WROOM-32 / ESP-32S development board.
- Existing Hosyond SSD1306 I2C OLED, 128x64, fixed blue/yellow regions.
- Four jumper wires: VCC to 3V3; GND to GND; SDA to GPIO21; SCL to GPIO22.
- BOOT button on GPIO0, active low. No motion sensor.
- USB data cable for upload and initial power.
- Raspberry Pi is the development host, not a runtime dependency.

## Behavior
Power on to a welcome screen. One BOOT press displays a short titled advice card until the next press. Debounce presses and draw only once while held. Shuffle the entire deck without repeats within a cycle; avoid an immediate repeat across cycles. Reset may start a new shuffle.

Start with twelve cards in the sketch, expanding toward forty after readability review. Cover exploration, clarification, experiments, verification, and reflection.

## Display
Use size-one 6x8 text, up to 21 characters per line. Title occupies the top 16 pixels; body starts at y=20 with at most four lines; footer at y=56. Hand-wrap ASCII advice. Confirm every card fits on the actual display.

## Software
Arduino C++; ESP32 Dev Module target (`esp32:esp32:esp32`); Adafruit SSD1306, GFX and dependencies. Probe 0x3C/0x3D; report display detection and selected cards over serial at 115200 baud. Keep Wi-Fi off when update credentials are not configured; see [Wi-Fi updates](WIFI-UPDATES.md). Advice remains fully offline. Record exact working dependency versions after compilation.

## Acceptance
- Compile and upload from the Pi; record exact versions, port, commands and results.
- Welcome screen and every card are readable with no clipped text.
- Each press draws once; holding the button does not advance repeatedly.
- All twelve cards appear once per deck; no immediate boundary repeat.
- At least fifty presses and thirty minutes without a crash.
- Reset, power-cycle and USB reconnect work.
- Missing OLED yields a clear serial error and BOOT continues to draw advice over serial.

## Boundaries and status
Firmware v1-0.2.3 is ESP32-compiled and running after USB and OTA uploads; serial startup and GPIO0 input cycles are verified. OLED acceptance remains pending; direct-IP OTA is hardware-verified. V1 uses USB power. Battery, case, shake, audio, live AI and magnetic heading are deferred. Do not hold BOOT during normal reset/power-on.
