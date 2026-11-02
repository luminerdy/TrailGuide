#!/usr/bin/env python3
"""Exercise the actual portrait LVGL UI on the host, without hardware stubs."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import hashlib
import subprocess

ROOT = Path(__file__).resolve().parents[1]
LVGL = ROOT / 'build/v2-c6-bringup/vendor/Arduino/libraries/lvgl'
OUT = ROOT / 'build/v2-0.1.8/host'
OUT.mkdir(parents=True, exist_ok=True)
flags = ['-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-DLV_CONF_INCLUDE_SIMPLE', '-I'+str(ROOT/'trailguide_v2'), '-I'+str(LVGL)]
config = hashlib.sha256((ROOT/'trailguide_v2/lv_conf.h').read_bytes()).hexdigest()[:12]

def compile_source(source):
    name = config + '-' + hashlib.sha256(str(source).encode()).hexdigest()[:16] + '.o'
    previous = ROOT / 'build/v2-0.1.7/host' / name
    if previous.exists() and source.stat().st_mtime <= previous.stat().st_mtime:
        return str(previous) # Read-only reuse only when config/source truly match.
    obj = OUT / name
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
# Compile the actual target's diagnostic against REAL host LVGL as well, so its
# skip logic is exercised under both animated and allocation-fallback branches.
sketch = (ROOT/'trailguide_v2/trailguide_v2.ino').read_text()
start = sketch.index('static void selfTest() {')
end = sketch.index('\n}\n\nvoid setup()', start) + 2
(OUT/'native_selftest.h').write_text(sketch[start:end] + '\n')
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-I'+str(OUT), *flags, str(ROOT/'tests/v2_ui_test.cpp'), *objects, '-lm', '-Wl,--wrap=malloc,--wrap=free', '-o', str(exe)], check=True)
subprocess.run([str(exe)], cwd=ROOT, check=True, timeout=30)
from PIL import Image, ImageDraw
ARTIFACTS = ROOT / 'build/v2-0.1.8'
for ppm in ARTIFACTS.glob('*.ppm'):
    if not ppm.stem.startswith('motion-'):
        Image.open(ppm).save(ppm.with_suffix('.png'))
sheet = Image.new('RGB', (344 * 6, 672), '#101c24')
caption = ImageDraw.Draw(sheet)
for col, frame in enumerate([0, 42, 94, 126, 149, 150]):
    image = Image.open(ARTIFACTS / f'motion-{frame:03d}.ppm')
    sheet.paste(image.resize((344, 640), Image.Resampling.NEAREST), (344 * col, 24))
    caption.text((344 * col + 8, 5), f'{frame * 40} ms', fill='white')
sheet.save(ARTIFACTS / 'native-contact-sheet.png')
all_cards = Image.new('RGB', (172 * 8, 320 * 2))
for row, mode in enumerate(['ask', 'try']):
    for card in range(8):
        all_cards.paste(Image.open(ARTIFACTS / f'{mode}-{card}-final.ppm'), (172 * card, 320 * row))
all_cards.save(ARTIFACTS / 'native-all-cards.png')
subprocess.run(['ffmpeg', '-v', 'error', '-framerate', '25', '-i', str(ARTIFACTS/'motion-%03d.ppm'),
                '-vf', 'scale=516:960:flags=neighbor', '-c:v', 'libx264', '-threads', '2',
                '-pix_fmt', 'yuv420p', '-y', str(ARTIFACTS/'native-six-seconds.mp4')], check=True)
print('PASS: real LVGL8.4 ASan/UBSan, six-second projected blue-face/word approach, exact keyframes/dark beat/settled handoff; both modes/all eight cards, complete copy/font20/scroll, same-card skip, instant Guide/Back, real pointer hold/release/drag/OTA, rollover, teardown, 100-cycle heap stability, caller allocation failure and low-LVGL-heap fallback. Native PNG/PPM/contact sheets/MP4 exported privately in build/v2-0.1.8/.')
