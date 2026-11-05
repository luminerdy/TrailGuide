#!/usr/bin/env python3
"""V3 owned preparation/content boundaries. No hardware or network."""
import hashlib
import importlib.util
import json
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
def load_tool(name):
 spec=importlib.util.spec_from_file_location(name,ROOT/'tools'/f'{name}.py')
 assert spec is not None and spec.loader is not None,'Tool module specification/loader unavailable'
 module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module);return module
class Contracts(unittest.TestCase):
 def test_new_output_isolation(self):
  import prepare_v3
  self.assertEqual(prepare_v3.PRIVATE, ROOT/'build/v3-0.1.21')
 def test_installed_roots_rejected_before_archive_read(self):
  import prepare_v3
  from unittest.mock import patch
  # No successful preparation is allowed, even while proving the old guard RED.
  with patch.object(Path,'read_bytes',side_effect=AssertionError('archive read before immutable guard')):
   for version in ('1.6','1.7','1.8','1.9','1.10','1.11','1.12','1.13','1.14','1.16'):
    for suffix in ('','/nested','/nested/deep'):
     with self.subTest(version=version,suffix=suffix):
      with self.assertRaisesRegex(ValueError,'immutable'):
       prepare_v3.prepare(ROOT/('build/v3-0.'+version+suffix))
 def test_speaker_bench_rejected_before_archive_read(self):
  prepare_v3=load_tool('prepare_v3')
  from unittest.mock import patch
  with patch.object(Path,'read_bytes',side_effect=AssertionError('archive read before speaker-bench guard')):
   for suffix in ('','/nested','/nested/deep'):
    with self.subTest(suffix=suffix):
     with self.assertRaisesRegex(ValueError,'immutable'):
      prepare_v3.prepare(ROOT/('build/v3-speaker-bench-0.1.15'+suffix))
 def test_installed_target_roots_rejected_before_archive_read(self):
  prepare_v3=load_tool('prepare_v3')
  from unittest.mock import patch
  roots=('build/v3-0.1.18','build/v3-0.1.19','build/v3-coaching-compass-port/target','build/v3-ui-shake-0.1.19/target','build/v3-shake-calibration-0.1.20/target','build/v3-global-gestures-0.1.21/target')
  with patch.object(Path,'read_bytes',side_effect=AssertionError('archive read before installed-target guard')):
   for root in roots:
    for suffix in ('','/nested/deep'):
     with self.subTest(root=root,suffix=suffix):
      with self.assertRaisesRegex(ValueError,'immutable'):prepare_v3.prepare(ROOT/(root+suffix))
 def test_foreign_root_rejected_before_archive_read(self):
  import prepare_v3
  from unittest.mock import patch
  with patch.object(Path,'read_bytes',side_effect=AssertionError('foreign output read archive')):
   with self.assertRaises(ValueError):prepare_v3.prepare(ROOT/'build/v3-0.1.0')
 def test_diagnostic_increment_version(self):
  self.assertIn('version="v3-0.1.21-global-gestures"',(ROOT/'trailguide_v3/ui.h').read_text())
  self.assertIn('gyr_range_dps=2048 odr_enum=normal60 reset=not_issued suffix=unidentified', (ROOT/'trailguide_v3/trailguide_v3.ino').read_text())
 def test_shake_source_and_native_suites_are_wired(self):
  import ast
  strings={node.value for node in ast.walk(ast.parse((ROOT/'tools/prepare_v3.py').read_text())) if isinstance(node,ast.Constant) and isinstance(node.value,str)}
  self.assertIn('shake.h',strings)
  runner=(ROOT/'tools/test_v3_app.py').read_text()
  for source in ('v3_shake_detector_test.cpp','v3_shake_rotation_test.cpp','v3_global_policy_native_test.cpp','v3_shake_integration_native_test.cpp','v3_ui_cleanup_native_test.cpp'):self.assertIn(source,runner)
 def test_owned_preparer_exists(self):
  self.assertTrue((ROOT/'tools/prepare_v3.py').is_file(),'first app preparer missing')
 def test_coaching_build_wiring(self):
  import ast
  tree=ast.parse((ROOT/'tools/prepare_v3.py').read_text())
  strings={node.value for node in ast.walk(tree) if isinstance(node,ast.Constant) and isinstance(node.value,str)}
  self.assertTrue({'coaching_model.h','coaching_ui.h','coaching_generated.h','content/coaching.json'}<=strings)
  self.assertTrue(any(isinstance(node,ast.ImportFrom) and node.module=='build_v3_coaching' for node in ast.walk(tree)))
  self.assertIn('tests/v3_coaching_test.cpp',(ROOT/'tools/test_v3_app.py').read_text())
 def test_clean_coaching_frame_destination(self):
  import tempfile
  from unittest.mock import patch
  test_v3_app=load_tool('test_v3_app')
  self.assertTrue(hasattr(test_v3_app,'coaching_frame_directory'),'full runner must create the native harness actual frame destination')
  with tempfile.TemporaryDirectory() as d:
   host=Path(d)/'new/host'
   with patch.object(test_v3_app,'OUT',host):
    destination=test_v3_app.coaching_frame_directory()
   self.assertEqual(destination,host.parent/'evidence')
   self.assertTrue(destination.is_dir())
 def test_recovery_replay_uses_current_output_and_binary(self):
  import tempfile
  from unittest.mock import patch
  replay_v3_recovery=load_tool('replay_v3_recovery')
  self.assertEqual(replay_v3_recovery.OUT,ROOT/'build/v3-0.1.21/evidence')
  with tempfile.TemporaryDirectory() as d:
   root=Path(d);capture=root/'synthetic.txt'
   capture.write_text('TG_MOTION1 t=100 at=100 valid=1 rest=1 phase=4>4 trigger=0 gates=0,0,0,0,1 origin_e6=1:0,0,1000000 acc_g_e6=0,0,1000000 gyro_dps_e3=0,0,0\n')
   import subprocess
   with patch.object(replay_v3_recovery,'ROOT',root),patch.object(replay_v3_recovery,'OUT',root/'build/v3-0.1.21/evidence'),patch.object(sys,'argv',['replay',str(capture),'--start','100','--end','100','--expected-rows','1']),patch.object(replay_v3_recovery.subprocess,'run',return_value=subprocess.CompletedProcess([],0,'','')) as execute:
    self.assertEqual(replay_v3_recovery.main(),0)
   self.assertEqual(execute.call_args.args[0][0],str(root/'build/v3-0.1.21/host/v3-app-recovery-test'))
   self.assertFalse((root/'build/v3-0.1.11').exists())
 def test_content_and_exact_drivers(self):
  import prepare_v3
  from build_v3_content import render
  m=prepare_v3.prepare()
  self.assertEqual((Path(m['sketch'])/'cards_generated.h').read_text(),render(json.loads((ROOT/'content/cards.json').read_text())))
  from build_v3_coaching import render as render_coaching
  generated=render_coaching(json.loads((ROOT/'content/coaching.json').read_text()),json.loads((ROOT/'content/cards.json').read_text()))
  self.assertEqual((Path(m['sketch'])/'coaching_generated.h').read_text(),generated)
  self.assertEqual(m['own_files']['coaching_generated.h'],hashlib.sha256(generated.encode()).hexdigest())
  for name in ('coaching_model.h','coaching_ui.h'):
   self.assertEqual((Path(m['sketch'])/name).read_bytes(),(ROOT/'trailguide_v3'/name).read_bytes())
  old=json.loads((ROOT/'build/v3-s3-bringup/imu/preparation.json').read_text())
  self.assertEqual(m['vendor_files'],old['vendor_files'])
  self.assertEqual(m['library_files'],old['library_files'])
  self.assertIn('PSRAM=opi',m['fqbn']);self.assertIn('app3M_fat9M_16MB',m['fqbn'])
 def test_refuse_foreign_output(self):
  import prepare_v3
  for p in ['build/v3-0.1.7','build/v3-0.1.7/nested','build/v3-0.1.6','build/v3-0.1.6/nested','build/v3-0.1.5','build/v3-0.1.5/nested','build/v3-0.1.4','build/v3-0.1.4/nested','build/v3-0.1.3','build/v3-0.1.3/nested','build/v3-0.1.2','build/v3-0.1.2/nested','build/v3-0.1.1/nested','build/v3-0.1.5/../v3-0.1.1','build/v3-0.1.1','build/v3-0.1.0','build/v3-s3-bringup/imu','build/v3-s3-bringup/minimal','trailguide_v2','build/v3-0.1.0/../escape']:
   with self.assertRaises(ValueError):prepare_v3.prepare(ROOT/p)
if __name__=='__main__':unittest.main()
