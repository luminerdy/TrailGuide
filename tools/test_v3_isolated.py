#!/usr/bin/env python3
"""Run every Python test in a seeded disposable source copy, never fallbacks.

Only local pinned archive assets are seeded. No installs, target tools or serial.
The isolated source copy is removed after the run; logs/hashes remain in 1.16 output.
"""
from pathlib import Path
from contextlib import contextmanager
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
import zipfile

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/v3-0.1.16/evidence'

def inventory(root):
    return {str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in root.rglob('*') if p.is_file()}

def isolated_environment(sandbox):
    config=sandbox/'xdg-config';cache=sandbox/'xdg-cache'
    config.mkdir(exist_ok=True);cache.mkdir(exist_ok=True)
    # Chromium headless otherwise leaves scoped_dir profiles under the real HOME.
    # Keep TMPDIR short for browser Unix sockets, but contain profile/cache state.
    return {**os.environ,'TMPDIR':os.environ['TMPDIR'],'PYTHONHASHSEED':'17',
            'XDG_CONFIG_HOME':str(config),'CHROME_CONFIG_HOME':str(config),
            'XDG_CACHE_HOME':str(cache)}

@contextmanager
def disposable_sandbox():
    temp_root=Path(os.environ['TMPDIR'])
    if min(shutil.disk_usage(ROOT).free,shutil.disk_usage(temp_root).free)<2*1024**3:
        raise RuntimeError('Insufficient free space: require 2 GiB before isolated tests')
    with tempfile.TemporaryDirectory(prefix='trailguide-v3-1.16-',dir=temp_root) as folder:
        yield Path(folder)

def main():
    with disposable_sandbox() as sandbox:
        OUT.mkdir(parents=True,exist_ok=True)
        expected={n:json.loads((ROOT/f'build/v3-motion-investigation/installed-v3-0.1.{n}-files.private.json').read_text()) for n in (6,7,8,9,10,11,12,13,14)}
        bench_root=ROOT/'build/v3-speaker-bench-0.1.15'
        bench_files=json.loads((ROOT/'build/v3-motion-investigation/installed-speaker-bench-0.1.15-files.private.json').read_text())
        assert all(inventory(ROOT/f'build/v3-0.1.{n}')==files for n,files in expected.items()) and inventory(bench_root)==bench_files,'installed fallback already changed'
        source=sandbox/'source';source.mkdir()
        for name in ('tools','tests','content','docs','prototype','trailguide_v2','trailguide_v3','trailguide_v3_bringup','oled_oracle'):
            shutil.copytree(ROOT/name,source/name,ignore=shutil.ignore_patterns('__pycache__','node_modules'))
        seed=source/'build/v3-s3-bringup';seed.mkdir(parents=True)
        archive=ROOT/'build/v3-s3-bringup/ESP32-S3-Touch-LCD-1.85-Demo.zip'
        os.link(archive,seed/archive.name)  # immutable read-only input; no ZIP copies consuming fallback space
        sys.path.insert(0,str(ROOT/'tools'))
        from prepare_v3 import DRIVERS,ARCHIVE_SHA256
        assert hashlib.sha256(archive.read_bytes()).hexdigest()==ARCHIVE_SHA256
        with zipfile.ZipFile(archive) as z:
            for name in DRIVERS:
                p=seed/'vendor/Arduino/examples/LVGL_Arduino'/name;p.parent.mkdir(parents=True,exist_ok=True)
                p.write_bytes(z.read('Arduino/examples/LVGL_Arduino/'+name))
        (seed/'imu').mkdir()
        shutil.copyfile(ROOT/'build/v3-s3-bringup/imu/preparation.json',seed/'imu/preparation.json')
        # Seed only clearly synthetic C++ fixtures, never captured evidence.
        for name in ('recorder.synthetic.wire','recorder.manual.synthetic.wire'):
            fixture=ROOT/'build/v3-0.1.16'/name
            if fixture.exists():
                (source/'build/v3-0.1.16').mkdir(exist_ok=True);shutil.copyfile(fixture,source/'build/v3-0.1.16'/name)
        (sandbox/'tmp').mkdir()
        env=isolated_environment(sandbox)
        result=subprocess.run([sys.executable,'-m','unittest','discover','-s','tests','-p','*_test.py','-v'],cwd=source,env=env,capture_output=True,text=True,timeout=600)
        (OUT/'isolated-python.private.log').write_text(result.stdout+result.stderr)
        unchanged=all(inventory(ROOT/f'build/v3-0.1.{n}')==files for n,files in expected.items()) and inventory(bench_root)==bench_files
        report={'returncode':result.returncode,'seeded_archive_sha256':ARCHIVE_SHA256,'PYTHONHASHSEED':17,'installed_file_counts':{str(n):len(files) for n,files in expected.items()},'installed_all_exact':unchanged,'source_sha256':inventory(source/'trailguide_v3'),'sandbox':str(sandbox),'target_or_hardware_run':False}
        (OUT/'isolated-python.private.json').write_text(json.dumps(report,indent=2)+'\n')
        print(result.stdout+result.stderr);assert unchanged,'installed 1.6/1.7/1.8/1.9/1.10/1.11/1.12/1.13/1.14/speaker-bench fallback changed'
        return result.returncode

if __name__=='__main__':raise SystemExit(main())
