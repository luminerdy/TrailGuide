# Wi-Fi firmware updates

Required for V1 and carried forward independently to V2 (portrait ESP32-C6) and V3 (round ESP32-S3). Verify each target's flash/partition layout, update implementation and recovery; a V1 OTA test is not verification of either new target.
V1 status: implemented, host-tested, ESP32-compiled and hardware-verified on prior firmware. Currently running v1-0.2.4 after a USB upload correcting the OLED driver to SH1106; OTA code and credentials are unchanged and serial confirms OTA readiness. Two consecutive wireless updates, password rejection, reconnect and interrupted-transfer recovery were verified before this driver correction. See BRINGUP.md for actual results.

V2 status: authenticated local Wi-Fi/OTA is implemented and hardware-tested through v2-0.1.4, including two successful wireless updates, password rejection, interrupted-transfer recovery and offline/reconnect checks. See [V2-WIFI-UPDATES.md](V2-WIFI-UPDATES.md) for its separate credentials, safe uploader, ephemeral callback port and remote version verification. V3 remains independent/unverified.

## Behavior

The first flash uses USB and installs firmware with ArduinoOTA support plus a partition layout containing two application slots and OTA metadata. Confirm the actual flash capacity and chosen partition layout before flashing. Subsequent application updates can be sent by the Pi over the same trusted local Wi-Fi network, without a USB data connection. The ESP32 still needs power.

Use station mode with a stable, identifiable hostname. Report the IP address and update readiness over serial. Support direct IP addressing if hostname discovery is unavailable. Do not require internet access or start an open access point.

Configure the network name, Wi-Fi password and a distinct, nonempty OTA password in ignored local secrets.h, with a tracked example containing placeholders only. Firmware builds without credentials leave Wi-Fi disabled. Do not commit credentials or expose updates to the public internet. Use the authentication supported by the pinned ESP32 core and record its version.

Network connection and reconnect attempts must not block button handling or display startup. Missing Wi-Fi must leave all advice functionality working. Start the update listener after obtaining an address and service it regularly, including in serial-only mode. Indicate an update in progress and report failures. Every replacement firmware must retain OTA support and compatible credentials unless deliberately returning to USB-only operation.

## Acceptance before declaring ready

- Record the ESP32 core, Arduino CLI, partition scheme and actual USB port.
- Compile the OTA-enabled sketch and flash it over USB.
- For V1, with the OLED absent verify serial advice and BOOT still work; for V2, verify touchscreen guidance remains available while offline.
- Send an application with a changed version identifier over Wi-Fi; verify reboot into that version and a second successful wireless update.
- Verify an incorrect OTA password is rejected.
- Verify startup without the network and a later reconnect; advice remains usable.
- Verify direct-IP uploads and document the exact working upload command without embedding passwords in the repository.
- Test a failed/interrupted transfer and recovery on the bench; do not claim automatic rollback without implementing and testing it.
- Retain USB recovery instructions and a known working firmware image. Partition-table changes require a separate USB recovery/update procedure.

Implementation reference: [Espressif ArduinoOTA example](https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino). Pin the dependency version before implementation; the master example can change.

## V1 implementation and commands

Pinned target: Arduino CLI 1.3.1, Espressif core 3.3.2; SH110X 2.1.14,
GFX 1.13.0 and BusIO 1.17.4. Install pinned libraries with `--no-deps` after
installing BusIO to prevent transitive upgrades. This core uses
SHA256 password hashes and PBKDF2-HMAC-SHA256 challenge/response with 10,000
iterations. Use the uploader bundled with the same core.

Copy `oled_oracle/secrets.example.h` to ignored `oled_oracle/secrets.h` and
replace all placeholders. Use a distinct OTA password. Keep the file readable
only by your account (`chmod 600 oled_oracle/secrets.h`). Never publish compiled
firmware containing credentials.

```sh
arduino-cli compile --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --output-dir build/v1 oled_oracle
arduino-cli upload --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --port /dev/ttyUSB0 --input-dir build/v1 oled_oracle
python3 tools/ota_upload.py ESP32_IP build/v1/oled_oracle.ino.bin
```

The helper prompts for the OTA password without placing it in shell history or
process arguments. Use an application `.ino.bin`, never the merged image, for
OTA. Both devices must be reachable on the same trusted LAN; uploads use UDP
3232 to the ESP32 and a TCP callback to the Pi on port 3232. `--host PI_IP` can
select the Pi interface if needed. `trailguide-v1.local` is optional; the serial
IP works without mDNS.

The default 4 MB partition table provides two application slots and OTA metadata.
Do not change partitions over OTA. Recover by using the USB upload command above
with a known working build and its original partition settings. Retain the whole
build directory locally; it contains credentials and is ignored by Git.

Connection/retry polling does not wait for Wi-Fi. Missing credentials disable
Wi-Fi; missing OLED still starts and services OTA. An actual transfer uses the
core's synchronous update handler, so normal interaction can pause during that
transfer. Serial reports start, failure and completion. No automatic rollback is
implemented or claimed.
