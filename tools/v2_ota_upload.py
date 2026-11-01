#!/usr/bin/env python3
"""Upload a C6 application using private credentials, then verify its running version."""
import argparse
import importlib.util
import ipaddress
import json
from pathlib import Path
import re
import socket
import stat
import struct
import time
import urllib.request

ROOT = Path(__file__).resolve().parents[1]


def validate_image(image):
    if not 256 <= len(image) <= 0x330000:
        raise ValueError('Application does not fit the existing C6 OTA slot')
    if image[0] != 0xe9 or struct.unpack_from('<H', image, 12)[0] != 13:
        raise ValueError('Expected an ESP32-C6 image, never a V1/S3 binary')
    if struct.unpack_from('<I', image, 32)[0] != 0xabcd5432:
        raise ValueError('Expected an application image, not a bootloader or merged flash dump')


def read_password(path):
    if not stat.S_ISREG(path.stat().st_mode) or path.stat().st_mode & 0o077:
        raise ValueError('Private configuration must be a regular file with mode 600')
    match = re.search(r'^\s*#define\s+TRAILGUIDE_OTA_PASSWORD\s+("(?:[^"\\]|\\.)*")\s*$', path.read_text(), re.M)
    if not match:
        raise ValueError('Private configuration has no OTA password definition')
    password = json.loads(match.group(1))
    if len(password) < 16:
        raise ValueError('OTA password must have at least sixteen characters')
    return password


def callback_port(host):
    """Choose a free Pi callback port; avoid an earlier transfer's TIME_WAIT."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as listener:
        listener.bind((host, 0))
        return listener.getsockname()[1]


def uploader():
    path = Path.home()/'.arduino15/packages/esp32/hardware/esp32/3.3.2/tools/espota.py'
    spec = importlib.util.spec_from_file_location('espota_v2', path)
    if spec is None or spec.loader is None:
        raise RuntimeError('Pinned ESP32 core uploader unavailable')
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    setattr(mod, 'PROGRESS', False)
    setattr(mod, 'TIMEOUT', 10)
    return mod


def get_status(address, timeout=3):
    # Direct local access; never route device metadata through configured proxies.
    request = urllib.request.Request('http://' + str(ipaddress.IPv4Address(address)) + '/status')
    with urllib.request.build_opener(urllib.request.ProxyHandler({})).open(request, timeout=timeout) as response:
        data = json.loads(response.read(1024))
    if not isinstance(data, dict) or data.get('device') != 'trailguide-v2':
        raise ValueError('Status target is not TrailGuide V2')
    return data


def wait_version(address, expected, timeout=45):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        try:
            data = get_status(address)
            if data.get('version') == expected and data.get('ota_ready') is True:
                return data
        except (OSError, ValueError):
            pass
        time.sleep(1)
    raise RuntimeError('Could not confirm expected running version and OTA readiness')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('address', type=ipaddress.IPv4Address, help='C6 direct local IPv4 address')
    parser.add_argument('firmware', type=Path, help='C6 application .ino.bin only')
    parser.add_argument('--expected-version', required=True)
    parser.add_argument('--secrets', type=Path, default=ROOT/'trailguide_v2/secrets.h')
    parser.add_argument('--host', default='0.0.0.0', type=ipaddress.IPv4Address)
    parser.add_argument('--host-port', type=int, default=0, help='Pi callback TCP port; 0 selects a free ephemeral port')
    args = parser.parse_args()
    if not 0 <= args.host_port <= 65535:
        parser.error('Host port must be 0 through 65535')
    try:
        validate_image(args.firmware.read_bytes())
        password = read_password(args.secrets)
        if get_status(str(args.address)).get('ota_ready') is not True:
            raise ValueError('C6 update listener is not ready')
        local_port = args.host_port or callback_port(str(args.host))
        result = uploader().serve(str(args.address), str(args.host), 3232, local_port, password, False, str(args.firmware))
        if result != 0:
            raise RuntimeError('Wireless transfer failed; do not assume the image was installed')
        data = wait_version(str(args.address), args.expected_version)
        print('Verified running version:', data['version'], '; OTA ready; application slot:', data['slot'])
    except (OSError, ValueError, RuntimeError) as error:
        parser.exit(1, 'V2 OTA failed: ' + str(error) + '\n')

if __name__ == '__main__':
    main()
