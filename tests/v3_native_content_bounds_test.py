"""Native C++ bounds are separate from browser JSON/Unicode rendering."""
from copy import deepcopy
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import build_prototype
import build_v2_content
import build_v3_content


class NativeContentBoundsTest(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((ROOT / 'content/cards.json').read_text())

    def card(self, data):
        return next(c for c in data['cards'] if c['card_id'] == 'tg-001')

    def test_native_revision_is_nonbool_nonzero_uint32(self):
        for renderer in (build_v2_content.render, build_v3_content.render):
            for revision in (True, False, 0, -1, 1.0, 0x100000000, 10**100):
                with self.subTest(renderer=renderer.__module__, revision=revision):
                    data = deepcopy(self.data)
                    self.card(data)['revision'] = revision
                    with self.assertRaises(ValueError):
                        renderer(data)
            for revision in (1, 0xffffffff):
                data = deepcopy(self.data)
                self.card(data)['revision'] = revision
                self.assertIn(', ' + str(revision) + ', ', renderer(data))

    def test_native_rejects_controls_but_keeps_deliberate_lf(self):
        for renderer in (build_v2_content.render, build_v3_content.render):
            for control in [chr(n) for n in (*range(10), *range(11, 32), 127)]:
                for field in ('title', 'activity', 'notice', 'try_invitation'):
                    with self.subTest(renderer=renderer.__module__, control=repr(control), field=field):
                        data = deepcopy(self.data)
                        card = self.card(data)
                        if field in ('title', 'activity'):
                            card[field] += control
                        elif field == 'notice':
                            card['facilitator'][field][0] += control
                        else:
                            card['facilitator'][field] += control
                        with self.assertRaisesRegex(ValueError, 'control'):
                            renderer(data)
            data = deepcopy(self.data)
            self.card(data)['title'] = 'First line\nSecond "quoted" line\\tail'
            output = renderer(data)
            self.assertIn(json.dumps(self.card(data)['title']), output)

    def test_browser_keeps_unicode_json_policy(self):
        data = deepcopy(self.data)
        self.card(data)['title'] = '雪\t<script>\u2028'
        self.card(data)['revision'] = 0x100000000
        output = build_prototype.render(data, '__CONTENT_JSON__')
        self.assertEqual(json.loads(output)['cards'][0]['title'], self.card(data)['title'])
        self.assertNotIn('<script>', output)

    def test_canonical_generated_bytes_unchanged(self):
        for renderer, folder in ((build_v2_content.render, 'trailguide_v2'),
                                 (build_v3_content.render, 'trailguide_v3')):
            self.assertEqual(renderer(self.data).encode(), (ROOT / folder / 'cards_generated.h').read_bytes())


if __name__ == '__main__':
    unittest.main()
