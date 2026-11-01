# V3 round S3 — read-only setup checkpoint

## Scope and measured identity

This checkpoint covers USB identification, read-only silicon/security/flash probes, two original-flash backups and a host build of the manufacturer's example. **No firmware upload, flash erase/write, eFuse write or application deployment was performed.** A first write remains gated on physical board identification and a reviewed test image. V1/V2 are excluded; C6 drivers, pin maps, binaries and partitions are not reused.

- Re-enumerated one native Espressif USB JTAG/serial interface, USB VID/PID `303a:1001`. No process owned its serial node immediately before the probe. USB identity alone does not identify the board or chip.
- Pinned esptool **4.11.0** measured **ESP32-S3 (QFN56), silicon revision v0.2**, a 40 MHz crystal and embedded **8 MB PSRAM (AP_3v3)**. This is a silicon feature report, not a runtime PSRAM memory test.
- JEDEC flash manufacturer `0x20`, device `0x4018`; detected capacity **16 MB (16,777,216 bytes)**. Secure Boot and Flash Encryption were reported disabled.
- The intended product is the **original Waveshare ESP32-S3-Touch-LCD-1.85**, not the B/C enclosure variants or the non-touch LCD model. The observations are consistent with its published memory specification but do not prove the physical model, PCB revision, display or sensor identity. Obtain a readable rear silkscreen/model/revision photograph before approving a write.
- Manufacturer documentation distinguishes unmarked V1, V2 marked `2.0`, and V2.1 marked `2.1`, and states they share programs.[1] This is a manufacturer compatibility statement, not confirmation of the attached board's revision.

## Original preservation and recovery

All private evidence is local and ignored under `build/v3-s3-bringup/`. Do not upload binaries, raw logs, device addresses or credential-bearing data. The directory is private; original images and raw probe/read logs have mode `600`.

Two independent complete reads start at flash offset zero and cover the measured 16 MB:

- `build/v3-s3-bringup/original-flash-16mb.bin`
- `build/v3-s3-bringup/original-flash-second-read.bin`

Each file is **16,777,216 bytes**. The two SHA-256 values matched and a byte-for-byte comparison passed. Sizes, SHA-256 values and byte-for-byte comparison must all agree before any future write. The local manifest `build/v3-s3-bringup/backup-evidence.json` records the hashes; preserve both original files unchanged, not as destinations for later reads. Matching backups establish repeatable readback, not a tested restoration.

Probes used explicit `--before usb_reset --after no_reset`, not a UART-bridge reset sequence. This is esptool's native USB JTAG/serial reset strategy. The uploaded read stub runs in RAM only; it is not a firmware write. Every serial operation must first re-identify the same attached S3 and check port ownership; never select another device by remembered port name. The reproducible read command, with the existing pinned local Python environment active, is:

```sh
umask 077
python -m esptool version  # must report 4.11.0
python -m esptool --chip esp32s3 --port PORT --baud 921600 \
  --before usb_reset --after no_reset \
  read_flash 0 0x1000000 NEW_UNUSED_PRIVATE_BACKUP_PATH
```

`PORT` is a freshly verified native S3 port and the destination must not exist. Use different filenames for independent reads. Capture output privately, not in published transcripts. The existing preserved pair must not be overwritten when repeating this procedure. After both reads and comparisons, a controlled S3-only `esptool.reset.HardReset(port, uses_usb=True).reset()` was requested with DTR initially false to leave the read stub; the native serial node remained present. No original-application startup or visible display acceptance is inferred from that reset.

Original partition records decoded from the saved image at `0x8000`:

| Label | Type/subtype | Offset | Size |
| --- | --- | --- | --- |
| nvs | data / `0x02` | `0x9000` | `0x6000` |
| factory | app / `0x00` | `0x10000` | `0x300000` |
| flash_test | data / `0x81` | `0x310000` | `0x84000` |
| model | data / `0x82` | `0x394000` | `0x5c3000` |

Only partition metadata was summarized; original application/NVS contents remain private. This is not the proposed Arduino demo partition layout. Future recovery would require separate authorization, fresh identity/security checks, hash/size checks and restoration of the **whole original image at offset zero**, not merely its factory application. No restoration has been tested. Do not apply a C6 image or partition layout.

## Manufacturer sources and exact package

Official original-model references:

- [Hardware and revision reference](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85)
- [Resources and schematics](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85/Resources-And-Documents)
- [Arduino dependencies and example](https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85/Arduino)
- [Original schematic](https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85.pdf)
- [V2.1 schematic](https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85_V2.1.pdf)
- [Manufacturer-linked demo](https://drive.google.com/file/d/1sOXcEqyUIieougwPozj5q5Z8X1xILjIs/view)

The actual downloaded package is `ESP32-S3-Touch-LCD-1.85-Demo.zip`, **201,161,399 bytes**, SHA-256:

`fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990`

This is a locally computed integrity pin, not a manufacturer signature or independently published checksum. The Google Drive large-file confirmation was resolved to the actual ZIP; an HTML warning page was not mistaken for the archive. ZIP CRC testing passed for all 4,832 entries. Extracted Arduino source/library files were compared against the archive: **1,836 files matched unchanged**. A guessed static ZIP URL returned 404 and was not substituted as the source. Direct automated documentation retrieval returned 403; the official pages were read in a real browser instead.

The archive and extracted vendor tree live only under `build/v3-s3-bringup/`; both schematic PDFs and extracted reference text are retained there. The original schematic labels QMI8658C, while the V2.1 schematic labels QMI8658A.[4][5] Do not infer the actual populated sensor suffix from either reference alone.

## Inspected display/touch/IMU configuration

The published original-model hardware table and bundled `Arduino/examples/LVGL_Arduino` agree on the following configuration.[1] These are **reference/source settings**, not a physical bus/pixel/touch verification:

| Function | Reference / bundled source |
| --- | --- |
| Display | ST77916; 360×360; 16-bit color; QSPI, 80 MHz source setting |
| LCD clock / CS | GPIO40 / GPIO21 |
| LCD data 0 / 1 / 2 / 3 | GPIO46 / GPIO45 / GPIO42 / GPIO41 |
| LCD TE / backlight | GPIO18 / GPIO5 |
| LCD reset | TCA9554 EXIO2; native reset GPIO is `-1` |
| Touch | CST816 family, I2C address `0x15`; exact fitted suffix unverified |
| Touch SDA / SCL / interrupt | GPIO1 / GPIO3 / GPIO4; separate `Wire1` bus, 400 kHz |
| Touch reset | TCA9554 EXIO1; native reset GPIO is `-1` |
| Shared peripheral I2C SDA / SCL | GPIO11 / GPIO10 (`Wire`) |
| GPIO expander | TCA9554PWR at `0x20` |
| IMU | QMI8658 accelerometer/gyroscope; example selects `0x6b`, header also defines `0x6a` |
| IMU interrupts | Manufacturer table: INT1 EXIO5, INT2 EXIO4 |

EXIO labels are expander outputs, **not ESP32 GPIO numbers**. The manufacturer describes an expander interrupt change to GPIO13 on newer hardware; verify the actual revision/schematic before adding interrupt-driven features. Preserve the manufacturer's ST77916 panel initialization and expander-controlled reset ordering. The bundled all-peripheral example also initializes RTC, SD, audio and microphone and performs a Wi-Fi scan. It is not a minimal display-only or privacy-hardened TrailGuide application, nor authenticated OTA firmware. Nothing in this checkpoint runs those behaviors on the board.

## Reproducible host build — no upload

Selected the actual board-level `Arduino/examples/LVGL_Arduino`, not LVGL's generic library example. It includes display, CST816 touch, QMI8658 and the manufacturer's UI. Source and bundled libraries remain unmodified.

- Existing Arduino CLI **1.3.1**, installed Espressif Arduino core **3.3.2**; no global dependencies installed/replaced.
- Bundled Arduino LVGL **8.3.10**, bundled ESP32-audioI2S **2.0.0**, local source drivers, core `ESP_I2S` and `ESP_SR`.
- Manufacturer's published core range is **3.0.2–3.1.1**.[3] Building with the existing 3.3.2 is a compatibility experiment, not manufacturer-certified compatibility, even if it compiles.
- Use the bundled `libraries/lvgl/src/lv_conf.h` for C and C++; 16-bit color, 48 KiB LVGL heap and demo configuration are retained.
- `PSRAM=opi` and 16 MB QIO flash are source-target build assumptions for the original S3R8 product, not measured runtime operation.
- The unmodified example enables speech recognition, so the selected core partition profile is `esp_sr_16`. This deliberately differs from the original image. Its two 3 MB OTA app slots do not mean the example implements TrailGuide OTA. Review model-image packaging and every upload offset before any later write.

Run from the repository root with the existing CLI on `PATH`:

```sh
umask 077
ROOT="$PWD/build/v3-s3-bringup"
DEMO="$ROOT/vendor/Arduino/examples/LVGL_Arduino"
LIBS="$ROOT/vendor/Arduino/libraries"
FQBN='esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=esp_sr_16,PSRAM=opi,FlashMode=qio'
mkdir -p "$ROOT/config"
cp "$LIBS/lvgl/src/lv_conf.h" "$ROOT/config/lv_conf.h"
FLAGS="-I$ROOT/config -DLV_CONF_INCLUDE_SIMPLE"
arduino-cli compile --jobs 1 --fqbn "$FQBN" --libraries "$LIBS" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-path "$ROOT/compile-cache" --output-dir "$ROOT/lvgl-demo" "$DEMO"
```

The exact local wrapper is `build/v3-s3-bringup/compile-demo.sh`; compile output is captured privately in `compile.private.log`. Compilation does not open a serial port or upload firmware. The first attempt placed the entire LVGL source include directory in the extra flags; this let headers resolve without Arduino discovering/building the LVGL library, and linking failed with undefined `lv_init`, `lv_tabview_get_tab_act` and other LVGL symbols. The build cache had no LVGL library objects and the used-library list omitted LVGL. The corrected command above supplies a **byte-identical copy of only `lv_conf.h`** in a separate local config directory, so library discovery can select and compile the bundled LVGL normally. No vendor source patch or global library installation was used.

## Verified build result and packaging caveat

The corrected compile **exited 0**. Static usage was **2,489,075 bytes (79%)** of the 3,145,728-byte app slot and **188,396 bytes (57%)** of globals, leaving 139,284 bytes by the compiler's static accounting. These are not runtime free-memory, PSRAM, frame-rate or hardware acceptance measurements. The exported application is **2,489,216 bytes** (image padding/metadata differs from the reported sketch usage); its ELF, bootloader, partition and merged images were verified to exist. Offline esptool S3 image parsing passed with valid checksum and validation hash.

The primary candidate is `build/v3-s3-bringup/lvgl-demo/LVGL_Arduino.ino.bin`, SHA-256:

`9c2ee16ce42571cf7ce9f3c3105b3d60f514321987b92f2efc7d2f8b6b2343ff`

`build/v3-s3-bringup/build-evidence.json` records local paths, sizes and SHA-256 values for all exported artifacts. The **16 MB merged image is not a complete speech-model deployment**: its model partition (`0xd10000`–`0xfeffff`, 3,014,656 bytes) is all `0xff`. The core separately generated `build/v3-s3-bringup/compile-cache/srmodels.bin`, **2,468,362 bytes**, SHA-256 `0312f2dde9581cd604e752fbfa287d687a2acc0631e593a35a24c4a518d75879`. The installed core's board recipe supplies that separate image at `0xd10000` during a normal speech-profile upload. It is retained but **not written**, merged or assumed unnecessary. Do not approve an application-only/merged-only upload on the strength of compilation.

The actual built partition table was decoded and retained in `build-partitions.json`: NVS `0x9000/0x5000`, OTA metadata `0xe000/0x2000`, app0 `0x10000/0x300000`, app1 `0x310000/0x300000`, SPIFFS `0x610000/0x700000`, model `0xd10000/0x2e0000`, coredump `0xff0000/0x10000`. It differs materially from the preserved original image.

Core 3.3.2 also contains a named `waveshare_esp32_s3_touch_lcd_185` board definition with its own pin variant and defaults (including a 120 MHz flash-frequency property). Its existence and configured USB product ID do not identify the attached physical board. The successful command deliberately documents the explicit generic S3 settings actually tested, not a claim that the named preset, its defaults or every hardware revision has been verified.

## Remaining gates

1. Confirm the exact physical product/rear silkscreen/revision and touch variant from a readable label/photo; match the relevant schematic. USB/silicon/flash cannot establish these.
2. Review both preserved full-flash reads, local hashes, original partitions and the candidate build/partition/model artifacts. Keep originals immutable. Obtain explicit firmware-write approval separately.
3. If a manufacturer-version build is needed, isolate a compatible core under the ignored local build tree only; do not replace the host's global core or libraries. Do not silently patch the unmodified reference to hide compatibility failures.
4. After a separately authorized upload, verify boot and actual pixels, touch orientation/edge mapping, IMU identity/readings, memory stability and power behavior. No such hardware acceptance is claimed here.
5. Build V3-specific offline application, shake/rest/cooldown handling, reveal cancellation, touch alternative, reduced-motion behavior, authenticated update/recovery and human acceptance separately. The manufacturer demo and browser study are not production V3 firmware.

## Sources

[1] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85
[3] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85/Arduino
[4] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85.pdf
[5] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85_V2.1.pdf
