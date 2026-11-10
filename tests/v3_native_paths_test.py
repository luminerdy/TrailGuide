"""Portable current-output consistency; no compiler, hardware or accepted writes."""
from pathlib import Path
import ast
import importlib
import sys
import unittest
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
HELPERS=('test_v3_app','v3_native_objects','v3_q1_protocol_artifacts','test_v3_hwcdc_budget','test_v3_coaching','test_v3_isolated','test_v3_range_focused','replay_v3_recovery')
CPP=('v3_app_ui_test.cpp','v3_coaching_compass_test.cpp','v3_coaching_test.cpp','v3_app_motion_integration_test.cpp','v3_app_turnover_test.cpp','v3_app_recorder_test.cpp','v3_sound_ui_test.cpp','v3_ui_cleanup_native_test.cpp')
class NativePaths(unittest.TestCase):
    def test_current_helpers_and_cpp_output_literals(self):
        import prepare_v3
        expected=str(prepare_v3.PRIVATE.relative_to(ROOT))
        self.assertEqual(expected,'build/v3-0.1.31')
        for name in HELPERS:
            text=(ROOT/'tools'/(name+'.py')).read_text()
            literals=[n.value for n in ast.walk(ast.parse(text)) if isinstance(n,ast.Constant) and isinstance(n.value,str)]
            for literal in literals:
                if 'build/v3-0.1.28' in literal:self.fail(name+': stale output '+literal)
        for name in CPP:
            text=(ROOT/'tests'/name).read_text()
            self.assertNotIn('build/v3-0.1.28',text,name)
            self.assertIn(expected,text,name)
    def test_native_derivations(self):
        import prepare_v3, test_v3_app as app, v3_native_objects as native, v3_q1_protocol_artifacts as protocol
        self.assertEqual(native.OUT,prepare_v3.PRIVATE)
        self.assertEqual(app.LVGL,prepare_v3.PRIVATE/'libraries/lvgl')
        self.assertEqual(app.OUT,prepare_v3.PRIVATE/'host')
        self.assertEqual(protocol.output(ROOT),prepare_v3.PRIVATE)
        self.assertIn('-I'+str(app.LVGL),native.flags())
    def test_full_runner_fresh_current_library_stops_before_compiler(self):
        import tempfile, shutil
        import prepare_v3, test_v3_app as app, v3_native_objects as native
        class StopCompiler(Exception):pass
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);current=root/'build/v3-0.1.31';library=current/'libraries/lvgl'
            (library/'src').mkdir(parents=True);(library/'src/fresh.c').write_text('int fresh;')
            (root/'trailguide_v3').mkdir();(root/'trailguide_v3/ui.h').write_text('// fixture')
            (root/'trailguide_v3/lv_conf.h').write_text('// fixture')
            calls=[]
            def run(command,**kwargs):
                calls.append(command)
                if command[0]=='gcc':
                    self.assertIn(str(library/'src/fresh.c'),command)
                    self.assertIn('-I'+str(library),command)
                    self.assertTrue(Path(command[-1]).is_relative_to(current/'host'))
                    raise StopCompiler()
            with patch.object(app,'ROOT',root),patch.object(app,'OUT',current/'host'),patch.object(app,'LVGL',library),patch.object(native,'ROOT',root),patch.object(native,'OUT',current),patch.object(app,'prepare',return_value={'sketch':str(current/'prepared/trailguide_v3')}),patch.object(app,'run_child',side_effect=run),patch.object(shutil,'disk_usage',return_value=type('Usage',(),{'free':3*1024**3})()):
                with self.assertRaises(StopCompiler):app.main()
            self.assertFalse((root/'build/v3-0.1.28').exists())
            self.assertTrue((current/'frames').is_dir())
            self.assertEqual(calls[-1][0],'gcc')
if __name__=='__main__':unittest.main()
