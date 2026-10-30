# Raspberry Pi handoff

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
