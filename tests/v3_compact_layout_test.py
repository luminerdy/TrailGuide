#!/usr/bin/env python3
"""Exact human-approved compact payload; no prepare/build/hardware."""
from pathlib import Path
import hashlib
import unittest
ROOT=Path(__file__).resolve().parents[1]
class CompactLayout(unittest.TestCase):
 def test_approved_compact_coaching_payload(self):
  source=(ROOT/'trailguide_v3/coaching_ui.h').read_text()
  plain='  if(opening){backButton=button(Action::Options,"Options",130,72,100,48);lv_obj_set_style_bg_opa(backButton,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(backButton,0,0);auto *caption=lv_obj_get_child(backButton,0);lv_obj_set_style_text_font(caption,&lv_font_montserrat_12,0);lv_obj_center(caption);}'
  original='  if(opening)backButton=button(Action::Options,"Options",130,72,100,48);'
  self.assertEqual(source.count(plain),1,'latest approved Options refinement must be present exactly once')
  # The only departure from the approved comparison is the later text-only Options request.
  self.assertEqual(hashlib.sha256(source.replace(plain,original).encode()).hexdigest(),'9b78f87e4b75ff1a21971a0da44f727261cd427eb790f0146f95672c0ee7ed2e')
 def test_only_approved_font_config(self):
  self.assertEqual(hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(),'45c55c7cbc4bfe5d73a525aaecaa44bf1a2cef9ad8e82fc0b2b157e4e5093b84')
if __name__=='__main__':unittest.main()
