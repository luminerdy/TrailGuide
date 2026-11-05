import importlib.util
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('storage_runner', ROOT/'tools/test_v3_isolated.py')
assert spec is not None and spec.loader is not None
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)

class StorageIsolation(unittest.TestCase):
    def test_timeout_streams_log_and_publishes_final_inventory(self):
        import json, sys
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder); protected=root/'protected';protected.mkdir();(protected/'ok').write_text('fixed')
            expected=runner.inventory(protected)
            report=runner.run_attempt([sys.executable,'-u','-c','import time;print("started",flush=True);time.sleep(30)'],
                root, os.environ.copy(),root/'attempts',{protected:expected},timeout=.1)
            self.assertEqual(report['status'],'timeout')
            self.assertTrue(report['installed_all_exact'])
            self.assertIn('started',Path(report['log']).read_text())
            self.assertEqual(json.loads(Path(report['report']).read_text())['status'],'timeout')
            self.assertTrue(report['process_group_reaped'])

    def test_inventory_failure_is_reported_not_lost(self):
        import sys
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder); protected=root/'protected';protected.mkdir()
            with patch.object(runner,'inventory',side_effect=OSError('synthetic inventory failure')):
                report=runner.run_attempt([sys.executable,'-c','pass'],root,os.environ.copy(),root/'attempts',{protected:{}},timeout=2)
            self.assertEqual(report['status'],'failed')
            self.assertFalse(report['installed_all_exact'])
            self.assertIn('synthetic inventory failure',str(report['post_inventories']))
            self.assertTrue(Path(report['report']).exists())

    def test_next_preflight_detects_missing_or_incomplete_report(self):
        import json,sys
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder); attempts=root/'attempts'; old=attempts/'attempt-old';old.mkdir(parents=True)
            (old/'child.log').write_text('interrupted')
            with self.assertRaisesRegex(RuntimeError,'incomplete'):
                runner.run_attempt([sys.executable,'-c','pass'],root,os.environ.copy(),attempts,{},timeout=2)
            (old/'status.json').write_text(json.dumps({'status':'running'}))
            with self.assertRaisesRegex(RuntimeError,'incomplete'):
                runner.run_attempt([sys.executable,'-c','pass'],root,os.environ.copy(),attempts,{},timeout=2)

    def test_attempt_history_is_not_overwritten(self):
        import sys
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder)
            first=runner.run_attempt([sys.executable,'-c','print("first")'],root,os.environ.copy(),root/'attempts',{},timeout=2)
            before=Path(first['report']).read_bytes()
            second=runner.run_attempt([sys.executable,'-c','print("second")'],root,os.environ.copy(),root/'attempts',{},timeout=2)
            self.assertNotEqual(first['report'],second['report'])
            self.assertEqual(Path(first['report']).read_bytes(),before)

    def test_interruption_is_published_and_descendants_cannot_outlive_sandbox(self):
        import json,sys
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder)
            # A real nested process stands in for a browser child; no browser runs.
            command=[sys.executable,'-u','-c',
                     'import subprocess,sys,time; p=subprocess.Popen([sys.executable,"-c","import time;time.sleep(30)"]);print(p.pid,flush=True);time.sleep(30)']
            report=runner.run_attempt(command,root,os.environ.copy(),root/'attempts',{},timeout=.15)
            descendant=int(Path(report['log']).read_text().strip())
            state=Path(f'/proc/{descendant}/stat')
            if state.exists():self.assertEqual(state.read_text().split()[2],'Z','descendant still running before sandbox cleanup')
            command=[sys.executable,'-c','import os,signal,time;os.kill(os.getppid(),signal.SIGTERM);time.sleep(30)']
            interrupted=runner.run_attempt(command,root,os.environ.copy(),root/'attempts',{},timeout=2)
            self.assertEqual(interrupted['status'],'interrupted')
            self.assertEqual(interrupted['returncode'],130)
            self.assertTrue(interrupted['process_group_reaped'])
            self.assertEqual(json.loads(Path(interrupted['report']).read_text())['status'],'interrupted')

    def test_sandbox_is_removed_even_when_a_job_raises(self):
        sandbox = None
        with self.assertRaisesRegex(OSError, 'simulated write failure'):
            from types import SimpleNamespace
            # Exercise tiny cleanup fixture independently of live resource headroom.
            with patch.object(runner.shutil,'disk_usage',return_value=SimpleNamespace(free=3*1024**3)):
                with runner.disposable_sandbox() as sandbox:
                    (sandbox/'evidence.tmp').write_text('temporary')
                    raise OSError('simulated write failure')
        self.assertIsNotNone(sandbox)
        assert sandbox is not None
        self.assertFalse(sandbox.exists())

    def test_low_space_blocks_before_creating_a_sandbox(self):
        from types import SimpleNamespace
        with patch.object(runner.shutil, 'disk_usage', return_value=SimpleNamespace(free=0)):
            with patch.object(runner.tempfile, 'TemporaryDirectory') as create:
                with self.assertRaisesRegex(RuntimeError, 'free space'):
                    with runner.disposable_sandbox():
                        self.fail('Low disk must prevent entry')
                create.assert_not_called()

    def test_browser_state_is_scoped_to_disposable_sandbox(self):
        with tempfile.TemporaryDirectory(prefix='disk-test-') as folder:
            sandbox = Path(folder)
            original = {'XDG_CONFIG_HOME':'/old/config', 'XDG_CACHE_HOME':'/old/cache',
                        'CHROME_CONFIG_HOME':'/old/chrome', 'TMPDIR':os.environ['TMPDIR']}
            with patch.dict(os.environ, original):
                before = dict(os.environ)
                env = runner.isolated_environment(sandbox)
                self.assertEqual(dict(os.environ), before)
                for key in ('XDG_CONFIG_HOME','XDG_CACHE_HOME','CHROME_CONFIG_HOME'):
                    path = Path(env[key])
                    self.assertTrue(path.is_relative_to(sandbox))
                    self.assertTrue(path.is_dir())
                self.assertEqual(env['TMPDIR'], before['TMPDIR'])
                self.assertEqual(env['PYTHONHASHSEED'], '17')

if __name__ == '__main__':
    unittest.main()
