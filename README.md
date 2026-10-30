# TrailGuide

An offline advice oracle for Trail Guides at the Frontier Ops AI workshop, November 9–12, 2026. Draw a new bearing: a practical question or experiment that helps teams learn to work with AI.

| Version | Hardware | Interaction | Status |
| --- | --- | --- | --- |
| V1 | HiLetgo ESP-WROOM-32 + Hosyond SSD1306 128x64 OLED | Onboard BOOT button | USB and OTA verified; OLED acceptance pending |
| V2 | Waveshare ESP32-S3-Touch-LCD-1.85, 360x360 round touchscreen | Tap and shake, compass animation | Planned |

## Start here

- [Delivery plan](docs/PLAN.md)
- [V1 specifications](docs/V1-SPEC.md)
- [V2 specifications](docs/V2-SPEC.md)
- [Raspberry Pi handoff](docs/PI-HANDOFF.md)
- [Wi-Fi update requirements](docs/WIFI-UPDATES.md)
- [Current Pi development results](docs/BRINGUP.md)
- [V1 firmware](oled_oracle/oled_oracle.ino)

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

Arduino C++, Espressif ESP32 core, Adafruit SSD1306, Adafruit GFX and dependencies. V1 target: ESP32 Dev Module (`esp32:esp32:esp32`). Serial diagnostics: 115200 baud. The sketch probes 0x3C and 0x3D.

For Arduino IDE, add https://espressif.github.io/arduino-esp32/package_esp32_index.json in Additional Boards Manager URLs, install esp32 by Espressif Systems, install the Adafruit libraries, and open the sketch. Select the detected port and ESP32 Dev Module, Verify, then Upload. If upload remains at Connecting, hold BOOT until writing begins, then release.

For Pi setup, let the agent inspect and install the toolchain once terminal access is available; see the handoff document. No credentials belong in this repo.

References: [Espressif I2C](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html), [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306).
