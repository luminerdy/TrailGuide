#!/usr/bin/env python3
"""Focused synthetic range checks; no target build or hardware.

Current-config native objects; full host fallback if absent. Writes only in 1.13.
"""
from pathlib import Path
import hashlib
import json
import subprocess
import sys
import v3_native_objects as native

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/v3-0.1.21/focused'
BASE = ROOT/'build/v3-0.1.9'
LVBASE = ROOT/'build/v3-0.1.10'
NAMES = ('imu', 'gesture', 'imu_equivalence', 'motion_integration', 'model', 'motion', 'recorder', 'recovery', 'ui', 'turnover')

def main():
    names = sys.argv[1:] or list(NAMES)
    assert all(n in NAMES for n in names), names
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT.parent/'frames').mkdir(exist_ok=True)
    (OUT.parent/'turnover-frames').mkdir(exist_ok=True)
    inventory = json.loads((ROOT/'build/v3-motion-investigation/installed-v3-0.1.9-files.private.json').read_text())
    def verified(path):
        assert hashlib.sha256(path.read_bytes()).hexdigest() == inventory[str(path.relative_to(BASE))], path
        return path
    for name, namespace in (('imu', 'v3'), ('gesture', 'v3app')):
        source = verified(BASE/f'prepared/trailguide_v3/{name}.h').read_text()
        if name == 'imu':
            source = source.replace('namespace v3 {', 'namespace baseline {')
        else:
            source = source.replace('#include "imu.h"', '#include "imu_baseline_generated.h"').replace('namespace v3app {', 'namespace baselineapp {').replace('v3::', 'baseline::')
        (OUT/f'{name}_baseline_generated.h').write_text(source)
    sketch = (ROOT/'trailguide_v3/trailguide_v3.ino').read_text()
    start = sketch.index('  motionDiagnostics.service(Serial,now);')
    end = sketch.index('    if (!motionDiagnostics.active() && !motionDiagnostics.draining() && before == v3::ImuState::Probing', start)
    (OUT/'startup_motion_generated.h').write_text(sketch[start:end]+'    (void)before;\n  }\n')
    flags = ['-std=c++17', '-Wall', '-Wextra', '-Werror', '-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-I'+str(OUT)]
    for name in names:
        objects = []
        extra = []
        if name in ('motion_integration', 'recovery', 'ui', 'turnover'):
            lv_inventory = json.loads((ROOT/'build/v3-motion-investigation/installed-v3-0.1.10-files.private.json').read_text())
            assert hashlib.sha256((LVBASE/'prepared/trailguide_v3/cards_generated.h').read_bytes()).hexdigest() == lv_inventory['prepared/trailguide_v3/cards_generated.h']
            objects = native.current_objects()
            extra = native.flags()
            if name == 'ui':
                extra.append('-Wl,--wrap=lv_timer_create')
        binary = OUT/('v3-range-'+name)
        command = ['g++', *flags, *extra, str(ROOT/f'tests/v3_app_{name}_test.cpp'), *objects, '-lm', '-o', str(binary)]
        print('COMPILE', name, 'read-only LVGL objects='+str(len(objects)), flush=True)
        subprocess.run(command, check=True, cwd=ROOT)
        print('RUN', binary, flush=True)
        result = subprocess.run([str(binary)], cwd=ROOT, timeout=180)
        if result.returncode:
            return result.returncode if result.returncode > 0 else 128-result.returncode
    print('PASS focused synthetic checks; no target/hardware/network; current host cache rebuilt only if needed', flush=True)
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
