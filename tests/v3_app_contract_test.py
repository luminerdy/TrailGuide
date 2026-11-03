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
class Contracts(unittest.TestCase):
 def test_new_output_isolation(self):
  import prepare_v3
  self.assertEqual(prepare_v3.PRIVATE, ROOT/'build/v3-0.1.7')
  with self.assertRaises(ValueError):prepare_v3.prepare(ROOT/'build/v3-0.1.0')
 def test_owned_preparer_exists(self):
  self.assertTrue((ROOT/'tools/prepare_v3.py').is_file(),'first app preparer missing')
 def test_content_and_exact_drivers(self):
  import prepare_v3
  from build_v2_content import render
  m=prepare_v3.prepare()
  self.assertEqual((Path(m['sketch'])/'cards_generated.h').read_text(),render(json.loads((ROOT/'content/cards.json').read_text())))
  old=json.loads((ROOT/'build/v3-s3-bringup/imu/preparation.json').read_text())
  self.assertEqual(m['vendor_files'],old['vendor_files'])
  self.assertEqual(m['library_files'],old['library_files'])
  self.assertIn('PSRAM=opi',m['fqbn']);self.assertIn('app3M_fat9M_16MB',m['fqbn'])
 def test_refuse_foreign_output(self):
  import prepare_v3
  for p in ['build/v3-0.1.6','build/v3-0.1.6/nested','build/v3-0.1.5','build/v3-0.1.5/nested','build/v3-0.1.4','build/v3-0.1.4/nested','build/v3-0.1.3','build/v3-0.1.3/nested','build/v3-0.1.2','build/v3-0.1.2/nested','build/v3-0.1.1/nested','build/v3-0.1.5/../v3-0.1.1','build/v3-0.1.1','build/v3-0.1.0','build/v3-s3-bringup/imu','build/v3-s3-bringup/minimal','trailguide_v2','build/v3-0.1.0/../escape']:
   with self.assertRaises(ValueError):prepare_v3.prepare(ROOT/p)
if __name__=='__main__':unittest.main()
