# TrailGuide project continuity

Codex develops this project on Scotty's Raspberry Pi 500. The Pi builds and flashes firmware; the ESP32 runs independently. Deliver a dependable offline advice oracle for Frontier Ops, November 9–12, 2026.

## At session start

Read README.md, docs/BRINGUP.md, docs/PLAN.md, docs/V1-SPEC.md and docs/WIFI-UPDATES.md before continuing. Read docs/V2-SPEC.md when working on the incoming round touchscreen board. Recheck current files, Git status, serial devices and available tools; prior environment restrictions may have changed.

## Decisions and user requests

- V1 uses classic ESP-WROOM-32, BOOT on GPIO0, and SSD1306 128x64 I2C OLED on SDA21/SCL22, powered at 3V3. Verify the real board before flashing.
- As of October 29 the ESP32 is connected over USB; the user plans to connect the OLED the next day. Continue development without the display using serial advice.
- Implement password-protected local Wi-Fi firmware updates before the first USB flash. OTA is implemented and hardware-verified. Advice must work without Wi-Fi. Keep credentials in ignored local files.
- Preserve V1 as the fallback. V2 is a separate Waveshare ESP32-S3-Touch-LCD-1.85 target, expected November 1; inspect its revision before implementing hardware support.
- The user prefers autonomous work and minimal permission interruptions. Respect the active session's permission boundaries; configuration instructions given in chat do not prove settings were changed.

## Privacy

Never publish IP addresses, actual network names, passwords, private keys, local credentials, build binaries or private serial logs. Keep credentials in ignored local files. Use placeholders in shared commands.

## Current checkpoint

Git history is restored from GitHub main bca518a7a29879b41f6d0b699de91130bd29904e, preserving local changes. The user confirmed https://github.com/luminerdy/TrailGuide as the publishing destination. Use the existing GitHub origin; the earlier GitLab mention was superseded. Check current Git status before publishing.

V1 is ESP32-D0WD-V3 revision 3.1, 4 MB flash, connected through CP2102 at /dev/ttyUSB0. Network and serial access work. Arduino CLI 1.3.1, Espressif core 3.3.2, SSD1306 2.5.15, GFX 1.13.0 and BusIO 1.17.4 are installed. See docs/BRINGUP.md for exact commands and local recovery artifacts.

Normal firmware v1-0.2.3 is running after USB bring-up and successful OTA updates. Host tests and ESP32 builds pass. Hardware checks verified serial-only advice, fifty electrical GPIO0 cycles, two consecutive wireless updates, wrong-password rejection, station reconnect and interrupted-transfer recovery. No automatic rollback is claimed. The temporary bench firmware was replaced by normal v1-0.2.3.

## Next work

1. With USB unplugged, have the user connect the OLED to 3V3/GND/SDA21/SCL22. Reconnect and reset; no live OLED hot-plug detection is implemented.
2. Verify welcome and every card, mechanical BOOT presses, fifty-press/thirty-minute acceptance, reset, power-cycle and power-only operation.
3. Keep V1 as the fallback; inspect the V2 board and revision after arrival before adding its hardware support.

## Save progress

Update docs/BRINGUP.md after meaningful work and before ending a session: changes, exact commands/versions, test results, actual flashed firmware, blockers and next steps. Keep this file concise and revise outdated checkpoint facts. Never record passwords or private keys. Distinguish planned, host-tested, ESP32-compiled and hardware-verified work.
