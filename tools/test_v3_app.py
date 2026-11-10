#!/usr/bin/env python3
"""Compile and exercise the actual target-bundled LVGL 8.3.10; no hardware."""
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
import hashlib
import subprocess
import sys
import os
import signal
import shutil


def run_child(command, *, timeout=None, check=False, capture_output=False, **kwargs):
    """Finite budgets; standalone sessions or an explicitly owned outer group.

    Under run_attempt, descendants stay in its group, including compiler workers
    launched from threads. Local timeout still raises TimeoutExpired and reaps
    the direct child; the outer boundary quiesces the whole group before return.
    Never kill the shared group here (including on normal parallel completion).
    Standalone calls retain their own session and nested-group cleanup.
    """
    supervised = (os.environ.get('TG_SUPERVISED_SESSION') == '1'
                  and os.getpgrp() == os.getsid(0))
    if timeout is None: timeout=300 if Path(command[0]).name in ('gcc','g++') else 180
    if capture_output:
        kwargs.update(stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    with subprocess.Popen(command,start_new_session=not supervised,**kwargs) as child:
        try:
            stdout,stderr=child.communicate(timeout=timeout)
        finally:
            try:
                if supervised:child.kill()
                else:os.killpg(child.pid,signal.SIGKILL)
            except ProcessLookupError:pass
            child.wait(timeout=5)
        result=subprocess.CompletedProcess(command,child.returncode,stdout,stderr)
        if check:result.check_returncode()
        return result
from prepare_v3 import prepare
import v3_native_objects as native

ROOT = Path(__file__).resolve().parents[1]
LVGL = ROOT / 'build/v3-0.1.31/libraries/lvgl'
OUT = ROOT / 'build/v3-0.1.31/host'

def coaching_frame_directory():
    # The native harness writes here, including on a completely fresh output.
    directory=OUT.parent/'evidence'
    directory.mkdir(parents=True,exist_ok=True)
    return directory

def reliability_loop(sketch):
    """Compile the actual loop order, including the audio acceptance boundary."""
    start = sketch.index('  if (ready) Lvgl_Loop();')
    end = sketch.index('  if (uint32_t(now-last_heartbeat)>=5000)', start)
    return sketch[start:end]


def reliability_cases():
    """Both real UI owners; cue lifecycle plus raw faults/recording modes."""
    for owner in ('0', '1'):
        for mode in ('navigation', 'replacement', 'instant', 'switch', 'identity',
                     'playing', 'wrap', 'deadline'):
            yield [mode, owner]
        for mode in ('gesture', 'gesture-instant'):
            for instant in ('0', '1'):
                yield [mode, owner, instant]
        for mode in ('partial', 'unread'):
            yield [mode, owner]
        for recording in ('0', '1'):
            for domain in range(10):
                yield ['fault', owner, str(domain), recording]
            for mode in ('notready', 'muted'):
                yield [mode, owner, '0', recording]


def main():
    if shutil.disk_usage(ROOT).free<2*1024**3:
        raise RuntimeError('Insufficient free space: require 2 GiB before native tests')
    run_child([sys.executable, str(ROOT/'tools/test_v3_hwcdc_budget.py')], check=True)
    manifest=prepare()  # Host and S3 builds consume the same archive-pinned, unchanged library.
    (ROOT/'build/v3-0.1.31/frames').mkdir(parents=True,exist_ok=True)
    assert (ROOT/'trailguide_v3/ui.h').is_file(), 'Missing minimal V3 UI'
    OUT.mkdir(parents=True, exist_ok=True)
    flags = native.flags()

    def compile_source(source):
        obj = native.object_path(source)
        if not obj.exists():
            run_child(['gcc', '-std=c99', *flags, '-c', str(source), '-o', str(obj)], check=True, capture_output=True)
        return str(obj)

    sources = sorted((LVGL/'src').rglob('*.c'))
    assert sources, 'Missing pinned bundled LVGL sources'
    with ThreadPoolExecutor(max_workers=2) as pool:
        objects = list(pool.map(compile_source, sources))
    native.seal(objects)
    sketch_source=(ROOT/'trailguide_v3/trailguide_v3.ino').read_text()
    start=sketch_source.index('  if (uint32_t(now-last_heartbeat)>=5000)')
    end=sketch_source.index('\n  delay(5);',start)
    (OUT/'startup_heartbeat_generated.h').write_text(sketch_source[start:end])
    startup=OUT/'v3-app-startup-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(OUT),str(ROOT/'tests/v3_app_startup_test.cpp'),*objects,'-lm','-o',str(startup)],check=True)
    run_child([str(startup)],check=True,timeout=30)
    exe = OUT/'v3-app-ui-test'
    run_child(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags,
                    '-I'+str(Path(manifest['sketch'])), str(ROOT/'tests/v3_app_ui_test.cpp'), *objects, '-Wl,--wrap=lv_timer_create', '-lm', '-o', str(exe)], check=True)
    run_child([str(exe)], cwd=ROOT, check=True, timeout=180)
    (OUT.parent/'turnover-frames').mkdir(exist_ok=True)
    turnover = OUT/'v3-app-turnover-test'
    run_child(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags,
                    '-I'+str(Path(manifest['sketch'])), str(ROOT/'tests/v3_app_turnover_test.cpp'), *objects, '-lm', '-o', str(turnover)], check=True)
    run_child([str(turnover)], cwd=ROOT, check=True, timeout=180)
    content_frames = OUT.parent/'content-frames'
    content_frames.mkdir(parents=True, exist_ok=True)
    content_exe = OUT/'v3-content-ui-test'
    run_child(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', *flags,
                    str(ROOT/'tests/v3_content_ui_test.cpp'), *objects, '-lm', '-o', str(content_exe)], check=True)
    run_child([str(content_exe), str(content_frames)], cwd=ROOT, check=True, timeout=180)
    motion_start=sketch_source.index('  motionDiagnostics.service(Serial,now);')
    motion_end=sketch_source.index('    if (!motionDiagnostics.active() && !motionDiagnostics.draining() && before == v3::ImuState::Probing',motion_start)
    (OUT/'startup_motion_generated.h').write_text(sketch_source[motion_start:motion_end]+'    (void)before;\n  }\n')
    integration=OUT/'v3-app-motion-integration-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(OUT),'-I'+str(Path(manifest['sketch'])),str(ROOT/'tests/v3_app_motion_integration_test.cpp'),*objects,'-lm','-o',str(integration)],check=True)
    run_child([str(integration)],cwd=ROOT,check=True,timeout=180)
    recovery=OUT/'v3-app-recovery-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(Path(manifest['sketch'])),str(ROOT/'tests/v3_app_recovery_test.cpp'),*objects,'-lm','-o',str(recovery)],check=True)
    run_child([str(recovery)],cwd=ROOT,check=True,timeout=180)
    coaching_frames=coaching_frame_directory()
    coaching=OUT/'v3-coaching-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    '-I'+str(Path(manifest['sketch'])),str(ROOT/'tests/v3_coaching_test.cpp'),*objects,'-lm','-o',str(coaching)],check=True)
    run_child([str(coaching),str(coaching_frames)],cwd=ROOT,check=True,timeout=180)
    compact=OUT/'v3-compact-ui-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    str(ROOT/'tests/v3_compact_ui_test.cpp'),*objects,'-lm','-o',str(compact)],check=True)
    run_child([str(compact)],cwd=ROOT,check=True,timeout=180)
    (ROOT/'build/v3-0.1.31/evidence/ui').mkdir(parents=True,exist_ok=True)
    sound_ui=OUT/'v3-sound-ui-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    str(ROOT/'tests/v3_sound_ui_test.cpp'),*objects,'-Wl,--wrap=lv_timer_create','-lm','-o',str(sound_ui)],check=True)
    run_child([str(sound_ui)],cwd=ROOT,check=True,timeout=180)
    (OUT.parent/'evidence/compass/frames').mkdir(parents=True,exist_ok=True)
    compass=OUT/'v3-coaching-compass-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                    str(ROOT/'tests/v3_coaching_compass_test.cpp'),*objects,'-lm','-o',str(compass)],check=True)
    run_child([str(compass)],cwd=ROOT,check=True,timeout=180)
    for source in ('v3_shake_detector_test.cpp','v3_shake_rotation_test.cpp'):
        binary=OUT/source.removesuffix('.cpp')
        run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                        str(ROOT/'tests'/source),'-o',str(binary)],check=True)
        run_child([str(binary)],cwd=ROOT,check=True,timeout=60)
    (OUT.parent/'evidence/ui-cleanup/frames').mkdir(parents=True,exist_ok=True)
    for source in ('v3_shake_integration_native_test.cpp','v3_global_policy_native_test.cpp','v3_ui_cleanup_native_test.cpp'):
        binary=OUT/source.removesuffix('.cpp')
        run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                        str(ROOT/'tests'/source),*objects,'-lm','-o',str(binary)],check=True)
        run_child([str(binary)],cwd=ROOT,check=True,timeout=180)
    for source in ('v3_chime_test.cpp','v3_chime_adapter_test.cpp'):
        binary=OUT/source.removesuffix('.cpp')
        run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,'-pthread',
                        '-I'+str(ROOT/'tests/stubs'),str(ROOT/'tests'/source),'-o',str(binary)],check=True)
        run_child([str(binary)],cwd=ROOT,check=True,timeout=60)
        if source=='v3_chime_adapter_test.cpp':
            for failure in range(1,9):run_child([str(binary),str(failure)],cwd=ROOT,check=True,timeout=60)
            # The S3 SDK disables hardware atomics; standard atomic correctness
            # is required, not the host's always-lock-free optimization trait.
            software=OUT/'v3_chime_sdk_atomic_test'
            software_flags=['-UTRAILGUIDE_CHIME_EXPECT_SOFTWARE_ATOMICS','-DTRAILGUIDE_CHIME_EXPECT_SOFTWARE_ATOMICS',
                            '-U__GCC_ATOMIC_INT_LOCK_FREE','-D__GCC_ATOMIC_INT_LOCK_FREE=1',
                            '-U__GCC_ATOMIC_LONG_LOCK_FREE','-D__GCC_ATOMIC_LONG_LOCK_FREE=1']
            run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,'-pthread',*software_flags,
                            '-I'+str(ROOT/'tests/stubs'),str(ROOT/'tests'/source),'-o',str(software)],check=True)
            run_child([str(software)],cwd=ROOT,check=True,timeout=60)
            for failure in range(1,9):run_child([str(software),str(failure)],cwd=ROOT,check=True,timeout=60)
    # Keep the lightweight framing/dispatch probes and composed real UI/audio
    # regressions in the canonical release path, not only private investigations.
    for source in ('v3_reliability_fault_dispatch_test.cpp', 'v3_reliability_framing_test.cpp'):
        binary=OUT/source.removesuffix('.cpp')
        run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                   '-I'+str(OUT),str(ROOT/'tests'/source),'-o',str(binary)],check=True)
        run_child([str(binary)],cwd=ROOT,check=True,timeout=60)
    (OUT/'r3_loop_generated.h').write_text(reliability_loop(sketch_source))
    composition=OUT/'v3_reliability_cue_composition_test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,'-pthread',
               '-I'+str(OUT),'-I'+str(ROOT/'tests/stubs'),
               str(ROOT/'tests/v3_reliability_cue_composition_test.cpp'),*objects,
               '-lm','-o',str(composition)],check=True)
    for arguments in reliability_cases():
        run_child([str(composition),*arguments],cwd=ROOT,check=True,timeout=60)
    import v3_q1_protocol_artifacts as protocol_artifacts
    protocol_artifacts.produce(ROOT,run_child)
    for name in ('model','gesture','imu','motion'):
        binary=OUT/('v3-app-'+name+'-test')
        run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,str(ROOT/('tests/v3_app_'+name+'_test.cpp')),'-o',str(binary)],check=True)
        run_child([str(binary)],check=True)
    baseline=ROOT/'build/v3-0.1.9/prepared/trailguide_v3/imu.h'
    baseline_inventory=__import__('json').loads((ROOT/'build/v3-motion-investigation/installed-v3-0.1.9-files.private.json').read_text())
    assert hashlib.sha256(baseline.read_bytes()).hexdigest()==baseline_inventory['prepared/trailguide_v3/imu.h']
    (OUT/'imu_baseline_generated.h').write_text(baseline.read_text().replace('namespace v3 {','namespace baseline {'))
    baseline_gesture=ROOT/'build/v3-0.1.9/prepared/trailguide_v3/gesture.h'
    assert hashlib.sha256(baseline_gesture.read_bytes()).hexdigest()==baseline_inventory['prepared/trailguide_v3/gesture.h']
    (OUT/'gesture_baseline_generated.h').write_text(baseline_gesture.read_text().replace('#include "imu.h"','#include "imu_baseline_generated.h"').replace('namespace v3app {','namespace baselineapp {').replace('v3::','baseline::'))
    equivalence=OUT/'v3-app-imu-equivalence-test'
    run_child(['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,'-I'+str(OUT),str(ROOT/'tests/v3_app_imu_equivalence_test.cpp'),'-o',str(equivalence)],check=True)
    run_child([str(equivalence)],check=True)
    run_child([sys.executable,'-B','-m','unittest','discover','-s','tests','-p','v3_recorder_protocol_test.py','-v'],
              cwd=ROOT,env={**os.environ,'TG_PROTOCOL_TIER':'integration'},check=True)
    print('PASS: actual target-bundled LVGL 8.3.10; ASan/UBSan enabled; synthetic host input only.')


if __name__ == '__main__':
    main()
