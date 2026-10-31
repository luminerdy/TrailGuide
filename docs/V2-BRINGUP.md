# V2 portrait C6 — first USB bring-up

## Evidence and limits

The current V2 target has been probed, backed up, USB-flashed with the manufacturer LVGL demo, and serial-verified after a controlled application reset. This is a hardware bring-up image, not the TrailGuide application or production acceptance. V1 was excluded from every reset, probe and upload command.

- Chip family: ESP32-C6, silicon revision 0.2, 40 MHz crystal. Reported security state: secure boot and flash encryption disabled.
- Flash capacity: 8 MB detected. Two independent full 8,388,608-byte reads matched byte-for-byte before any write. The original image and verification logs remain private and ignored under `build/v2-c6-bringup/`.
- Package-name caveat: esptool 4.11.0 described the device as C6FH8; uploader esptool 5.1.0 described it as C6FH4. Both identified C6 revision 0.2. Keep this tool-version discrepancy explicit rather than treating either package label as physical PCB identification. The 8 MB flash measurement is separate evidence.
- Original partition table: NVS at `0x9000`, size `0x6000`; PHY initialization at `0xf000`, size `0x1000`; factory application at `0x10000`, size `0x600000`.
- Built the downloaded manufacturer's unmodified `04_lvgl_arduino_v8` example using its bundled libraries. Compiler reported 657,214 bytes of application usage and 67,292 bytes of globals; these are static build figures, not measured runtime free memory or frame rate.
- Upload verified hashes for bootloader, partition table, OTA initialization image and application, then reset the C6.
- Serial startup after an additional controlled C6-only reset showed the C6 ROM boot, `Arduino_GFX LVGL_Arduino_v8 example`, `V8.4.0`, touch-driver initialization output `read: 8161`, and `Setup done`. The touch driver prints three decimal register bytes without separators; do not interpret that concatenation as a decoded part ID. No I2C failure or display-begin failure message appeared in the captured startup.
- Visible pixels, orientation, edge mapping and a real touch action still require human observation. Serial initialization is not proof of a readable display or correct touch coordinates.
- The demo does not implement TrailGuide cards, team storage, Wi-Fi updates or authenticated OTA. Two OTA-capable application slots are provisioned, but that alone does not install or verify OTA functionality.

## Manufacturer source and configuration

Sources: [hardware documentation](https://docs.waveshare.com/ESP32-C6-Touch-LCD-1.47), [manufacturer demo archive](https://files.waveshare.com/wiki/ESP32-C6-Touch-LCD-1.47/ESP32-C6-Touch-LCD-1.47-Demo.zip).

Downloaded archive SHA256:

`ad8e27b172035fb73b5dbe88b821b1ff37bd677c20db294a9da7e5317ee176dd`

Verified build dependencies:

- Arduino CLI 1.3.1; Espressif Arduino core 3.3.2.
- Bundled GFX Library for Arduino 1.5.9 and LVGL 8.4.0.
- Bundled `esp_lcd_touch_axs5106l` driver and the example's `lv_conf.h`.
- Target `esp32:esp32:esp32c6:CDCOnBoot=cdc,FlashSize=8M,PartitionScheme=default_8MB,FlashMode=qio,FlashFreq=80`.

The family resources specify JD9853 display control. The downloaded example uses an `Arduino_ST7789` wrapper plus its custom `lcd_reg_init()` register sequence; retain that sequence. Do not replace it with a generic ST7789 hello-world configuration just because the C++ class has that name.

Example pin configuration:

- LCD SCK 1, MOSI 2, CS 14, DC 15, reset 22, backlight 23.
- Touch SDA 18, SCL 19, reset 20, interrupt 21; driver address `0x63`.
- Portrait 172x320, rotation 0. These are the inspected source settings; physical coordinate/readability acceptance is separate.

New Arduino `default_8MB` layout: NVS `0x9000/0x5000`, OTA metadata `0xe000/0x2000`, app0 `0x10000/0x330000`, app1 `0x340000/0x330000`, SPIFFS `0x670000/0x180000`, coredump `0x7f0000/0x10000`. This differs from the original factory layout. Restoring the original means restoring the full saved image, not just its application partition.

## Rebuild and upload

Run from the repository root with the locally retained manufacturer archive extracted under the ignored vendor directory. These commands document the successful build configuration; check the currently connected chip/port again before any repeat upload. Replace `PORT` with the independently identified C6 port, never V1's UART bridge.

```sh
ROOT="$PWD/build/v2-c6-bringup"
DEMO="$ROOT/vendor/Arduino/examples/04_lvgl_arduino_v8"
LIBS="$ROOT/vendor/Arduino/libraries"
FQBN='esp32:esp32:esp32c6:CDCOnBoot=cdc,FlashSize=8M,PartitionScheme=default_8MB,FlashMode=qio,FlashFreq=80'
FLAGS="-I$DEMO -DLV_CONF_INCLUDE_SIMPLE"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$LIBS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --output-dir "$ROOT/lvgl-demo" "$DEMO"
arduino-cli upload --fqbn "$FQBN" --port PORT \
  --input-dir "$ROOT/lvgl-demo" "$DEMO"
```

The include flags supply the example-specific LVGL configuration to both C and C++ compilation without modifying global libraries or the V1 toolchain. Close serial monitors before uploading. Keep binaries, logs and full-flash backups out of Git.

## Original-image recovery

Private recovery image: `build/v2-c6-bringup/original-flash-8mb.bin`. Its independent matching read is `original-flash-second-read.bin` in the same ignored directory. Files and logs are restricted locally. Retain both and compare size/hash before any restoration; do not overwrite them with a read of the newly flashed demo.

If restoration is requested, identify the C6 port again, close monitors, confirm security state has not changed, then use the private esptool 4.11.0 environment:

```sh
BACKUP=build/v2-c6-bringup/original-flash-8mb.bin
sha256sum "$BACKUP" build/v2-c6-bringup/original-flash-second-read.bin
python -m esptool --chip esp32c6 --port PORT --baud 921600 \
  write_flash --flash_size keep --flash_mode keep --flash_freq keep 0x0 "$BACKUP"
```

This overwrites the C6's entire flash, including partitions and NVS, with the preserved original state. Do not run it against V1 or substitute a V1/S3 image. The backup reads were verified; a restore has not been performed or claimed tested. USB recovery must remain available throughout development.

## Next gates

1. Human confirmation that the widgets are visible and a real button/slider reacts correctly; check orientation and edges.
2. Confirm the printed PCB variant/revision, runtime memory budget and optional peripherals separately.
3. Build/review the separate read-only portrait TrailGuide layout with eight draft pairs, rather than shrinking the circular preview.
4. Implement and test the C6-specific offline application, authenticated OTA and durable storage before any production claim. See [V2-SPEC.md](V2-SPEC.md) and [PLAN.md](PLAN.md).
