#!/usr/bin/env python3
"""Run every Python test in a seeded disposable source copy, never fallbacks.

Only local pinned archive assets are seeded. No installs, target tools or serial.
The isolated source copy is removed after the run; logs/hashes remain in the owned output.
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
import signal
import time
import threading


def assert_complete_attempts(out):
    for folder in sorted(out.glob('attempt-*')):
        try:
            status=json.loads((folder/'status.json').read_text())['status']
        except (OSError,ValueError,KeyError):
            status='incomplete'
        if status not in ('passed','failed','timeout','interrupted'):
            raise RuntimeError('incomplete prior attempt (inspect/retain before retry): '+str(folder))


def run_attempt(command,cwd,env,out,protected,timeout=600,metadata=None):
    """Stream evidence and audit after the owned Linux process group quiesces.

    Normal owned helpers inherit this group via explicit environment opt-in.
    Deliberate setsid/setpgid escape is outside this non-hostile runner contract;
    /proc group-state inspection is read-only, never a name-based kill sweep.
    SIGKILL/power loss cannot run finally; the next preflight rejects running or
    missing status records instead. An operator must retain and resolve them.
    """
    out.mkdir(parents=True,exist_ok=True)
    assert_complete_attempts(out)
    folder=Path(tempfile.mkdtemp(prefix='attempt-',dir=out))
    report={'status':'running','returncode':None,'command':list(command),'metadata':metadata or {},
            'log':str(folder/'child.log'),'report':str(folder/'status.json'),
            'process_group_reaped':False,'target_or_hardware_run':False}
    def publish():
        temp=folder/'status.tmp'
        temp.write_text(json.dumps(report,indent=2)+'\n')
        temp.replace(folder/'status.json')
    publish()
    child=None
    old_handler=None
    def interrupted(signum,frame):
        raise InterruptedError('signal '+str(signum))
    if threading.current_thread() is threading.main_thread():
        old_handler=signal.signal(signal.SIGTERM,interrupted)
    try:
        with (folder/'child.log').open('w') as log:
            # Explicitly opt normal owned helpers into this session's group.
            # No signal handlers in worker threads or detached helper sessions.
            child_env={**env,'TG_SUPERVISED_SESSION':'1'}
            child=subprocess.Popen(command,cwd=cwd,env=child_env,stdout=log,stderr=subprocess.STDOUT,start_new_session=True)
            report['pid']=child.pid
            try:
                report['returncode']=child.wait(timeout=timeout)
                report['status']='passed' if report['returncode']==0 else 'failed'
            except subprocess.TimeoutExpired:
                report.update(status='timeout',returncode=124)
    except (KeyboardInterrupt,InterruptedError) as exc:
        report.update(status='interrupted',returncode=130,error=str(exc))
    except Exception as exc:
        report.update(status='failed',returncode=1,error=repr(exc))
    finally:
        report['completion_status']=report['status']
        # Kill only the owned group even when its leader exited normally:
        # inherited browser/helper children can outlive it. Never touch unrelated
        # groups or delete their state.
        try:
            if child is not None:
                try:os.killpg(child.pid,signal.SIGTERM)
                except ProcessLookupError:pass
                time.sleep(.05)
                try:os.killpg(child.pid,signal.SIGKILL)
                except ProcessLookupError:pass
                child.wait(timeout=5)
                # killpg is asynchronous. Confirm only this owned group is
                # quiescent before inventory/sandbox finalization. Linux zombies
                # are non-running; they cannot be waited by a non-parent.
                deadline=time.monotonic()+5
                while True:
                    active=[]
                    for entry in Path('/proc').iterdir():
                        if not entry.name.isdigit():continue
                        try:fields=(entry/'stat').read_text().rpartition(')')[2].split()
                        except (FileNotFoundError,ProcessLookupError):continue
                        if int(fields[2])==child.pid and fields[0] not in ('Z','X'):
                            active.append(entry.name)
                    if not active:break
                    if time.monotonic()>=deadline:
                        raise RuntimeError('owned process group still running: '+','.join(active))
                    time.sleep(.01)
                report['process_group_reaped']=True
        except Exception as exc:
            report.update(status='failed',returncode=1,cleanup_error=repr(exc))
        post={}
        for path,expected in protected.items():
            try:
                actual=inventory(path)
                post[str(path)]={'exact':actual==expected,'files':actual}
            except Exception as exc:
                post[str(path)]={'exact':False,'error':repr(exc)}
        report['post_inventories']=post
        report['installed_all_exact']=all(p['exact'] for p in post.values())
        if not report['installed_all_exact']:
            report.update(status='failed',returncode=1)
        publish()
        if old_handler is not None:signal.signal(signal.SIGTERM,old_handler)
    return report

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/v3-0.1.22/evidence'

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
    with tempfile.TemporaryDirectory(prefix='trailguide-v3-1.21-',dir=temp_root) as folder:
        yield Path(folder)

def main():
    assert_complete_attempts(OUT)
    with disposable_sandbox() as sandbox:
        OUT.mkdir(parents=True,exist_ok=True)
        expected={n:json.loads((ROOT/f'build/v3-motion-investigation/installed-v3-0.1.{n}-files.private.json').read_text()) for n in (6,7,8,9,10,11,12,13,14,16)}
        bench_root=ROOT/'build/v3-speaker-bench-0.1.15'
        bench_files=json.loads((ROOT/'build/v3-motion-investigation/installed-speaker-bench-0.1.15-files.private.json').read_text())
        assert all(inventory(ROOT/f'build/v3-0.1.{n}')==files for n,files in expected.items()) and inventory(bench_root)==bench_files,'installed fallback already changed'
        installed_targets={
            'v3-0.1.18-coaching-compass':ROOT/'build/v3-coaching-compass-port/target',
            'v3-0.1.19-ui-shake':ROOT/'build/v3-ui-shake-0.1.19/target',
            'v3-0.1.20-shake-calibration':ROOT/'build/v3-shake-calibration-0.1.20/target',
            'v3-0.1.21-global-gestures':ROOT/'build/v3-global-gestures-0.1.21/target',
        }
        target_inventories={version:json.loads((ROOT/'build/v3-motion-investigation'/('installed-'+version+'-files.private.json')).read_text()) for version in installed_targets}
        assert all(inventory(path)==target_inventories[version] for version,path in installed_targets.items()),'installed target already changed'
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
        # Copy only producer-bound synthetic artifacts, never captures. Missing
        # or stale inputs fail the required integration cases (no silent skips).
        from v3_q1_protocol_artifacts import OUTPUTS,MANIFEST,output
        output(source).mkdir(parents=True,exist_ok=True)
        for name in (*OUTPUTS,MANIFEST):
            fixture=output(ROOT)/name
            if fixture.exists():shutil.copyfile(fixture,output(source)/name)
        (sandbox/'tmp').mkdir()
        env=isolated_environment(sandbox)
        env['TG_PROTOCOL_TIER']='integration'
        protected={ROOT/f'build/v3-0.1.{n}':files for n,files in expected.items()}
        protected[bench_root]=bench_files
        protected.update({path:target_inventories[version] for version,path in installed_targets.items()})
        report=run_attempt([sys.executable,'-B','-m','unittest','discover','-s','tests','-p','*_test.py','-v'],
                           source,env,OUT,protected,timeout=600,metadata={
                               'seeded_archive_sha256':ARCHIVE_SHA256,'PYTHONHASHSEED':17,
                               'source_sha256':inventory(source/'trailguide_v3'),
                               'sandbox':str(sandbox),'protocol_tier':'integration'})
        print(Path(report['log']).read_text())
        print(json.dumps({'status':report['status'],'report':report['report']}))
        return report['returncode']

if __name__=='__main__':raise SystemExit(main())
