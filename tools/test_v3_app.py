#!/usr/bin/env python3
"""Compile and exercise the actual target-bundled LVGL 8.3.10; no hardware."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import hashlib
import subprocess
from prepare_v3 import prepare

ROOT = Path(__file__).resolve().parents[1]
LVGL = ROOT / 'build/v3-0.1.4/libraries/lvgl'
OUT = ROOT / 'build/v3-0.1.4/host'


def main():
    manifest=prepare()  # Host and S3 builds consume the same archive-pinned, unchanged library.
    (ROOT/'build/v3-0.1.4/frames').mkdir(parents=True,exist_ok=True)
    assert (ROOT/'trailguide_v3/ui.h').is_file(), 'Missing minimal V3 UI'
    OUT.mkdir(parents=True, exist_ok=True)
    flags = ['-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
             '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_CONF_PATH='+str(ROOT/'trailguide_v3/lv_conf.h'),
             '-I'+str(ROOT/'trailguide_v3'), '-I'+str(LVGL)]
    config = hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes() + repr(flags).encode()).hexdigest()[:12]

    def compile_source(source):
        obj = OUT / (config + '-' + hashlib.sha256(str(source).encode()+source.read_bytes()).hexdigest()[:16] + '.o')
        if not obj.exists():
            subprocess.run(['gcc', '-std=c99', *flags, '-c', str(source), '-o', str(obj)], check=True, capture_output=True)
        return str(obj)

    sources = sorted((LVGL/'src').rglob('*.c'))
    assert sources, 'Missing pinned bundled LVGL sources'
    with ThreadPoolExecutor(max_workers=2) as pool:
        objects = list(pool.map(compile_source, sources))
    sketch_source=(ROOT/'trailguide_v3/trailguide_v3.ino').read_text()
    start=sketch_source.index('  if (uint32_t(now-last_heartbeat)>=5000)')
    end=sketch_source.index('\n  delay(5);',start)
    (OUT/'startup_heartbeat_generated.h').write_text(sketch_source[start:end])
    startup=OUT/'v3-app-startup-test'
    subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(OUT),str(ROOT/'tests/v3_app_startup_test.cpp'),*objects,'-lm','-o',str(startup)],check=True)
    subprocess.run([str(startup)],check=True,timeout=30)
    exe = OUT/'v3-app-ui-test'
    subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags,
                    '-I'+str(Path(manifest['sketch'])), str(ROOT/'tests/v3_app_ui_test.cpp'), *objects, '-Wl,--wrap=lv_timer_create', '-lm', '-o', str(exe)], check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True, timeout=180)
    motion_start=sketch_source.index('  motionDiagnostics.service(Serial,now);')
    motion_end=sketch_source.index('    if (!motionDiagnostics.active() && !motionDiagnostics.draining() && before == v3::ImuState::Probing',motion_start)
    (OUT/'startup_motion_generated.h').write_text(sketch_source[motion_start:motion_end]+'    (void)before;\n  }\n')
    integration=OUT/'v3-app-motion-integration-test'
    subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(OUT),'-I'+str(Path(manifest['sketch'])),str(ROOT/'tests/v3_app_motion_integration_test.cpp'),*objects,'-lm','-o',str(integration)],check=True)
    subprocess.run([str(integration)],cwd=ROOT,check=True,timeout=180)
    for name in ('model','gesture','imu','motion'):
        binary=OUT/('v3-app-'+name+'-test')
        subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,str(ROOT/('tests/v3_app_'+name+'_test.cpp')),'-o',str(binary)],check=True)
        subprocess.run([str(binary)],check=True)
    print('PASS: actual target-bundled LVGL 8.3.10; ASan/UBSan enabled; synthetic host input only.')


if __name__ == '__main__':
    main()
