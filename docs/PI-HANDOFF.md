# Raspberry Pi handoff

## Current installed release — v3-0.1.31-mode-switch

**Installed DRAFT source: `v3-0.1.31-mode-switch`.** Classic main cards and Guide now offer an **opposite-mode destination button**: **Try** while reading Ask, **Ask** while reading Try. Either draws one independent card from the other mode and returns to its main screen—not the matching identity. Another and intentional gestures retain the newly selected mode. Ordinary Guide/Back preserves the current identity. Main controls are Home / Try or Ask / conditional More text; Guide controls are Back / Try or Ask / conditional More text. There is no passive Ask label.

Coaching is unchanged: its **Try (optional)** / **Question** controls remain distinct from Classic's independent-deck shortcut. Words, 20/18px fonts, six-second reveal, separate two-second settled reading hold, motion/sensor policy and shared boot-OFF sound are unchanged from28. Classic still boots by default. See the [current release contract](RELEASE-v3-0.1.31.md), [Coaching display guide](COACHING-DISPLAY-GUIDE.md) and [printable Coaching guide](COACHING-DISPLAY-GUIDE.pdf).

Original31 native/SDK and installation evidence is **reused**, not newly built by this publication. Qualitative approval of the new flow is limited to one unit, not exhaustive gesture/readability acceptance. **OPEN:** inherited Coaching compass/path automatic-shake gaps and the original turnover-stuck-compass/blank cause. No gesture repair is claimed. [Frozen28](BASELINE-v3-0.1.28.md) remains a separate historical baseline; its tag and manifest are preserved. Private device identifiers, binaries and raw evidence are excluded.

This is the original V1 host-setup guide, not the current C6 development checkpoint. Start with the [project and coaching handoff](HANDOFF.md) and [verification record](BRINGUP.md). Do not apply these V1 OLED/board commands to V2.

## Human setup
1. Finish Raspberry Pi OS setup and connect to the network.
2. Enable SSH using Pi settings or Imager configuration.
3. Provide hostname/IP and username to an agent session with shell access. Do not commit passwords or private keys.
4. Connect the ESP32 by USB data cable.
5. Wire the OLED per V1 spec with USB unplugged, then reconnect.

The agent needs an actual shell/SSH route. Cloning this repository or moving a conversation does not grant remote access.

## Agent setup
Inspect OS/CPU architecture and installed tools. Install Git and Arduino CLI from official sources. Configure Espressif's package index, install the ESP32 core and Adafruit SH110X/GFX/BusIO dependencies for the SH1106 OLED. Record versions. Check serial permissions; use the appropriate serial group and reconnect if needed, not world-writable device nodes.

Use `arduino-cli board list` to identify the port. V1 target is `esp32:esp32:esp32`. Never flash a guessed serial device.

After dependencies are installed, the intended commands from the repo root are:

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 oled_oracle
arduino-cli upload --fqbn esp32:esp32:esp32 --port <detected-port> oled_oracle
arduino-cli monitor --port <detected-port> --config baudrate=115200
```

These commands are a starting point, not a record of successful execution.

## Verification record (pending)
- Pi OS/architecture:
- Arduino CLI:
- ESP32 core:
- SH110X / GFX / BusIO:
- Board and serial port:
- Compile command/result:
- Upload command/result:
- Hardware acceptance checks:
