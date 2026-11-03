#!/usr/bin/env python3
"""Run every Python test in a seeded disposable source copy, never fallbacks.

Only local pinned archive assets are seeded. No installs, target tools or serial.
The isolated source copy is removed after the run; logs/hashes remain in 1.9 output.
"""
from pathlib import Path
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
import zipfile

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/v3-0.1.9/evidence'

def inventory(root):
    return {str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in root.rglob('*') if p.is_file()}

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    expected=json.loads((ROOT/'build/v3-motion-investigation/installed-v3-0.1.6-files.private.json').read_text())
    assert inventory(ROOT/'build/v3-0.1.6')==expected,'installed 1.6 baseline already changed'
    sandbox=Path(tempfile.mkdtemp(prefix='trailguide-v3-1.9-',dir=os.environ['TMPDIR']))
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
        fixture=ROOT/'build/v3-0.1.9'/name
        if fixture.exists():
            (source/'build/v3-0.1.9').mkdir(exist_ok=True);shutil.copyfile(fixture,source/'build/v3-0.1.9'/name)
    (sandbox/'tmp').mkdir()
    env={**os.environ,'TMPDIR':os.environ['TMPDIR'],'PYTHONHASHSEED':'17'}
    result=subprocess.run([sys.executable,'-m','unittest','discover','-s','tests','-p','*_test.py','-v'],cwd=source,env=env,capture_output=True,text=True,timeout=600)
    (OUT/'isolated-python.private.log').write_text(result.stdout+result.stderr)
    unchanged=inventory(ROOT/'build/v3-0.1.6')==expected
    report={'returncode':result.returncode,'seeded_archive_sha256':ARCHIVE_SHA256,'PYTHONHASHSEED':17,'installed_v1_6_file_count':len(expected),'installed_v1_6_exact':unchanged,'source_sha256':inventory(source/'trailguide_v3'),'sandbox':str(sandbox),'target_or_hardware_run':False}
    (OUT/'isolated-python.private.json').write_text(json.dumps(report,indent=2)+'\n')
    print(result.stdout+result.stderr);assert unchanged,'installed 1.6 fallback changed'
    shutil.rmtree(sandbox)  # only this task-created disposable source copy
    return result.returncode

if __name__=='__main__':raise SystemExit(main())
