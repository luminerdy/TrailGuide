"""Composed blocker regressions: tiny Python only, no native/browser/SDK."""
from concurrent.futures import ThreadPoolExecutor
import importlib.util
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import test_v3_app as app
import test_v3_isolated as runner


def running(pid):
    try:
        return Path(f'/proc/{pid}/stat').read_text().rpartition(')')[2].split()[0] not in ('Z', 'X')
    except FileNotFoundError:
        return False


def cleanup_owned(records):
    for record in records:
        if running(record['pid']):
            try:
                os.kill(record['pid'], signal.SIGKILL)
            except ProcessLookupError:
                pass


class OwnedHelperComposition(unittest.TestCase):
    def exercise(self, interruption=False, threaded=False):
        with tempfile.TemporaryDirectory(prefix='tg-blocker-') as folder:
            root = Path(folder)
            records = []
            jobs = []
            for index in range(2 if threaded else 1):
                path = root / f'child-{index}.json'
                code = ('import os,time,json;from pathlib import Path;'
                        f'Path({str(path)!r}).write_text(json.dumps(dict(pid=os.getpid(),pgid=os.getpgrp())));'
                        'time.sleep(30)')
                jobs.append([sys.executable, '-B', '-c', code])
            middle = ('import sys,os,signal,time;from pathlib import Path;'
                      f'sys.path.insert(0,{str(ROOT / "tools")!r});'
                      'from test_v3_app import run_child;'
                      'from concurrent.futures import ThreadPoolExecutor;'
                      f'jobs={jobs!r};pool=ThreadPoolExecutor(max_workers=2);'
                      'futures=[pool.submit(run_child,job,timeout=20,check=True) for job in jobs];'
                      f'paths={[str(root / f"child-{i}.json") for i in range(len(jobs))]!r};'
                      '\nwhile not all(Path(p).exists() for p in paths): time.sleep(.005)\n')
            if threaded:
                if interruption:
                    middle += 'os.kill(os.getppid(),signal.SIGTERM)\n'
                middle += '[future.result() for future in futures]\npool.shutdown()\n'
            else:
                middle = ('import sys,os;'
                          f'sys.path.insert(0,{str(ROOT / "tools")!r});'
                          'from test_v3_app import run_child;'
                          f'job={jobs[0]!r};')
                if interruption:
                    middle += ('job[-1]=job[-1].replace("time.sleep(30)",'
                               'f"import signal;os.kill({os.getppid()},signal.SIGTERM);time.sleep(30)");')
                middle += 'run_child(job,timeout=20,check=True)'
            try:
                report = runner.run_attempt([sys.executable, '-B', '-c', middle], root,
                                            os.environ.copy(), root / 'attempts', {}, timeout=.8)
                records = [json.loads((root / f'child-{i}.json').read_text()) for i in range(len(jobs))]
                self.assertEqual(report['status'], 'interrupted' if interruption else 'timeout')
                self.assertTrue(report['process_group_reaped'])
                self.assertTrue(root.exists(), 'assert cleanup before sandbox removal')
                self.assertFalse(any(running(item['pid']) for item in records),
                                 'owned run_child descendant still running after attempt finalization')
                self.assertTrue(all(item['pgid'] == report['pid'] for item in records))
                self.assertEqual(json.loads(Path(report['report']).read_text())['process_group_reaped'], True)
            finally:
                # RED must not leak its deliberately reproduced detached sessions.
                records = [json.loads(path.read_text()) for path in root.glob('child-*.json')]
                cleanup_owned(records)
                deadline = time.monotonic() + 2
                while any(running(item['pid']) for item in records) and time.monotonic() < deadline:
                    time.sleep(.01)
                self.assertFalse(any(running(item['pid']) for item in records), 'fixture cleanup failed')

    def test_outer_timeout_cleans_composed_helper(self):
        self.exercise()

    def test_outer_interruption_cleans_composed_helper(self):
        self.exercise(interruption=True)

    def test_outer_timeout_cleans_two_threadpool_helpers(self):
        self.exercise(threaded=True)

    def test_outer_interruption_cleans_two_threadpool_helpers(self):
        self.exercise(interruption=True, threaded=True)


class HelperOwnershipCompatibility(unittest.TestCase):
    def test_standalone_timeout_kills_own_nested_group(self):
        with tempfile.TemporaryDirectory(prefix='tg-standalone-') as folder:
            record = Path(folder) / 'owned.json'
            sleeper = 'import time;time.sleep(30)'
            code = ('import os,json,subprocess,sys,time;from pathlib import Path;'
                    f'p=subprocess.Popen([sys.executable,"-B","-c",{sleeper!r}]);'
                    f'Path({str(record)!r}).write_text(json.dumps(dict(pid=os.getpid(),nested=p.pid,pgid=os.getpgrp())));'
                    'time.sleep(30)')
            owned = []
            try:
                with patch.dict(os.environ):
                    os.environ.pop('TG_SUPERVISED_SESSION', None)
                    with self.assertRaises(subprocess.TimeoutExpired):
                        app.run_child([sys.executable, '-B', '-c', code], timeout=.4, capture_output=True)
                item = json.loads(record.read_text())
                owned = [{'pid': item['pid']}, {'pid': item['nested']}]
                self.assertEqual(item['pid'], item['pgid'])
                self.assertNotEqual(item['pgid'], os.getpgrp())
                self.assertFalse(any(running(p['pid']) for p in owned))
            finally:
                if record.exists():
                    item = json.loads(record.read_text())
                    cleanup_owned([{'pid': item['pid']}, {'pid': item['nested']}])

    def test_supervised_local_timeout_preserves_exception_and_cleans_nested_at_boundary(self):
        with tempfile.TemporaryDirectory(prefix='tg-local-') as folder:
            root = Path(folder)
            record = root / 'nested.pid'
            sleeper = ('import subprocess,sys,time;from pathlib import Path;'
                       'p=subprocess.Popen([sys.executable,"-B","-c","import time;time.sleep(30)"]);'
                       f'Path({str(record)!r}).write_text(str(p.pid));time.sleep(30)')
            command = ('import sys,subprocess;'
                       f'sys.path.insert(0,{str(ROOT / "tools")!r});from test_v3_app import run_child;'
                       '\ntry:\n'
                       f' run_child({[sys.executable, "-B", "-c", sleeper]!r},timeout=.4,capture_output=True)\n'
                       'except subprocess.TimeoutExpired:\n print("local timeout preserved",flush=True)\n'
                       'else:\n raise AssertionError("local timeout lost")\n')
            try:
                report = runner.run_attempt([sys.executable, '-B', '-c', command], root,
                                            os.environ.copy(), root / 'attempts', {}, timeout=3)
                pid = int(record.read_text())
                self.assertEqual(report['status'], 'passed', Path(report['log']).read_text())
                self.assertIn('local timeout preserved', Path(report['log']).read_text())
                self.assertTrue(report['process_group_reaped'])
                self.assertFalse(running(pid))
            finally:
                if record.exists():cleanup_owned([{'pid': int(record.read_text())}])

    def test_two_parallel_normal_helpers_do_not_kill_runner_or_each_other(self):
        with tempfile.TemporaryDirectory(prefix='tg-parallel-') as folder:
            root = Path(folder)
            code = ('import sys,os,json;from pathlib import Path;'
                    f'sys.path.insert(0,{str(ROOT / "tools")!r});from test_v3_app import run_child;'
                    'from concurrent.futures import ThreadPoolExecutor;'
                    'pool=ThreadPoolExecutor(max_workers=2);'
                    'jobs=[[sys.executable,"-B","-c",f"import time;time.sleep({delay});print({index})"] for index,delay in enumerate((.05,.2))];'
                    'futures=[pool.submit(run_child,job,check=True,capture_output=True) for job in jobs];'
                    'results=[f.result().stdout.decode().strip() for f in futures];pool.shutdown();'
                    f'Path({str(root / "results.json")!r}).write_text(json.dumps([results,os.environ["TG_SUPERVISED_SESSION"]]))')
            env = {**os.environ, 'TG_SUPERVISED_SESSION': 'caller-value'}
            before = dict(env)
            report = runner.run_attempt([sys.executable, '-B', '-c', code], root, env,
                                        root / 'attempts', {}, timeout=3)
            self.assertEqual(report['status'], 'passed', Path(report['log']).read_text())
            self.assertTrue(report['process_group_reaped'])
            self.assertEqual(json.loads((root / 'results.json').read_text()), [['0', '1'], '1'])
            self.assertEqual(env, before)

    def test_failed_launch_does_not_claim_reaped_process_group(self):
        with tempfile.TemporaryDirectory(prefix='tg-nochild-') as folder:
            root = Path(folder)
            with patch.object(runner.subprocess, 'Popen', side_effect=OSError('injected launch failure')):
                report = runner.run_attempt(['unused'], root, os.environ.copy(), root / 'attempts', {})
            self.assertEqual(report['status'], 'failed')
            self.assertFalse(report['process_group_reaped'])
            self.assertNotIn('pid', report)

    def test_failed_cleanup_does_not_claim_reaped_process_group(self):
        with tempfile.TemporaryDirectory(prefix='tg-cleanup-') as folder:
            root = Path(folder)
            with patch.object(runner.os, 'killpg', side_effect=PermissionError('injected cleanup failure')):
                report = runner.run_attempt([sys.executable, '-B', '-c', 'pass'], root,
                                            os.environ.copy(), root / 'attempts', {}, timeout=3)
            self.assertEqual(report['status'], 'failed')
            self.assertEqual(report['completion_status'], 'passed')
            self.assertFalse(report['process_group_reaped'])
            self.assertIn('injected cleanup failure', report['cleanup_error'])

    def test_unverifiable_cleanup_is_not_reported_as_reaped(self):
        with tempfile.TemporaryDirectory(prefix='tg-unverified-') as folder:
            root = Path(folder)
            original = Path.iterdir
            def unreadable_proc(path):
                if path == Path('/proc'):raise PermissionError('injected group verification failure')
                return original(path)
            with patch.object(Path, 'iterdir', unreadable_proc):
                report = runner.run_attempt([sys.executable, '-B', '-c', 'pass'], root,
                                            os.environ.copy(), root / 'attempts', {}, timeout=3)
            self.assertEqual(report['status'], 'failed')
            self.assertFalse(report['process_group_reaped'])
            self.assertIn('injected group verification failure', report['cleanup_error'])


class RollbackReportingFailure(unittest.TestCase):
    def test_failed_rollback_and_marker_keep_old_complete_outputs_and_original_error(self):
        import preparation_safety as safety
        with tempfile.TemporaryDirectory(prefix='tg-rollback-') as folder:
            private = Path(folder)
            (private / 'preparation.json').write_text('OLD COMPLETE')
            (private / 'prepared/sketch').mkdir(parents=True)
            (private / 'prepared/sketch/old').write_text('OLD SOURCE')
            publication = OSError('injected publication failure')
            replace, write = Path.replace, Path.write_text
            stage = private / 'not-entered'
            def fail_replace(source, target):
                if source == stage / 'prepared/sketch':
                    raise publication
                if source == stage / '.previous/prepared/sketch':
                    raise OSError('injected rollback failure')
                return replace(source, target)
            def fail_marker(path, *args, **kwargs):
                if path.name == '.rollback-needed':
                    raise OSError('injected marker failure')
                return write(path, *args, **kwargs)
            with self.assertRaises(OSError) as caught:
                with safety.preparation_stage(private) as stage:
                    (stage / 'prepared/sketch').mkdir(parents=True)
                    (stage / 'prepared/sketch/new').write_text('NEW SOURCE')
                    (stage / 'preparation.json').write_text('NEW COMPLETE')
                    with patch.object(Path, 'replace', fail_replace), patch.object(Path, 'write_text', fail_marker):
                        safety.publish(stage, private, 'sketch')
            self.assertTrue(stage.exists(), 'recovery stage deleted after marker write failed')
            self.assertEqual((stage / '.previous/preparation.json').read_text(), 'OLD COMPLETE')
            self.assertEqual((stage / '.previous/prepared/sketch/old').read_text(), 'OLD SOURCE')
            self.assertIs(caught.exception, publication)
            notes = '\n'.join(caught.exception.__notes__)
            self.assertIn('injected rollback failure', notes)
            self.assertIn('injected marker failure', notes)
            self.assertIn(str(stage), notes)


    def test_deep_sketch_symlink_rejected_before_archive_access(self):
        import importlib
        for module_name, anchor, sketch in (
                ('prepare_v3', 'build/v3-0.1.28', 'trailguide_v3'),
                ('prepare_v3_bringup', 'build/v3-s3-bringup/imu', 'trailguide_v3_bringup')):
            with self.subTest(module=module_name), tempfile.TemporaryDirectory(prefix='tg-deep-') as folder:
                root = Path(folder)
                private = root / anchor
                nested = private / 'prepared' / sketch / 'subdir/deeper'
                nested.mkdir(parents=True)
                outside = root / 'outside'
                outside.mkdir()
                (outside / 'keep').write_text('untouched')
                (nested / 'linked').symlink_to(outside, target_is_directory=True)
                module = importlib.import_module(module_name)
                with patch.object(module, 'ROOT', root), patch.object(module, 'PRIVATE', private), \
                     patch.object(Path, 'read_bytes', side_effect=AssertionError('archive accessed')):
                    with self.assertRaisesRegex(ValueError, 'symlink'):
                        module.prepare()
                self.assertEqual((outside / 'keep').read_text(), 'untouched')


if __name__ == '__main__':
    unittest.main()
