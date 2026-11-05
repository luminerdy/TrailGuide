#!/usr/bin/env python3
"""Frozen presentation regions, not motion/settlement lifecycle policy.

The public fixture was derived from the accepted compact20 header and compared
with accepted21. Native geometry/pixel/full-copy tests remain independent gates.
"""
from pathlib import Path
import hashlib
import json
import re
import unittest
ROOT=Path(__file__).resolve().parents[1]
# Whole method regions preserve styles, geometry, copy, wrapping and overflow.
# No token replacement whitelist: changes in these regions require review.
PAINT_METHODS=('render','startCompass','wander','drawCompass','drawBlank',
               'updateHint','chrome','drawWindow','maskText','compactFace',
               'label','button','icon')
TOKENS=re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|\w+|[^\s]',re.S)

def presentation(source):
    regions={}
    for name in PAINT_METHODS:
        # Find the declaration, never a call site inside another method.
        match=re.search(r'(?m)^ [^\n]*?\b'+name+r'\([^;\n]*?\)\s*(?:const)?\s*\{',source)
        if match is None: raise ValueError('missing presentation method: '+name)
        start=source.index('{',match.start(),match.end())
        depth=0; region=[]
        for token in TOKENS.finditer(source,start):
            value=token.group()
            if value.startswith(('//','/*')):continue
            region.append(value)
            if value=='{':depth+=1
            elif value=='}':
                depth-=1
                if depth==0:break
        else:raise ValueError('unclosed presentation method: '+name)
        if name=='render':
            # Teardown/reset before constructing the root is lifecycle policy,
            # not presentation. The paint starts at this exact root creation.
            paint=region.index('lv_obj_create')-2
            if region[paint:paint+3]!=['root','=','lv_obj_create']:
                raise ValueError('missing render paint boundary')
            if any(t.startswith(('lv_color_','lv_font_','lv_draw_','lv_obj_set_','lv_label_')) for t in region[:paint]):
                raise ValueError('unexpected presentation in lifecycle prelude')
            region=region[paint:]
        regions[name]=hashlib.sha256(json.dumps(region,separators=(',',':')).encode()).hexdigest()
    return regions

def approved_presentation():
    return json.loads((ROOT/'tests/v3_q1_presentation.json').read_text())['methods']

class CompactLayout(unittest.TestCase):
 def test_approved_compact_coaching_payload(self):
  self.assertEqual(presentation((ROOT/'trailguide_v3/coaching_ui.h').read_text()),approved_presentation())
 def test_only_approved_font_config(self):
  self.assertEqual(hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(),'45c55c7cbc4bfe5d73a525aaecaa44bf1a2cef9ad8e82fc0b2b157e4e5093b84')
if __name__=='__main__':unittest.main()
