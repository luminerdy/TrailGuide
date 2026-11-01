#!/usr/bin/env python3
"""Retain the verified manufacturer's panel sequence locally, not vendored in Git."""
from pathlib import Path
import hashlib
import zipfile

ROOT = Path(__file__).resolve().parents[1]
SHA256 = 'ad8e27b172035fb73b5dbe88b821b1ff37bd677c20db294a9da7e5317ee176dd'

def extract(source):
    start = 'void lcd_reg_init(void) {'
    end = '\nuint32_t screenWidth'
    if source.count(start) != 1 or source.count(end) != 1:
        raise ValueError('Unrecognized manufacturer panel source; inspect before adapting')
    function = source[source.index(start):source.index(end)].strip()
    if not function.endswith('}'):
        raise ValueError('Incomplete manufacturer panel initialization')
    return '// Local manufacturer panel sequence; see docs/V2-BRINGUP.md.\n#pragma once\n' + function + '\n'

def main():
    archive = ROOT/'build/v2-c6-bringup/vendor/ESP32-C6-Touch-LCD-1.47-Demo.zip'
    if hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        raise SystemExit('Manufacturer archive changed; review before use')
    with zipfile.ZipFile(archive) as data:
        source = data.read('Arduino/examples/04_lvgl_arduino_v8/04_lvgl_arduino_v8.ino').decode().replace('\r\n','\n')
    destination = ROOT/'build/v2-0.1.0/generated/vendor_panel_init.h'
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(extract(source))
    print('Prepared checksum-verified local panel initialization.')

if __name__ == '__main__':
    main()
