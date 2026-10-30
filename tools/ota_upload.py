#!/usr/bin/env python3
"""Run the pinned ESP32 uploader with an interactive password, absent from argv."""
import argparse
import getpass
import importlib.util
from pathlib import Path
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('address', help='ESP32 IP address or trailguide-v1.local')
parser.add_argument('firmware', type=Path, help='application .ino.bin, never merged firmware')
parser.add_argument('--host', default='0.0.0.0', help='local interface address')
args = parser.parse_args()
script = Path.home() / '.arduino15/packages/esp32/hardware/esp32/3.3.2/tools/espota.py'
if not script.is_file() or not args.firmware.is_file():
    parser.error('Install ESP32 core 3.3.2 and supply an existing application binary')
spec = importlib.util.spec_from_file_location('espota', script)
espota = importlib.util.module_from_spec(spec)
spec.loader.exec_module(espota)
espota.PROGRESS = False
espota.TIMEOUT = 10
password = getpass.getpass('OTA password: ')
if not password:
    parser.error('An OTA password is required')
sys.exit(espota.serve(args.address, args.host, 3232, 3232, password, False, str(args.firmware)))
