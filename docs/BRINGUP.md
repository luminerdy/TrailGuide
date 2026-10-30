# Pi development record

## Communication handoff — October 30, 2026

- User named the assistant Trailguide; the name is saved in persistent memory.
- Telegram outbound messaging succeeded after the user started the bot;
  Telegram returned a successful send acknowledgement. Personal chat identifiers
  are kept out of this repository.
- Gateway status reports a running manual process, not an installed system
  service. Automatic startup after reboot has not been verified.
- Two-way Telegram conversation has not been verified in this CLI session.
- User reconfirmed GitHub as the publishing destination, using the existing
  `luminerdy/TrailGuide` origin. The earlier GitLab wording was a mistake.
- Both host suites were rebuilt and passed before publication with C++17 and
  `-Wall -Wextra -Werror`. Hardware was not reflashed during this handoff.

## SH1106 correction — October 30, 2026

- User photo shows a narrow band of text with random pixels below. User's
  local OLED product details identify a Hosyond 1.3-inch white 128x64 **SH1106**,
  not the previously assumed SSD1306. Wiring remains SDA21/SCL22 at 3V3.
- Changed the driver to `Adafruit_SH1106G`, using Adafruit SH110X 2.1.14,
  and bumped the firmware to `v1-0.2.4`. OTA code and credentials are unchanged.
- Added a host regression assertion requiring the SH1106 display type; observed
  its intended failure against the old SSD1306 implementation, then both host
  suites passed with the corrected driver. Host stubs do not verify pixels.
- ESP32 build passed: 995,259 application bytes; 50,932 bytes globals. Local,
  ignored output: `build/v1-0.2.4-sh1106` (contains credentials; do not publish).
- USB connection returned. Uploaded `build/v1-0.2.4-sh1106` using Arduino CLI
  with the existing 4 MB/default partition settings; esptool identified the same
  ESP32-D0WD-V3 revision 3.1 and verified written hashes. Controlled-reset serial
  readback confirms `v1-0.2.4`, SH1106 at `0x3C`, and OTA readiness.
- User confirms the display now works and confirms the requested mechanical
  BOOT checks: new cards on presses and no repeated advance while held.
  Every-card readability, fifty presses/thirty minutes, and independent-power
  checks remain. USB connection errors were observed, but their cause was not
  established.

## Hermes takeover — October 30, 2026

- User reports the OLED is now connected. USB inspection finds the CP2102
  bridge at `/dev/ttyUSB0`; no firmware was flashed or changed.
- Local `main` and a fresh `git ls-remote origin refs/heads/main` both identify
  `442f637003264360b6f821aab61532df16285e0a`. Working tree was clean at takeover.
- Arduino CLI still reports 1.3.1. Both host suites were rebuilt with C++17,
  `-Wall -Wextra -Werror` and passed. Executables were written under `$TMPDIR`,
  not `/tmp`.
- Initial serial observation was garbled. A controlled RTS reset with DTR
  deasserted produced readable startup at 115200 8N1: normal firmware
  `v1-0.2.3`, OLED detected at `0x3C`, and OTA ready. The initial garble's
  cause is not established; do not infer a firmware or wiring fault.
- OLED address acknowledgement does not verify visible pixels or readability.
  Next: user confirmation of the welcome screen and mechanical BOOT behavior,
  then every-card readability, fifty presses/thirty minutes, and power checks.
- `gh` is not installed; remote commit comparison used Git. No publishing done.

## Previous verification — October 29, 2026

## Current checkpoint

V1 is classic ESP32-D0WD-V3 revision 3.1, 40 MHz crystal, 4 MB flash
(manufacturer 5e/device 4016), CP2102 10c4:ea60, `/dev/ttyUSB0`.
Raspberry Pi 500 runs aarch64 Debian 13, kernel 6.18.50+rpt-rpi-2712.
Network and serial access work; `scotty` belongs to `dialout`.
Git history restored at bca518a using `git fetch origin main` and
`git reset --mixed FETCH_HEAD`; existing edits preserved. Check Git history
and remotes for publication status.

Normal firmware is **v1-0.2.3**, with offline serial advice and authenticated
OTA. OLED is not connected. ESP32 joins a compatible 2.4 GHz network
on the same LAN as the Pi. Network identifiers and addresses are intentionally
omitted from shared records. Credentials came from the user-specified local file;
they are stored only in ignored, mode-600 `oled_oracle/secrets.h`, along with
a distinct generated OTA password. Never publish these firmware binaries.

## Installed versions

- Arduino CLI 1.3.1: `/home/scotty/.local/bin/arduino-cli`.
- Espressif Arduino core 3.3.2; default 4 MB partitions: OTA metadata and two
  0x140000-byte application slots at 0x10000 and 0x150000.
- Adafruit SSD1306 2.5.15, GFX 1.13.0, BusIO 1.17.4.
- Python esptool 4.11.0: `/home/scotty/.local/share/trailguide-venv`.

CLI installed from official ARM64 1.3.1 archive. Core installation required
`arduino-cli config set network.connection_timeout 600s` after a download timeout.
For reproducible libraries, install BusIO first, then GFX and SSD1306 with
`arduino-cli lib install --no-deps` and the versions above; dependency resolution
otherwise upgraded the initially requested GFX 1.12.1 to actual 1.13.0.

## Builds and USB upload

```sh
arduino-cli core install esp32:esp32@3.3.2 --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --output-dir build/v1-0.2.3 oled_oracle
arduino-cli upload --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --port /dev/ttyUSB0 --input-dir build/v1-0.2.1 oled_oracle
python3 tools/ota_upload.py ESP32_ADDRESS build/v1-0.2.3/oled_oracle.ino.bin
```

USB-flashed v1-0.2.0 initially, then v1-0.2.1 with the compatible network.
Written hashes verified by esptool. Normal credentialed firmware uses 995,239
of 1,310,720 application bytes and 50,916 bytes globals. Credential-free build
also passed (321,583 bytes application, 22,468 globals), not flashed.

## Verification

Both host suites pass with C++17, `-Wall -Wextra -Werror`:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests/stubs tests/firmware_test.cpp -o /tmp/trailguide-firmware-test
/tmp/trailguide-firmware-test
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/ota_test.cpp -o /tmp/trailguide-ota-test
/tmp/trailguide-ota-test
```

Host coverage: missing OLED/allocation failure, both display addresses, 1,000
deck cycles, debounce/hold/rollover, layout limits, offline advice during network
failure, retries, listener reconnect and callbacks. These do not verify OLEDs.

Hardware results with OLED absent:

- Serial startup at 115200 reports version and serial-only fallback.
- Fifty GPIO0 cycles via CP2102 DTR produced exactly fifty cards, no adjacent
  repeats or resets; held input drew once. Advice worked without reachable Wi-Fi.
  These are electrical input checks, not fifty mechanical BOOT presses.
- Wrong OTA password rejected (authentication error 0); firmware remained usable.
- Two consecutive direct-IP OTA uploads succeeded: v1-0.2.1 → v1-0.2.2 →
  v1-0.2.3. Serial confirmed each new version and renewed OTA readiness.
- Authenticated transfer terminated by sender at 10% reported OTA error 4.
  Normal v1-0.2.3 remained bootable. A subsequent OTA upload succeeded. This
  tests interrupted-transfer recovery, not automatic rollback or power loss.
- Temporary v1-0.2.4-bench disconnected the station once at 20 seconds.
  Advice continued while disconnected; production retry restored Wi-Fi and OTA.
  Normal v1-0.2.3 restored afterward. Bench source/binary retained locally.
- USB-powered throughout; application transfers used Wi-Fi, not USB upload.
  USB-independent power operation remains to be checked physically.

## Recovery and remaining acceptance

Verification logs are retained under ignored `build/verification/`.

Known normal build: ignored `build/v1-0.2.3`, including source snapshot.
Application SHA256: `3564e24ea0741ed7df65ff36fac86f9a61b56465bd5ee1afb78da1eceb43543c`.
Original pre-TrailGuide flash: `build/recovery/pre-trailguide-4mb.bin`, SHA256
`d911e73b462b36998a21300ed3be4423e6cf3fc7ba0052f400828e9712bfbc5e`.
Use the USB command above with `build/v1-0.2.3` to recover the normal build.
Do not send merged images or partition changes through OTA. See WIFI-UPDATES.md.

Pending: OLED wiring with USB unplugged, every-card readability, mechanical BOOT,
thirty-minute acceptance, power-cycle and USB reconnect/power-only checks.
An explicit RTS reset initially produced noisy boot fragments; later monitor
opens showed clean boots. Router/AP disappearance and return, mDNS upload, and
power loss during transfer have not been tested. Direct-IP OTA is verified.

Session handoff saved in AGENTS.md. Do not publish actual network identifiers,
addresses, credentials, local binaries or verification logs. Next session starts
with OLED bring-up; preserve normal v1-0.2.3 and its OTA credentials.

Publishing destination confirmed by the user: https://github.com/luminerdy/TrailGuide.
Session work is committed locally; shared records omit private network details.
