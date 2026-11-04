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
    def test_sandbox_is_removed_even_when_a_job_raises(self):
        sandbox = None
        with self.assertRaisesRegex(OSError, 'simulated write failure'):
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
