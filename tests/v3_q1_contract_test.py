"""Small source/runner regressions: no native compilation or browser."""
import importlib.util
import json
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
sys.path.insert(0, str(ROOT/'tests'))
import v3_compact_layout_test as layout

class PresentationContract(unittest.TestCase):
    def test_paint_mutations_rejected_but_runtime_policy_is_not_layout(self):
        source = (ROOT/'trailguide_v3/coaching_ui.h').read_text()
        self.assertEqual(layout.presentation(source), layout.approved_presentation())
        for before, after in [('0x124d8b','0x124d8c'), ('lv_font_montserrat_18','lv_font_montserrat_14'),
                              ('opening?256:244','opening?255:244'), ('Scroll for full text','Scroll'),
                              ('text.y2<=viewport.y2','text.y2<viewport.y2'),
                              ('LV_LABEL_LONG_WRAP','LV_LABEL_LONG_CLIP')]:
            self.assertIn(before, source)
            with self.subTest(mutation=before):
                self.assertNotEqual(layout.presentation(source.replace(before,after)), layout.approved_presentation())
        policy = source.replace('return root && !pointerDown && !scrolling;', 'return root && !pointerDown && !scrolling && !busy();')
        self.assertNotEqual(source, policy)
        self.assertEqual(layout.presentation(policy), layout.approved_presentation())
        # Rendering owns teardown policy as well as paint. A new lifecycle
        # reset in the pre-paint prelude must not be treated as geometry drift.
        lifecycle=source.replace('cancel();++resetCounts[0];gesture.reset();','cancel();++resetCounts[0];gesture.reset();shake.reset();')
        self.assertNotEqual(lifecycle,source)
        self.assertEqual(layout.presentation(lifecycle),layout.approved_presentation())

class BoundedNativeRunner(unittest.TestCase):
    def test_child_timeout_and_full_runner_resource_preflight(self):
        import subprocess
        import test_v3_app as app
        from types import SimpleNamespace
        with self.assertRaises(subprocess.TimeoutExpired):
            app.run_child([sys.executable,'-c','import time;time.sleep(30)'],timeout=.05)
        with patch.object(app.shutil,'disk_usage',return_value=SimpleNamespace(free=0)), patch.object(app,'prepare') as prepare:
            with self.assertRaisesRegex(RuntimeError,'2 GiB'): app.main()
            prepare.assert_not_called()

    def test_all_native_commands_use_bounded_helper_and_gesture_loop_is_bounded(self):
        import ast
        tree=ast.parse((ROOT/'tools/test_v3_app.py').read_text())
        main=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='main')
        self.assertFalse(any(isinstance(n,ast.Attribute) and n.attr=='run' and isinstance(n.value,ast.Name) and n.value.id=='subprocess' for n in ast.walk(main)))
        gesture=(ROOT/'tests/v3_app_gesture_test.cpp').read_text()
        self.assertNotIn('while(!cool.tick(cool.up)){}',gesture)
        self.assertIn('expected cooldown trigger exhausted',gesture)

class ProtocolArtifactContract(unittest.TestCase):
    def test_integration_cannot_skip_missing_producer_artifacts(self):
        import v3_recorder_protocol_test as protocol
        with tempfile.TemporaryDirectory() as folder, patch.object(protocol,'ROOT',Path(folder)), patch.dict(os.environ,{'TG_PROTOCOL_TIER':'integration'}):
            for name in ('test_actual_cpp_writer_and_decoder','test_actual_manual_cpp_header_rows_crc'):
                try:
                    with self.assertRaisesRegex(AssertionError,'producer'):
                        getattr(protocol.Protocol(name),name)()
                except unittest.SkipTest as exc:
                    self.fail('integration silently skipped missing producer: '+str(exc))

    def test_current_producer_command_emits_bound_manifest_and_rejects_staleness(self):
        import v3_q1_protocol_artifacts as artifacts
        import shutil
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder)
            for name in artifacts.INPUTS:
                p=root/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(ROOT/name,p)
            # Mock only the forbidden compiler/native boundary; the helper must
            # actually call both canonical commands before sealing tiny fixtures.
            calls=[]
            def execute(command,**kwargs):
                calls.append(command)
                if command[0]=='g++':
                    artifacts.binary(root).parent.mkdir(parents=True,exist_ok=True)
                    artifacts.binary(root).write_bytes(b'synthetic test executable marker')
                else:
                    for name in artifacts.OUTPUTS:(artifacts.output(root)/name).write_text('SYNTHETIC validator fixture')
            artifacts.produce(root,execute)
            self.assertEqual(len(calls),2)
            self.assertEqual(calls[0][0],'g++')
            self.assertEqual(calls[1],[str(artifacts.binary(root))])
            artifacts.validate(root)
            manifest=json.loads((artifacts.output(root)/artifacts.MANIFEST).read_text())
            self.assertTrue(manifest['synthetic_only'])
            self.assertNotIn(str(root),json.dumps(manifest))
            for name in (artifacts.INPUTS[0], 'trailguide_v3/lv_conf.h'):
                p=root/name;old=p.read_bytes();p.write_bytes(old+b' changed')
                with self.assertRaisesRegex(ValueError,'producer'):artifacts.validate(root)
                p.write_bytes(old)
            p=artifacts.output(root)/artifacts.OUTPUTS[0];p.write_text('stale bytes')
            with self.assertRaisesRegex(ValueError,'output'):artifacts.validate(root)

class DeliveredArtifactContract(unittest.TestCase):
    def test_build_and_browser_setup_never_regenerate_delivered_html(self):
        import v3_experience_test as experience
        import subprocess
        real_run=subprocess.run
        delivered=ROOT/'prototype/v3-experience.html'
        before=delivered.read_bytes()
        commands=[]
        def checked(command,**kwargs):
            commands.append(command)
            if '--output' not in command:
                self.assertIn('--check',command,'delivered HTML must be checked, never regenerated by tests')
            return real_run(command,**kwargs)
        with patch.object(experience.subprocess,'run',side_effect=checked):
            experience.BuildTest('test_cli_check_detects_stale_output_and_invalid_content').test_cli_check_detects_stale_output_and_invalid_content()
            with patch.object(experience,'Chromium'):
                case=experience.BrowserTest('test_fair_cycles_both_modes_complete_canonical_text_and_boundary')
                case.setUp();case.doCleanups()
        self.assertEqual(delivered.read_bytes(),before)
        self.assertTrue(any('--output' in c and '--check' not in c for c in commands),'generation must still be exercised on scratch output')

if __name__ == '__main__': unittest.main()
