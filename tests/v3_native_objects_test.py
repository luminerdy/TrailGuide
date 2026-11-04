#!/usr/bin/env python3
"""Native cache/config boundaries; tiny synthetic files, never compile/prepare."""
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import v3_native_objects as native
from prepare_v3 import ARCHIVE_SHA256

class NativeObjects(unittest.TestCase):
 def setUp(self):
  self.temp=tempfile.TemporaryDirectory();self.addCleanup(self.temp.cleanup)
  self.root=Path(self.temp.name);self.out=self.root/'build/v3-0.1.18'
  (self.root/'trailguide_v3').mkdir();(self.root/'trailguide_v3/lv_conf.h').write_text('#define LV_FONT_MONTSERRAT_12 1\n')
  (self.out/'host').mkdir(parents=True)
  for name,value in [('ROOT',self.root),('OUT',self.out)]:
   p=patch.object(native,name,value);p.start();self.addCleanup(p.stop)
  library=self.out/'libraries/lvgl/src';library.mkdir(parents=True)
  files={};self.objects=[]
  for i in range(424):
   source=library/f'unit-{i}.c';source.write_text('/* synthetic cache unit */\n')
   files['src/'+source.name]=hashlib.sha256(source.read_bytes()).hexdigest()
   obj=native.object_path(source);obj.write_bytes(b'synthetic object, not executable');self.objects.append(str(obj))
  (self.out/'preparation.json').write_text(json.dumps({'archive_sha256':ARCHIVE_SHA256,'library_files':files}))
  native.seal(self.objects)
 def test_current_config_cache_accepted(self):
  self.assertEqual(set(native.verified_objects()),set(self.objects))
 def test_font_change_rejects_cache(self):
  (self.root/'trailguide_v3/lv_conf.h').write_text('#define LV_FONT_MONTSERRAT_12 0\n')
  with self.assertRaisesRegex(AssertionError,'config changed'):native.verified_objects()
 def test_object_corruption_rejected(self):
  Path(self.objects[0]).write_bytes(b'wrong')
  with self.assertRaises(AssertionError):native.verified_objects()
 def test_library_change_rejected(self):
  (self.out/'libraries/lvgl/src/unit-0.c').write_text('wrong source')
  with self.assertRaises(AssertionError):native.verified_objects()
 def test_missing_cache_calls_only_full_current_host_runner(self):
  (self.out/'host/native-objects.json').unlink()
  with patch.object(native.subprocess,'run',side_effect=lambda *a,**k:native.seal(self.objects)) as run:
   self.assertEqual(set(native.current_objects()),set(self.objects))
  run.assert_called_once_with([sys.executable,str(self.root/'tools/test_v3_app.py')],cwd=self.root,check=True)
 def test_fallback_must_produce_verified_cache(self):
  (self.out/'host/native-objects.json').unlink()
  with patch.object(native.subprocess,'run'):
   with self.assertRaises(FileNotFoundError):native.current_objects()
if __name__=='__main__':unittest.main()
