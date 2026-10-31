"""Draft paired-face content checks; these are not editorial approval."""
import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
REPRESENTATIVES = {'tg-001', 'tg-002', 'tg-004', 'tg-003', 'tg-011', 'tg-028', 'tg-031', 'tg-036'}


class FacilitatorContentTest(unittest.TestCase):
    def test_eight_draft_pairs_cover_themes_without_replacing_base_content(self):
        data = json.loads((ROOT / 'content/cards.json').read_text())
        paired = [c for c in data['cards'] if 'facilitator' in c]
        self.assertEqual({c['card_id'] for c in paired}, REPRESENTATIVES,
                         'Eight representative paired faces have not been authored')
        self.assertEqual(len(paired), 8)
        self.assertEqual({c['theme'] for c in paired}, {t['theme_id'] for t in data['themes']})
        catalog = (ROOT / 'docs/CARD-CATALOG.md').read_text()
        for card in paired:
            with self.subTest(card=card['card_id']):
                f = card['facilitator']
                self.assertEqual(set(f), {'ask', 'notice', 'follow_ups', 'try_invitation',
                                        'try_notice', 'try_follow_ups', 'review_state', 'try_review_state'})
                self.assertEqual(card['review_state'], 'draft')
                self.assertEqual(f['review_state'], 'draft')
                self.assertEqual(f['try_review_state'], 'draft')
                self.assertGreaterEqual(card['revision'], 2)
                self.assertTrue(card['action'].strip())
                self.assertTrue(card['activity'].strip())
                section = re.search(r'^### ' + card['card_id'] + r' - .*?(?=^### |^## |\Z)',
                                    catalog, re.M | re.S)
                self.assertIsNotNone(section)
                assert section is not None  # Narrow for static analysis after the test assertion.
                self.assertIn('Revision ' + str(card['revision']) + ' | Draft', section.group())
                for field in ['ask', 'try_invitation']:
                    self.assertIsInstance(f[field], str)
                    self.assertTrue(f[field].endswith('?'))
                    self.assertLessEqual(len(f[field]), 100)
                    self.assertIn(f[field], section.group())
                for field in ['notice', 'follow_ups', 'try_notice', 'try_follow_ups']:
                    self.assertIsInstance(f[field], list)
                    self.assertTrue(1 <= len(f[field]) <= 2)
                    for entry in f[field]:
                        self.assertIsInstance(entry, str)
                        self.assertTrue(entry.strip())
                        self.assertLessEqual(len(entry), 220)
                        self.assertIn(entry, section.group())
                        if field.endswith('follow_ups'):
                            self.assertTrue(entry.endswith('?'))


if __name__ == '__main__':
    unittest.main()
