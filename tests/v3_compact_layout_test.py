#!/usr/bin/env python3
"""Exact human-approved compact payload; no prepare/build/hardware."""
from pathlib import Path
import hashlib
import unittest
ROOT=Path(__file__).resolve().parents[1]
class CompactLayout(unittest.TestCase):
 def test_approved_compact_coaching_payload(self):
  source=(ROOT/'trailguide_v3/coaching_ui.h').read_text()
  # Frozen exact native payload approved in the 192px side-trim comparison.
  self.assertEqual(hashlib.sha256(source.encode()).hexdigest(),'e9d6b7a2ab78d858f2cce2a2f514ea54e31bbeace9eaced29c7fa56bdc8e265b')
 def test_only_approved_font_config(self):
  self.assertEqual(hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(),'45c55c7cbc4bfe5d73a525aaecaa44bf1a2cef9ad8e82fc0b2b157e4e5093b84')
if __name__=='__main__':unittest.main()
