#!/usr/bin/env python3
"""Host-only regression: compile the pinned LOCAL HWCDC write verbatim.

Synthetic RTOS/ringbuffer/USB surfaces; never opens a device or compiles S3.
"""
from pathlib import Path
import hashlib
import json
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/v3-0.1.6/host-budget'
SDK = Path.home()/'.arduino15/packages/esp32'
DRIVER = SDK/'hardware/esp32/3.3.2/cores/esp32/HWCDC.cpp'
# Pin the complete implementation, not a hand-reimplemented retry model.
DRIVER_SHA256 = '8238d208409ac8a9e74e5960360a5e59c4f2f85dc2339fd96442a62297d5ff45'

def main():
    OUT.mkdir(parents=True, exist_ok=True)
    raw = DRIVER.read_bytes()
    assert hashlib.sha256(raw).hexdigest() == DRIVER_SHA256, 'Unknown local HWCDC source'
    source = raw.decode()
    start = source.index('size_t HWCDC::write(const uint8_t *buffer, size_t size) {')
    end = source.index('\nsize_t HWCDC::write(uint8_t c)', start)
    method = source[start:end]
    assert method.count('uint32_t tries = tx_timeout_ms;') == 1
    assert method.count('tries--;') == 1
    (OUT/'hwcdc_write_generated.h').write_text(method.replace('HWCDC::write', 'Transport::write', 1))
    sketch = (ROOT/'trailguide_v3/trailguide_v3.ino').read_text()
    start = sketch.index('  Serial.begin(115200);')
    end = sketch.index('  I2C_Init();', start)
    (OUT/'budget_setup_generated.h').write_text(sketch[start:end])
    start = sketch.index('  if (uint32_t(now-last_heartbeat)>=5000)')
    end = sketch.index('\n  delay(5);', start)
    (OUT/'startup_heartbeat_generated.h').write_text(sketch[start:end])
    configs = sorted((SDK/'tools/esp32-arduino-libs').glob('*/esp32s3/*/include/sdkconfig.h'))
    assert configs
    for config in configs:
        assert '#define CONFIG_FREERTOS_HZ 1000\n' in config.read_text(), str(config)
    print('LOCAL SDK: Arduino 3.3.2 HWCDC sha256='+DRIVER_SHA256+'; all S3 sdkconfig variants CONFIG_FREERTOS_HZ=1000', flush=True)
    binary = OUT/'hwcdc-budget-test'
    subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-O0','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(OUT),str(ROOT/'tests/v3_hwcdc_budget_test.cpp'),'-o',str(binary)], check=True)
    if not sys.argv[1:]:
        negative = subprocess.run([str(binary), '--zero'], capture_output=True, text=True, timeout=30)
        assert negative.returncode == 1 and 'EXPECTED RED: iteration guard' in negative.stdout, negative.stdout + negative.stderr
        print(negative.stdout, end='', flush=True)
    result = subprocess.run([str(binary), *sys.argv[1:]], timeout=30)
    (OUT/'driver-manifest.json').write_text(json.dumps({'driver':str(DRIVER),'sha256':DRIVER_SHA256,'method_sha256':hashlib.sha256(method.encode()).hexdigest(),'tick_hz':1000,'method':'verbatim full HWCDC::write; class qualifier only renamed','hardware_verified':False},indent=2)+'\n')
    return result.returncode

if __name__ == '__main__':
    raise SystemExit(main())
