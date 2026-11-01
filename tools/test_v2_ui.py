#!/usr/bin/env python3
"""Exercise the actual portrait LVGL UI on the host, without hardware stubs."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import hashlib
import subprocess

ROOT = Path(__file__).resolve().parents[1]
LVGL = ROOT / 'build/v2-c6-bringup/vendor/Arduino/libraries/lvgl'
OUT = ROOT / 'build/v2-0.1.7/host'
OUT.mkdir(parents=True, exist_ok=True)
flags = ['-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-DLV_CONF_INCLUDE_SIMPLE', '-I'+str(ROOT/'trailguide_v2'), '-I'+str(LVGL)]
config = hashlib.sha256((ROOT/'trailguide_v2/lv_conf.h').read_bytes()).hexdigest()[:12]

def compile_source(source):
    obj = OUT / (config + '-' + hashlib.sha256(str(source).encode()).hexdigest()[:16] + '.o')
    if not obj.exists() or source.stat().st_mtime > obj.stat().st_mtime:
        result = subprocess.run(['gcc', '-std=c99', *flags, '-c', str(source), '-o', str(obj)], capture_output=True, text=True)
        if result.returncode:
            raise RuntimeError(result.stderr)
    return str(obj)

sources = sorted((LVGL/'src').rglob('*.c'))
if not sources:
    raise SystemExit('Manufacturer LVGL sources missing; see docs/V2-BRINGUP.md')
with ThreadPoolExecutor(max_workers=2) as pool:
    objects = list(pool.map(compile_source, sources))
exe = OUT/'v2-ui-test'
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags, str(ROOT/'tests/v2_ui_test.cpp'), *objects, '-lm', '-o', str(exe)], check=True)
subprocess.run([str(exe)], cwd=ROOT, check=True, timeout=30)
print('PASS: actual LVGL, both modes/all eight cards, whole-answer opacity fade with fixed complete text/layout and changing rendered pixels, 2960ms completion, same-card Reveal all, instant Guide/Back, cancellation/OTA/teardown, UTF-8/long text, scrolling and button bounds; ASan/UBSan enabled.')
