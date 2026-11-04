#!/usr/bin/env python3
"""Exact human-approved compact payload; no prepare/build/hardware."""
from pathlib import Path
import hashlib
import unittest
ROOT=Path(__file__).resolve().parents[1]
class CompactLayout(unittest.TestCase):
 def test_approved_compact_coaching_payload(self):
  source=(ROOT/'trailguide_v3/coaching_ui.h').read_text()
  # Frozen exact muted-button/Sound payload; native checks preserve the approved 192px face.
  self.assertEqual(hashlib.sha256(source.encode()).hexdigest(),'febf7ac10fee1d5ec10e2c003eaaf603cf4deb082755b2d8ed365238742c1dd1')
 def test_only_approved_font_config(self):
  self.assertEqual(hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(),'45c55c7cbc4bfe5d73a525aaecaa44bf1a2cef9ad8e82fc0b2b157e4e5093b84')
if __name__=='__main__':unittest.main()
