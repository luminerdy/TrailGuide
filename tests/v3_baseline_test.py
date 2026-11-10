"""Portable installed-source identity and non-destructive preparation guards."""
from pathlib import Path
import hashlib
import importlib
import json
import sys
import tempfile
import unittest
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))

def selected_manifest():
    return 'content/v3-current.json' if (ROOT/'content/v3-current.json').exists() else 'content/v3-baseline.json'

class Baseline(unittest.TestCase):
    def test_duplicate_owned_names_cannot_reduce_actual_inventory(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);self.prepared_fixture(root)
            path=root/selected_manifest();manifest=json.loads(path.read_text())
            removed=manifest['owned_files'][-1]
            manifest['owned_files'][-1]=manifest['owned_files'][0]
            path.write_text(json.dumps(manifest));(root/'trailguide_v3'/removed).unlink()
            with self.assertRaises(ValueError):checker.check_owned(root)

    def test_owned_manifest_requires_flat_string_names(self):
        import check_v3_baseline as checker
        for invalid in (None,17,[],{},'nested/file.h','../outside.h','', '.', '..', '/absolute.h', 'nested\\file.h'):
            with self.subTest(invalid=invalid), tempfile.TemporaryDirectory() as folder:
                root=Path(folder);self.prepared_fixture(root)
                path=root/selected_manifest();manifest=json.loads(path.read_text())
                manifest['owned_files'][0]=invalid;path.write_text(json.dumps(manifest))
                with self.assertRaises(ValueError):checker.check_owned(root)

    def test_current_is_selected_and_old_baseline_rejected(self):
        import check_v3_baseline as checker
        self.assertEqual(checker.check_owned(ROOT)['version'], 'v3-0.1.31-mode-switch')
        self.assertEqual(checker.check_owned(ROOT, ROOT/'content/v3-current.json')['version'], 'v3-0.1.31-mode-switch')
        with self.assertRaisesRegex(ValueError, 'source'):
            checker.check_owned(ROOT, ROOT/'content/v3-baseline.json')

    def test_explicit_baseline_and_current_selection(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);prepared=self.prepared_fixture(root)
            current=root/selected_manifest()
            baseline=root/'content/v3-baseline.json'
            baseline.write_bytes(current.read_bytes())
            self.assertEqual(checker.check_prepared(root,prepared,baseline), checker.check_prepared(root,prepared,current))
            bad=json.loads(current.read_text())
            bad['prepared_files']['ui.h']='0'*64
            current.write_text(json.dumps(bad))
            with self.assertRaisesRegex(ValueError,'source'):checker.check_owned(root)
            checker.check_prepared(root,prepared,baseline)
            current.unlink()
            checker.check_prepared(root,prepared)  # backward-compatible baseline fallback

    def test_default_is_owned_baseline_output(self):
        import prepare_v3
        self.assertEqual(prepare_v3.PRIVATE,ROOT/'build/v3-0.1.31')

    def test_accepted_and_private_roots_rejected_before_archive(self):
        import prepare_v3
        roots=['build/v3-0.1.'+str(n) for n in range(32) if n!=31]
        roots += ['build/v3-motion23-overnight','build/v3-reliability-0.1.22/target']
        with patch.object(Path,'read_bytes',side_effect=AssertionError('archive accessed')):
            for root in roots:
                for suffix in ('','/nested'):
                    with self.subTest(root=root,suffix=suffix):
                        with self.assertRaises(ValueError):prepare_v3.prepare(ROOT/(root+suffix))

    def test_checker_rejects_owned_source_mutation(self):
        checker=importlib.import_module('check_v3_baseline')
        checker.check_owned(ROOT)
        with tempfile.TemporaryDirectory() as folder:
            target=Path(folder)
            import shutil
            shutil.copytree(ROOT/'trailguide_v3',target/'trailguide_v3')
            (target/'content').mkdir()
            shutil.copyfile(ROOT/selected_manifest(),target/selected_manifest())
            path=target/'trailguide_v3/shake.h'
            path.write_bytes(path.read_bytes()+b'\n// mutation\n')
            with self.assertRaisesRegex(ValueError,'source'):checker.check_owned(target)

    def prepared_fixture(self, target):
        """Flat 29-file fixture with real inventory checks and matching hashes."""
        manifest=json.loads((ROOT/selected_manifest()).read_text())
        prepared=target/'prepared'
        owned=target/'trailguide_v3'
        prepared.mkdir();owned.mkdir();(target/'content').mkdir()
        for name in manifest['prepared_files']:
            data=('fixture '+name+'\n').encode()
            (prepared/name).write_bytes(data)
            manifest['prepared_files'][name]=hashlib.sha256(data).hexdigest()
            if name in manifest['owned_files']:(owned/name).write_bytes(data)
        (target/selected_manifest()).write_text(json.dumps(manifest))
        return prepared

    def test_checker_rejects_prepared_nested_extra_cpp(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);prepared=self.prepared_fixture(root)
            checker.check_prepared(root,prepared)
            (prepared/'src').mkdir()
            (prepared/'src/unapproved.cpp').write_text('// unapproved source\n')
            with self.assertRaisesRegex(ValueError,'Prepared source'):
                checker.check_prepared(root,prepared)

    def test_checker_rejects_owned_nested_extra_cpp(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);self.prepared_fixture(root)
            checker.check_owned(root)
            (root/'trailguide_v3/src').mkdir()
            (root/'trailguide_v3/src/unapproved.cpp').write_text('// unapproved source\n')
            with self.assertRaisesRegex(ValueError,'source'):
                checker.check_owned(root)

    def test_checker_accepts_flat_fixture(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);prepared=self.prepared_fixture(root)
            manifest=checker.check_prepared(root,prepared)
            self.assertEqual(len(list((root/'trailguide_v3').iterdir())),17)
            self.assertEqual(len(list(prepared.iterdir())),29)
            self.assertEqual(len(manifest['prepared_files']),29)

    def test_checker_rejects_nonregular_inventory_nodes(self):
        import os
        import check_v3_baseline as checker
        for kind in ('owned','prepared'):
            for node in ('empty_directory','directory_symlink','file_symlink',
                         'extra_file_symlink','dangling_symlink','root_symlink','fifo'):
                with self.subTest(kind=kind,node=node), tempfile.TemporaryDirectory() as folder:
                    root=Path(folder);prepared=self.prepared_fixture(root)
                    source=root/'trailguide_v3' if kind=='owned' else prepared
                    check=lambda:checker.check_owned(root) if kind=='owned' else checker.check_prepared(root,prepared)
                    check()
                    if node=='empty_directory':(source/'extra').mkdir()
                    elif node=='directory_symlink':
                        outside=root/'outside';outside.mkdir()
                        (outside/'unapproved.cpp').write_text('// outside source\n')
                        (source/'src').symlink_to(outside,target_is_directory=True)
                    elif node=='file_symlink':
                        path=next(source.iterdir());outside=root/'outside.cpp'
                        outside.write_bytes(path.read_bytes());path.unlink();path.symlink_to(outside)
                    elif node=='extra_file_symlink':
                        (source/'alias.cpp').symlink_to(next(source.iterdir()))
                    elif node=='dangling_symlink':
                        (source/'missing.cpp').symlink_to(root/'missing')
                    elif node=='root_symlink':
                        outside=root/'actual-source';source.rename(outside)
                        source.symlink_to(outside,target_is_directory=True)
                    elif node=='fifo':os.mkfifo(source/'unexpected.cpp')
                    with self.assertRaisesRegex(ValueError,'source' if kind=='owned' else 'Prepared source'):
                        check()

    def test_checker_allows_symlink_ancestors(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            parent=Path(folder);root=parent/'real';root.mkdir()
            self.prepared_fixture(root)
            alias=parent/'alias';alias.symlink_to(root,target_is_directory=True)
            checker.check_prepared(alias,alias/'prepared')

    def test_checker_rejects_prepared_vendor_mutation(self):
        import check_v3_baseline as checker
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder);prepared=self.prepared_fixture(root)
            checker.check_prepared(root,prepared)
            path=prepared/'Display_ST77916.cpp'
            self.assertTrue(path.is_file())
            original=path.read_bytes()
            path.write_bytes(original+b'// vendor mutation\n')
            self.assertNotEqual(path.read_bytes(),original)
            with self.assertRaisesRegex(ValueError,'Prepared source'):
                checker.check_prepared(root,prepared)

if __name__=='__main__':unittest.main()
