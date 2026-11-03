# V3 round S3 — display/touch and IMU bring-up

## Scope and measured identity

Current hardware: **v3-0.1.6 DRAFT** recovery correction is installed and startup/UI/real-IMU verified; see [V3-TRIAL.md](V3-TRIAL.md). Plain compass/touch were reported working. Corrected physical flip/card-only timing remain pending. Positive finite USB logging budget is retained; reader-closed compass/sample progress was verified. Earlier diagnostic/browser sections keep their historical scope.

- Re-enumerated one native Espressif USB JTAG/serial interface, USB VID/PID `303a:1001`. No process owned its serial node immediately before the probe. USB identity alone does not identify the board or chip.
- Pinned esptool **4.11.0** measured **ESP32-S3 (QFN56), silicon revision v0.2**, a 40 MHz crystal and embedded **8 MB PSRAM (AP_3v3)**. This is a silicon feature report, not a runtime PSRAM memory test.
- JEDEC flash manufacturer `0x20`, device `0x4018`; detected capacity **16 MB (16,777,216 bytes)**. Secure Boot and Flash Encryption were reported disabled.
- A clear PCB photograph confirms the printed model **ESP32-S3-Touch-LCD-1.85** and **Rev2.1**, matching the intended original touch model rather than the B/C enclosure or non-touch variants. The physical-model identification gate is resolved; this does not establish working display/touch/IMU or runtime PSRAM.
- Manufacturer documentation distinguishes unmarked V1, V2 marked `2.0`, and V2.1 marked `2.1`, and states they share programs.[1] The attached PCB's Rev2.1 marking is separately photo-confirmed; runtime compatibility still needs actual-board evidence.

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

EXIO labels are expander outputs, **not ESP32 GPIO numbers**. The manufacturer describes an expander interrupt change to GPIO13 on newer hardware; verify the actual revision/schematic before adding interrupt-driven features. Preserve the manufacturer's ST77916 panel initialization and expander-controlled reset ordering. The bundled all-peripheral example also initializes RTC, SD, audio and microphone and performs a Wi-Fi scan. It is not a minimal display-only or privacy-hardened TrailGuide application, nor authenticated OTA firmware. The installed minimal test does not run those optional behaviors.

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

The compiled-only manufacturer reference is `build/v3-s3-bringup/lvgl-demo/LVGL_Arduino.ino.bin`, SHA-256:

`9c2ee16ce42571cf7ce9f3c3105b3d60f514321987b92f2efc7d2f8b6b2343ff`

`build/v3-s3-bringup/build-evidence.json` records local paths, sizes and SHA-256 values for all exported artifacts. The **16 MB merged image is not a complete speech-model deployment**: its model partition (`0xd10000`–`0xfeffff`, 3,014,656 bytes) is all `0xff`. The core separately generated `build/v3-s3-bringup/compile-cache/srmodels.bin`, **2,468,362 bytes**, SHA-256 `0312f2dde9581cd604e752fbfa287d687a2acc0631e593a35a24c4a518d75879`. The installed core's board recipe supplies that separate image at `0xd10000` during a normal speech-profile upload. It is retained but **not written**, merged or assumed unnecessary. Do not approve an application-only/merged-only upload on the strength of compilation.

The actual built partition table was decoded and retained in `build-partitions.json`: NVS `0x9000/0x5000`, OTA metadata `0xe000/0x2000`, app0 `0x10000/0x300000`, app1 `0x310000/0x300000`, SPIFFS `0x610000/0x700000`, model `0xd10000/0x2e0000`, coredump `0xff0000/0x10000`. It differs materially from the preserved original image.

Core 3.3.2 also contains a named `waveshare_esp32_s3_touch_lcd_185` board definition with its own pin variant and defaults (including a 120 MHz flash-frequency property). Its existence and configured USB product ID do not identify the attached physical board. The successful command deliberately documents the explicit generic S3 settings actually tested, not a claim that the named preset, its defaults or every hardware revision has been verified.

## Earlier minimal display/touch foundation — v3-bringup-0.1.0

The owned source is `trailguide_v3_bringup/`. The earlier v0.1.0 preparation validated the pinned ZIP and prepared only the five required driver pairs, ST77916 component pair and LVGL 8.3.10 under ignored `build/v3-s3-bringup/minimal/`. The current helper instead targets the separate `imu/` increment described below. No manufacturer source is published or globally installed. Original archive/extraction remain unchanged.

The UI has a dark 360x360 screen, centered white/amber text, a 160x64 **Tap target**, **Taps: 0**, and the version. Text, button and focus bounds are tested inside radius 168. A clicked event increments once; there is no hold autorepeat, storage, network, audio, microphone, SD, speech or IMU initialization. Synthetic host pointer events are not finger tests; no synthetic taps were injected on the device.

### Reviewed DMA ownership correction

Review blocked the first candidate because the vendor startup color-bar routine rewrites and frees a buffer while queued DMA may still use it. The normal vendor LVGL flush path also returned before color transfer completion. Preparation now applies exactly four guarded substitutions to one derived `Display_ST77916.cpp`: omit the startup color-bar invocation; retain the actual panel IO handle; check panel creation; check each window draw and synchronously drain pending color transfers before returning its LVGL buffer. All panel register tables, resets, other eleven drivers and 898 LVGL files remain byte-identical to the pinned archive.

The installed SDK documents the queue-draining behavior of `esp_lcd_panel_io_tx_param`; the matching IDF SPI implementation confirms `(-1, NULL, 0)` drains without transmitting a command or parameter. Exact prepared-source comparison and a compiled test of the actual window function against a queued-DMA seam went RED then GREEN, including 100 transfers. This is a buffer-ordering test, not measured physical DMA timing. The bounded correction passed independent re-review before the first write.

### Earlier v0.1.0 software/build evidence

The corrected first target build exited zero: 603,687 application bytes, 126,424 globals, exported application 603,840 bytes. S3 headers/checksums/hashes, partition bounds and merged components were verified. Actual LVGL 8.3.10 ASan/UBSan checks exercised rendered copy/geometry, pointer press/hold/release, 100 clicks, cancellation and heartbeat rollover. These remain earlier evidence, not the current candidate. The current preparation helper uses a separate `imu/` output and rejects fallback `minimal/`; use the current recipe below rather than overwriting v0.1.0 evidence.

### Earlier installation and physical confirmation

The first v0.1.0 deployment verified bootloader `0x0`, partitions `0x8000`, OTA initialization `0xe000` and application `0x10000`. Startup and continuing versioned heartbeats were captured; both post-reset expander/touch ACKs were one. The visible counter and basic inside/outside target were subsequently confirmed by the physical operator, including an additional 100 taps. This is not full-screen calibration, hold/drag or soak acceptance.

Native USB monitor connection can itself produce `USB_UART_CHIP_RESET`, even with DTR/RTS false before opening; a later post-reset zero counter was inconclusive for earlier taps. Keep first-boot captures separate from human tap observations and do not reset an active check unnecessarily. Raw logs, compiled payloads and original recovery images remain private.

## Earlier optional IMU test — v3-bringup-0.1.1

The existing button/counter/fonts remain, with a circle-safe `IMU: ...` status. The owned `imu.h` probes only `0x6a`/`0x6b`, gates configuration on WHO_AM_I at register `0x00`, and reports revision register `0x01` separately. Unknown/missing identity disables IMU configuration without blocking display/touch. Bounded reads propagate failures and short transfers; no guessed software reset, interrupt/FIFO/motion engine, audio, microphone, RTC, SD, Wi-Fi, storage or recording is enabled.

Original-family source supplies the configuration/range/register behavior; locally hash-pinned supplementary manufacturer-distributed code corroborates WHO/STATUS0 meanings. The original example labels revision as Device ID; the wrapper does not repeat that error. Local `source-provenance.json` records exact source hashes and interpretation. Rev2.1 schematic naming and WHO_AM_I do not establish the actual A/C sensor suffix.

The loop uses at most two register operations per initialization tick, 10ms shared-Wire timeout, 250ms polling and five-second heartbeat/sample output. Configuration is read back: 4g accelerometer range, 512dps gyro range, normal 60 ODR enums, LPF/attitude engine off. Samples use signed little-endian native sensor axes; missing/stale/short/all-zero/all-ff/clipped frames suppress numbers. These safeguards are not calibration or proof of every hardware-fault case.

### Diagnostic build and checks

Preparation leaves the preserved `minimal/` fallback untouched; all derived sources/libraries/config and outputs stay in ignored `imu/`. It retains the same four reviewed DMA substitutions and byte-identical display/touch drivers and LVGL. No global dependency changes are required.

```sh
umask 077
mkdir -p build/v3-s3-bringup/imu
python3 tools/prepare_v3_bringup.py > build/v3-s3-bringup/imu/preparation-output.json
ROOT="$PWD/build/v3-s3-bringup/imu"
FQBN='esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
FLAGS="-I$ROOT/config -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH=$ROOT/config/lv_conf.h"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$ROOT/libraries" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-path "$ROOT/compile-cache" --output-dir "$ROOT/firmware" \
  "$ROOT/prepared/trailguide_v3_bringup"
python3 -m unittest discover -s tests -p '*_test.py' -v
python3 tools/test_v3_ui.py
```

Target compilation exited zero: **605,811 application bytes, 126,488 globals**, exported application **605,952 bytes**. Both S3 image headers/checksums/hashes and merged components were verified; bootloader, partitions and OTA initialization are byte-identical to v0.1.0. Thirty-four Python regressions passed. Actual bundled LVGL 8.3.10 ASan/UBSan preserved button/counter behavior and checked all ten status labels inside radius 168. The owned driver passed explicitly host-only fake-bus tests for identity, configuration/readback, failures/short reads, signed axes, invalid samples and timing/wrap. Independent review passed. These are distinct from the hardware evidence below.

### Actual update and physical-bus evidence

Fresh port ownership, S3/16MB identity matching the original target, full recovery-image hashes and exact source/package hashes were checked. Since bootloader/partitions/OTA initialization were unchanged, pinned esptool 4.11 wrote **only application0 at `0x10000`** and verified its data hash:

```sh
python -m esptool --chip esp32s3 --port PORT --baud 921600 \
  --before usb_reset --after hard_reset write_flash \
  --flash_mode keep --flash_freq keep --flash_size keep \
  0x10000 "$ROOT/firmware/trailguide_v3_bringup.ino.bin"
```

`PORT` must be freshly identified, not copied from another session. Serial connection/reset capture confirmed **v3-bringup-0.1.1**, expander/touch ACKs, initialized UI and continuing heartbeats. The actual IMU responded at **`0x6b`, WHO_AM_I=`0x05`, revision=`0x7c`**, with configuration readback `ok`; `0x6a` did not yield an identity. Four fresh physical-bus samples had acceleration magnitudes **1.0029–1.0040g** and no reported sampling read errors. These readings are consistent with gravity, not controlled motion/calibration evidence; the gyro is explicitly uncalibrated and shows bias. No fabricated sensor values or synthetic on-device taps were used.

Exact payload/source hashes, source provenance, commands, upload and raw serial evidence remain private under `imu/`. The prebuilt core descriptor still carries core-builder metadata; UI/serial markers identify the actual application version. Core 3.3.2 runtime has been exercised for this minimal subset, not manufacturer-certified for all peripherals. No PSRAM health test, native-to-screen axis mapping, measured shake threshold, motion/rest/cooldown handling, card draw or authenticated V3 OTA service is implemented.

## Remaining gates

1. Use the photo-confirmed original Rev2.1 model and its schematic. Do not request the same model photo again; retain separate checks for populated sensor IDs, runtime PSRAM, visible pixels and physical touch.
2. Keep both original recovery images immutable and verify any later replacement's target, hashes, partitions and actual authorization. Restoration of the original whole image has not been tested.
3. If a manufacturer-version build is needed, isolate a compatible core under the ignored local build tree only; do not replace the host's global core or libraries. Do not silently patch the unmodified reference to hide compatibility failures.
4. Boot/serial verification and the basic physical visible-counter/touch-target check passed for the minimal image. An additional 100 taps and rejection of outside-button taps were operator-reported. Full display/readability, hold/drag/screen-edge mapping, runtime PSRAM, controlled IMU motion/axes/noise, memory stability and power behavior remain separate gates. Actual WHO_AM_I/configuration and fresh samples are verified for v0.1.1, not shake-to-draw. No shake behavior is claimed.
5. The first V3-specific offline application and experimental gesture/reveal path are now installed. Validate them physically and tune handling from evidence before production claims. Authenticated V3 OTA, durable recording, production eligibility and full power/memory acceptance remain separate; manufacturer/browser studies are not substitutes.

## Sources

[1] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85
[3] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85/Arduino
[4] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85.pdf
[5] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85_V2.1.pdf
