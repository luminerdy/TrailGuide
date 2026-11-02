#!/usr/bin/env python3
"""Compile and exercise the actual target-bundled LVGL 8.3.10; no hardware."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import hashlib
import subprocess
from prepare_v3_bringup import prepare

ROOT = Path(__file__).resolve().parents[1]
LVGL = ROOT / 'build/v3-s3-bringup/minimal/libraries/lvgl'
OUT = ROOT / 'build/v3-s3-bringup/minimal/host'


def main():
    prepare()  # Host and S3 builds consume the same archive-pinned, unchanged library.
    assert (ROOT/'trailguide_v3_bringup/ui.h').is_file(), 'Missing minimal V3 UI'
    OUT.mkdir(parents=True, exist_ok=True)
    flags = ['-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
             '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_CONF_PATH='+str(ROOT/'trailguide_v3_bringup/lv_conf.h'),
             '-I'+str(ROOT/'trailguide_v3_bringup'), '-I'+str(LVGL)]
    config = hashlib.sha256((ROOT/'trailguide_v3_bringup/lv_conf.h').read_bytes() + repr(flags).encode()).hexdigest()[:12]

    def compile_source(source):
        obj = OUT / (config + '-' + hashlib.sha256(str(source).encode()).hexdigest()[:16] + '.o')
        if not obj.exists() or source.stat().st_mtime > obj.stat().st_mtime:
            subprocess.run(['gcc', '-std=c99', *flags, '-c', str(source), '-o', str(obj)], check=True, capture_output=True)
        return str(obj)

    sources = sorted((LVGL/'src').rglob('*.c'))
    assert sources, 'Missing pinned bundled LVGL sources'
    with ThreadPoolExecutor(max_workers=2) as pool:
        objects = list(pool.map(compile_source, sources))
    exe = OUT/'v3-ui-test'
    subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags,
                    str(ROOT/'tests/v3_ui_test.cpp'), *objects, '-lm', '-o', str(exe)], check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True, timeout=30)
    print('PASS: actual target-bundled LVGL 8.3.10; ASan/UBSan enabled; synthetic host input only.')


if __name__ == '__main__':
    main()
