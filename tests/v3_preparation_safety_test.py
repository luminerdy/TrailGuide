"""Q2: tiny, isolated preparations only; never use the vendor ZIP."""
import hashlib
import importlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
import zipfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))


class PreparationSafetyTest(unittest.TestCase):
    def fixture(self, root, module_name):
        module = importlib.import_module(module_name)
        anchor = 'build/v3-0.1.28' if module_name == 'prepare_v3' else 'build/v3-s3-bringup/imu'
        private = root / anchor
        sketch_name = 'trailguide_v3' if module_name == 'prepare_v3' else 'trailguide_v3_bringup'
        own = root / sketch_name
        own.mkdir()
        names = ('trailguide_v3.ino', 'ui.h', 'lv_conf.h', 'imu.h', 'model.h', 'gesture.h', 'shake.h', 'window_reveal.h', 'motion_diagnostics.h', 'flight_recorder.h', 'coaching_model.h', 'coaching_ui.h', 'sound_cue.h', 'chime.h', 'chime_tone.h') if module_name == 'prepare_v3' else ('trailguide_v3_bringup.ino', 'ui.h', 'lv_conf.h', 'imu.h')
        for name in names:
            (own / name).write_bytes(b'owned source\n')
        (root / 'content').mkdir()
        for name in ('cards.json', 'coaching.json'):
            (root / 'content' / name).write_bytes((ROOT / 'content' / name).read_bytes())
        archive = root / 'tiny.zip'
        display = (b'  test_draw_bitmap(panel_handle);\r\n'
                   b'esp_lcd_panel_handle_t panel_handle = NULL;\r\n'
                   b'  esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle);\r\n'
                   b'  esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color);')
        with zipfile.ZipFile(archive, 'w') as z:
            for name in module.DRIVERS:
                z.writestr('Arduino/examples/LVGL_Arduino/' + name, display if name == 'Display_ST77916.cpp' else b'vendor\n')
            z.writestr('Arduino/libraries/lvgl/src/tiny.c', b'unchanged library\n')
            z.writestr('Arduino/libraries/lvgl/lvgl.h', b'unchanged header\n')
        for relative in ('prepared/' + sketch_name, 'libraries', 'config', 'host-evidence'):
            directory = private / relative
            directory.mkdir(parents=True)
            (directory / 'keep').write_bytes(b'previous complete\n')
        (private / 'preparation.json').write_text('{"previous": "complete"}\n')
        return module, private, archive, own

    def snapshot(self, private):
        return {str(p.relative_to(private)): p.read_bytes() for p in private.rglob('*') if p.is_file()}

    def test_failed_inputs_preserve_previous_complete_preparation(self):
        for module_name in ('prepare_v3', 'prepare_v3_bringup'):
            for failure in ('extract', 'own-source', 'render'):
                if failure == 'render' and module_name != 'prepare_v3':
                    continue
                with self.subTest(module=module_name, failure=failure), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                    root = Path(tmp)
                    module, private, archive, own = self.fixture(root, module_name)
                    before = self.snapshot(private)
                    if failure == 'own-source':
                        (own / 'ui.h').unlink()
                    import contextlib
                    with contextlib.ExitStack() as stack:
                        for key, value in (('ROOT', root), ('PRIVATE', private), ('ARCHIVE', archive), ('ARCHIVE_SHA256', hashlib.sha256(archive.read_bytes()).hexdigest())):
                            stack.enter_context(patch.object(module, key, value))
                        if failure == 'extract':
                            stack.enter_context(patch.object(zipfile.ZipFile, 'read', side_effect=ValueError('injected extraction failure')))
                        elif failure == 'render':
                            stack.enter_context(patch('build_v3_content.render', side_effect=ValueError('injected render failure')))
                        with self.assertRaises((ValueError, FileNotFoundError)):
                            module.prepare()
                    self.assertEqual(self.snapshot(private), before)

    def test_publish_failure_restores_all_previous_outputs(self):
        import contextlib
        original_replace = Path.replace
        for module_name in ('prepare_v3', 'prepare_v3_bringup'):
            for fail_at in range(1, 9):
                with self.subTest(module=module_name, rename=fail_at), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                    root = Path(tmp)
                    module, private, archive, own = self.fixture(root, module_name)
                    before = self.snapshot(private)
                    calls = []
                    failure = OSError('injected publication rename failure')
                    def replace(source, target):
                        calls.append((source, target))
                        if len(calls) == fail_at:
                            raise failure
                        return original_replace(source, target)
                    with contextlib.ExitStack() as stack:
                        for key, value in (('ROOT', root), ('PRIVATE', private), ('ARCHIVE', archive), ('ARCHIVE_SHA256', hashlib.sha256(archive.read_bytes()).hexdigest())):
                            stack.enter_context(patch.object(module, key, value))
                        stack.enter_context(patch.object(Path, 'replace', replace))
                        with self.assertRaises(OSError) as caught:
                            module.prepare()
                        self.assertIs(caught.exception, failure)
                    self.assertEqual(self.snapshot(private), before)
                    self.assertFalse(any(private.glob('.preparation-*')))

    def test_success_publishes_final_paths_and_preserves_other_evidence(self):
        import contextlib
        for module_name in ('prepare_v3', 'prepare_v3_bringup'):
            with self.subTest(module=module_name), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                root = Path(tmp)
                module, private, archive, own = self.fixture(root, module_name)
                with contextlib.ExitStack() as stack:
                    for key, value in (('ROOT', root), ('PRIVATE', private), ('ARCHIVE', archive), ('ARCHIVE_SHA256', hashlib.sha256(archive.read_bytes()).hexdigest())):
                        stack.enter_context(patch.object(module, key, value))
                    manifest = module.prepare()
                    self.assertEqual(manifest, module.prepare())
                    child = private / 'owned-child'
                    child_manifest = module.prepare(child)
                    self.assertEqual(Path(child_manifest['sketch']).parents[1], child)
                    self.assertEqual(json.loads((child / 'preparation.json').read_text()), child_manifest)
                self.assertEqual(json.loads((private / 'preparation.json').read_text()), manifest)
                self.assertEqual(Path(manifest['sketch']).parents[1], private)
                self.assertEqual(Path(manifest['libraries']), private / 'libraries')
                self.assertNotIn('.preparation-', json.dumps(manifest))
                self.assertEqual((private / 'host-evidence/keep').read_bytes(), b'previous complete\n')
                for relative, digest in manifest['library_files'].items():
                    self.assertEqual(hashlib.sha256((private / 'libraries/lvgl' / relative).read_bytes()).hexdigest(), digest)
                self.assertFalse(any(private.glob('.preparation-*')))

    def test_archive_traversal_rejected_before_member_read(self):
        import contextlib
        original_read = zipfile.ZipFile.read
        for module_name in ('prepare_v3', 'prepare_v3_bringup'):
            with self.subTest(module=module_name), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                root = Path(tmp)
                module, private, archive, own = self.fixture(root, module_name)
                member = 'Arduino/libraries/lvgl/src/../escape.c'
                with zipfile.ZipFile(archive, 'a') as z:
                    z.writestr(member, b'must not be extracted')
                before = self.snapshot(private)
                reads = []
                def read(z, name, *args, **kwargs):
                    reads.append(name)
                    return original_read(z, name, *args, **kwargs)
                with contextlib.ExitStack() as stack:
                    for key, value in (('ROOT', root), ('PRIVATE', private), ('ARCHIVE', archive), ('ARCHIVE_SHA256', hashlib.sha256(archive.read_bytes()).hexdigest())):
                        stack.enter_context(patch.object(module, key, value))
                    stack.enter_context(patch.object(zipfile.ZipFile, 'read', read))
                    with self.assertRaises(ValueError):
                        module.prepare()
                self.assertNotIn(member, reads)
                self.assertEqual(self.snapshot(private), before)

    def test_unsupported_output_values_raise_value_error_before_archive(self):
        for module_name in ('prepare_v3', 'prepare_v3_bringup'):
            module = importlib.import_module(module_name)
            for output in (1, False, b'not-a-text-path', object()):
                with self.subTest(module=module_name, output=type(output).__name__), \
                     patch.object(Path, 'read_bytes', side_effect=AssertionError('invalid output read archive')):
                    with self.assertRaises(ValueError):
                        module.prepare(output)

    def test_unsupported_path_shapes_fail_closed_before_archive(self):
        for module_name, anchor in (('prepare_v3', 'build/v3-0.1.28'),
                                    ('prepare_v3_bringup', 'build/v3-s3-bringup/imu')):
            for shape in ('output-file', 'prepared', 'libraries', 'config', 'manifest-directory', 'anchor-loop'):
                with self.subTest(module=module_name, shape=shape), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                    root = Path(tmp)
                    module = importlib.import_module(module_name)
                    private = root / anchor
                    private.parent.mkdir(parents=True)
                    if shape == 'output-file':
                        private.write_bytes(b'not a directory')
                    elif shape == 'anchor-loop':
                        private.symlink_to(private)
                    else:
                        private.mkdir()
                        if shape == 'manifest-directory':
                            (private / 'preparation.json').mkdir()
                        else:
                            (private / shape).write_bytes(b'not a directory')
                    with patch.object(module, 'ROOT', root), patch.object(module, 'PRIVATE', private), \
                         patch.object(Path, 'read_bytes', side_effect=AssertionError('archive read before invalid shape rejection')):
                        with self.assertRaises(ValueError):
                            module.prepare()

    def test_reject_symlinks_before_archive_access(self):
        for module_name, anchor in (('prepare_v3', 'build/v3-0.1.28'),
                                    ('prepare_v3_bringup', 'build/v3-s3-bringup/imu')):
            for destination in ('anchor', 'prepared', 'libraries', 'config', 'preparation.json'):
                with self.subTest(module=module_name, destination=destination), tempfile.TemporaryDirectory(prefix='q2-') as tmp:
                    root = Path(tmp)
                    module = importlib.import_module(module_name)
                    private = root / anchor
                    outside = root / 'build/v3-0.1.20'
                    outside.mkdir(parents=True)
                    (outside / 'keep').write_bytes(b'accepted')
                    private.parent.mkdir(parents=True, exist_ok=True)
                    if destination == 'anchor':
                        private.symlink_to(outside, target_is_directory=True)
                    else:
                        private.mkdir()
                        (private / destination).symlink_to(outside, target_is_directory=True)
                    with patch.object(module, 'ROOT', root), patch.object(module, 'PRIVATE', private), \
                         patch.object(Path, 'read_bytes', side_effect=AssertionError('archive read before path rejection')):
                        with self.assertRaises(ValueError):
                            module.prepare()
                    self.assertEqual((outside / 'keep').read_bytes(), b'accepted')


if __name__ == '__main__':
    unittest.main()
