# V3 round S3 — display/touch bring-up

## Scope and measured identity

The original read-only checkpoint established silicon/flash identity, two original-flash backups and a host build of the manufacturer's example. The photo-confirmed **ESP32-S3-Touch-LCD-1.85 Rev2.1** now runs the independently reviewed minimal **v3-bringup-0.1.0** display/touch image. Four component writes were verified by esptool; startup diagnostics and continuing versioned heartbeats were read back after installation and a controlled reset. **The visible counter and basic touch target are now physically confirmed:** an additional 100 taps were reported, with only inside-button taps incrementing the counter. Full display/readability, hold/drag and screen-edge checks remain separate. No eFuses were written. V1/V2 are excluded; C6 drivers, pins, binaries and partitions are not reused.

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

## Installed minimal display/touch test — v3-bringup-0.1.0

The owned source is `trailguide_v3_bringup/`. `tools/prepare_v3_bringup.py` validates the pinned ZIP and prepares only the five required driver pairs, ST77916 component pair and LVGL 8.3.10 under ignored `build/v3-s3-bringup/minimal/`. No manufacturer source is published or globally installed. Original archive/extraction remain unchanged.

The UI has a dark 360x360 screen, centered white/amber text, a 160x64 **Tap target**, **Taps: 0**, and the version. Text, button and focus bounds are tested inside radius 168. A clicked event increments once; there is no hold autorepeat, storage, network, audio, microphone, SD, speech or IMU initialization. Synthetic host pointer events are not finger tests; no synthetic taps were injected on the device.

### Reviewed DMA ownership correction

Review blocked the first candidate because the vendor startup color-bar routine rewrites and frees a buffer while queued DMA may still use it. The normal vendor LVGL flush path also returned before color transfer completion. Preparation now applies exactly four guarded substitutions to one derived `Display_ST77916.cpp`: omit the startup color-bar invocation; retain the actual panel IO handle; check panel creation; check each window draw and synchronously drain pending color transfers before returning its LVGL buffer. All panel register tables, resets, other eleven drivers and 898 LVGL files remain byte-identical to the pinned archive.

The installed SDK documents the queue-draining behavior of `esp_lcd_panel_io_tx_param`; the matching IDF SPI implementation confirms `(-1, NULL, 0)` drains without transmitting a command or parameter. Exact prepared-source comparison and a compiled test of the actual window function against a queued-DMA seam went RED then GREEN, including 100 transfers. This is a buffer-ordering test, not measured physical DMA timing. The bounded correction passed independent re-review before the first write.

### Reproduce compilation and software checks

From the repository root, with the pinned archive already at the documented private path and the existing CLI on `PATH`:

```sh
umask 077
mkdir -p build/v3-s3-bringup/minimal
python3 tools/prepare_v3_bringup.py > build/v3-s3-bringup/minimal/preparation-output.json
ROOT="$PWD/build/v3-s3-bringup/minimal"
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

The installed generic S3 definition does not expose `default_16MB`; the explicitly inspected `app3M_fat9M_16MB` provides two 3 MiB application slots, no speech-model partition, and reserved filesystem space that this image does not mount. Reserved OTA slots are not an OTA service. `LV_TICK_CUSTOM=0` retains the vendor `esp_timer` tick source; the same owned font/config header is passed to C and C++.

The corrected target compilation exited zero: **603,687 application bytes**, **126,424 bytes globals**, **201,256 bytes remaining by static accounting**. The exported application is **603,840 bytes**. App/bootloader S3 headers, checksums, validation hashes, partition bounds and merged-image component bytes were verified. Thirty-two Python tests passed. Actual bundled LVGL 8.3.10 with ASan/UBSan exercised rendered geometry/copy, pointer press/hold/release, 100 clicks, drag/outside cancellation and heartbeat rollover. No static or host result establishes physical readability, runtime free memory or PSRAM health.

### Installation and serial evidence

Fresh USB/port-owner and S3/16MB checks matched the original backup target. Both complete recovery images and exact candidate hashes were checked before the write. With the pinned esptool 4.11 environment, the reviewed native-USB recipe wrote bootloader `0x0`, partitions `0x8000`, OTA initialization `0xe000`, and application `0x10000`. It did not write the all-peripheral demo or a speech model:

```sh
python -m esptool --chip esp32s3 --port PORT --baud 921600 \
  --before usb_reset --after hard_reset write_flash \
  --flash_mode keep --flash_freq keep --flash_size keep \
  0x0 "$ROOT/firmware/trailguide_v3_bringup.ino.bootloader.bin" \
  0x8000 "$ROOT/firmware/trailguide_v3_bringup.ino.partitions.bin" \
  0xe000 "$ROOT/firmware/boot_app0.bin" \
  0x10000 "$ROOT/firmware/trailguide_v3_bringup.ino.bin"
```

`PORT` must be freshly re-identified, not copied from an old session. All four writes reported data-hash verification. Serial capture read version **v3-bringup-0.1.0**, `ui_ready=1`, tap count zero and increasing uptime. Controlled native-USB reset capture then read `V3_START`, post-reset expander/touch ACKs both one, `V3_UI`, and continuing heartbeats without observed crash markers. ACKs establish responding addresses, not pixels or finger behavior. The runtime version is identified by UI/serial markers; the prebuilt core's application descriptor retains core-builder metadata.

Exact commands, prepared/original source hashes, artifact sizes/hashes, upload and serial logs remain private under `minimal/`. `build-manifest.json` and `deployment-verification.json` distinguish compiled payload from actual running-device evidence. Core 3.3.2 booted this test, but remains outside the manufacturer's published range and is not certified across all peripherals. Vendor touch read-error propagation remains weak; startup ACK gating does not simulate a later bus fault.

## Remaining gates

1. Use the photo-confirmed original Rev2.1 model and its schematic. Do not request the same model photo again; retain separate checks for populated sensor IDs, runtime PSRAM, visible pixels and physical touch.
2. Keep both original recovery images immutable and verify any later replacement's target, hashes, partitions and actual authorization. Restoration of the original whole image has not been tested.
3. If a manufacturer-version build is needed, isolate a compatible core under the ignored local build tree only; do not replace the host's global core or libraries. Do not silently patch the unmodified reference to hide compatibility failures.
4. Boot/serial verification and the basic physical visible-counter/touch-target check passed for the minimal image. An additional 100 taps and rejection of outside-button taps were operator-reported. Full display/readability, hold/drag/screen-edge mapping, runtime PSRAM, IMU identity/readings, memory stability and power behavior remain separate gates. No shake behavior is claimed.
5. Build V3-specific offline application, shake/rest/cooldown handling, reveal cancellation, touch alternative, reduced-motion behavior, authenticated update/recovery and human acceptance separately. The manufacturer demo and browser study are not production V3 firmware.

## Sources

[1] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85
[3] https://docs.waveshare.com/ESP32-S3-Touch-LCD-1.85/Arduino
[4] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85.pdf
[5] https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85/ESP32-S3-Touch-LCD-1.85_V2.1.pdf
