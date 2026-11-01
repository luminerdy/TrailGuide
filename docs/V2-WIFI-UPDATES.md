# V2 Wi-Fi and OTA operation

Status: implemented and hardware-tested on the portrait ESP32-C6. USB installed the v2-0.1.2 bootstrap; authenticated network updates then installed v2-0.1.3 and v2-0.1.4. Subsequent coaching-copy v2-0.1.5 and gradual-reveal v2-0.1.6 also installed wirelessly. The current v2-0.1.6 running version, changed application slot, continued runtime and OTA readiness were read back remotely. These are network transfers, not USB uploads disguised as OTA.

## Normal use

- V2 needs power, but not a USB data connection. A battery/power bank is usable if it supplies the board correctly; battery runtime and charging acceptance remain separate.
- The device joins its configured protected Wi-Fi in station mode. The development host must be on the same reachable trusted LAN. Internet access is not required; this is not an internet/cloud update service.
- Home shows `Draft | WiFi/OTA` when the update listener is ready, or `Draft | Offline` otherwise. Cards and Guide me remain usable without Wi-Fi. Connection and reconnect attempts run in the background.
- During an actual transfer, a full-screen Updating/Keep power connected notice blocks card actions. The pinned core's transfer handler is synchronous, so normal interaction pauses until transfer success/failure. Do not remove power while Updating.
- Replacements must retain OTA support, compatible private credentials and the existing partitions. USB recovery remains the fallback; do not change partition tables over OTA.

## Private configuration

Use `trailguide_v2/secrets.h`, ignored by Git and mode 600. A tracked [example](../trailguide_v2/secrets.example.h) contains placeholders only. The actual local file contains Wi-Fi SSID/password and a separate random V2 OTA password, at least sixteen characters long and different from the Wi-Fi password. Neither the actual file nor credential-bearing binaries/logs belong in GitHub or chat.

The first setup reused the available local Wi-Fi settings and generated a distinct V2 update password. No credentials were printed. Keep that file available privately for future builds/uploads. Without configuration, or with `TRAILGUIDE_DISABLE_WIFI`, updates stay disabled; guidance remains available.

Authentication uses the [pinned Espressif Arduino core 3.3.2 implementation](https://github.com/espressif/arduino-esp32/blob/3.3.2/libraries/ArduinoOTA/src/ArduinoOTA.cpp) and its matching uploader. This is authenticated local updating, **not encrypted transport or signed-firmware/secure-boot deployment**. Use only a trusted LAN; do not port-forward the listener or status endpoint to the internet.

## Build and update

Build the C6 application with the dependencies/commands in [V2-TRIAL.md](V2-TRIAL.md). `trailguide_v2/version.h` is the source version; numeric `TRAILGUIDE_PATCH_VERSION` build overrides support explicit bench versions without fragile escaped string flags. The accepted layout remains `FlashSize=8M,PartitionScheme=default_8MB`, with application slots at `0x10000` and `0x340000`, each `0x330000` bytes.

Once the first OTA-enabled firmware is installed over USB:

```sh
python3 tools/v2_ota_upload.py DEVICE_IP \
  build/v2-0.1.6/firmware/trailguide_v2.ino.bin \
  --expected-version v2-0.1.6
```

Replace DEVICE_IP with the independently identified private address, not a public host. The helper reads the private V2 configuration by default; `--secrets PATH` selects another restricted local configuration. No password appears in shell history or process arguments.

The helper rejects non-C6 images, bootloader/merged images and files exceeding the current application-slot budget before transferring. It uses only the application `.ino.bin`. Successful transfer alone is not success: it waits for the read-only status endpoint to report the expected running version and OTA readiness. Failed confirmation is reported honestly.

The ESP32 listens on UDP 3232. It connects back to a temporary TCP listener on the development host. By default the V2 helper selects an available ephemeral callback port, avoiding TCP TIME_WAIT conflicts from rapid successive uploads. If firewall rules require a specific port/interface, use `--host HOST_IP --host-port PORT` and allow that callback; a recently used fixed port can still be unavailable. The legacy V1 uploader is unchanged.

## Remote verification and privacy

`GET http://DEVICE_IP/status` returns only device type, firmware version, OTA-ready state, uptime and active application-slot address. It is read-only, unauthenticated metadata for a trusted LAN; it does not expose Wi-Fi credentials, update passwords, cards, team data or an HTTP upload/control API. The helper bypasses proxy settings for this local request. Private addresses and raw diagnostics remain local.

## Executed acceptance checks

- USB bootstrap compiled/flashed with matching 8MB partitions and verified upload hashes; original factory backups and earlier known-working images retained privately.
- Wi-Fi disconnect was induced only in the temporary bench bootstrap. A synthetic on-device Ask/Try/Guide diagnostic passed while offline; automatic retry restored the listener and remote status endpoint.
- An incorrect OTA password was rejected without replacing the running version or active application slot.
- An authenticated transfer was intentionally interrupted after at least 10%. The existing application/slot continued and the listener/status recovered. This is partial-transfer recovery, not an automatic-rollback or power-loss guarantee.
- A network update installed v2-0.1.3 in `0x340000`; another installed v2-0.1.4 in `0x10000`. Both running versions and OTA readiness were read back over the network. The final image excludes the temporary USB disconnect test hook.
- A second upload attempt initially hit a local fixed-port TIME_WAIT bind failure before transfer. The ephemeral callback selection fixed it, with a regression test and successful final network upload; this was not a device update failure.
- Real-LVGL ASan/UBSan tests cover the updating overlay, suppressed actions and preserved Guide context on dismissal, along with all eight pairs/modes. Network-controller tests cover retry/reconnect, lifecycle/error notifications, rollover and disabled configuration. Host passing is not a claim of new human touch/visibility acceptance.
- The v2-0.1.6 reveal increment compiled with unchanged partitions and installed over authenticated OTA, changing the active application from 0x340000 to 0x10000. Running version/readiness and subsequent increasing uptime were read back. Host tests also cover OTA beginning during a reveal: cancel its timer, preserve complete current copy and restore context on error dismissal. This is not new physical pacing/touch acceptance or a repeated interrupted-transfer test.

No automatic rollback is implemented or claimed. Interrupted-power recovery, prolonged Wi-Fi soak, independent-power endurance and human review of the new updating screen remain separate acceptance work. V1 was neither reset nor flashed for this implementation.
