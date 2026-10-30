"""Content contract tests; run: python3 -m unittest discover -s tests -p '*_test.py'."""
import collections
import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
THEMES = {'explore', 'clarify', 'experiment', 'verify', 'reflect', 'unstick', 'include', 'decide'}
SITUATIONS = {'too-many-ideas', 'nobodys-talking', 'ai-sounds-convincing', 'out-of-time'}


class ContentTest(unittest.TestCase):
    def test_complete_catalog_contract(self):
        path = ROOT / 'content/cards.json'
        self.assertTrue(path.is_file(), 'The expanded structured card catalog is missing')
        data = json.loads(path.read_text())
        self.assertEqual(data['schema_version'], 1)
        self.assertEqual({t['theme_id'] for t in data['themes']}, THEMES)
        self.assertEqual(len(data['themes']), 8)
        cards = data['cards']
        self.assertEqual(len(cards), 40)
        self.assertEqual(collections.Counter(c['theme'] for c in cards), {t: 5 for t in THEMES})
        self.assertEqual(len({c['card_id'] for c in cards}), 40)
        self.assertEqual(len({c['title'] for c in cards}), 40)
        for card in cards:
            with self.subTest(card=card['card_id']):
                self.assertRegex(card['card_id'], r'^tg-\d{3}$')
                self.assertIs(type(card['revision']), int)
                self.assertGreaterEqual(card['revision'], 1)
                self.assertEqual(card['review_state'], 'draft')
                self.assertIs(type(card['legacy_v1']), bool)
                for key, limit in [('title', 24), ('action', 160), ('question', 180), ('activity', 500)]:
                    self.assertIsInstance(card[key], str)
                    self.assertTrue(card[key].strip())
                    self.assertLessEqual(len(card[key]), limit)
                self.assertIs(type(card['minutes']), int)
                self.assertTrue(1 <= card['minutes'] <= 10)
                self.assertIsInstance(card['situation_tags'], list)
                self.assertTrue(set(card['situation_tags']) <= SITUATIONS)
                self.assertEqual(len(card['situation_tags']), len(set(card['situation_tags'])))
                self.assertIsInstance(card['ai_prompt'], str)
                self.assertLessEqual(len(card['ai_prompt']), 600)
        for situation in SITUATIONS:
            self.assertTrue(any(situation in c['situation_tags'] for c in cards), situation)
        sketch = (ROOT / 'oled_oracle/oled_oracle.ino').read_text()
        block = sketch.split('const Card cards[] = {', 1)[1].split('};', 1)[0]
        legacy = re.findall(r'\{"([^"\n]+)",\s*"((?:[^"\\]|\\.)*)"\}', block)
        self.assertEqual(len(legacy), 12)
        preserved = {c['title']: c for c in cards if c['legacy_v1']}
        self.assertEqual(len(preserved), 12)
        for index, (title, body) in enumerate(legacy, 1):
            self.assertEqual(preserved[title]['card_id'], f'tg-{index:03d}')
            self.assertEqual(preserved[title]['action'], body.replace('\\n', ' '))
        catalog = (ROOT / 'docs/CARD-CATALOG.md').read_text()
        self.assertEqual(len(re.findall(r'^### tg-\d{3} ', catalog, re.M)), 40)
        for card in cards:
            for key in ['card_id', 'title', 'action', 'question', 'activity']:
                self.assertIn(card[key], catalog)
            if card['ai_prompt']:
                self.assertIn(card['ai_prompt'], catalog)


if __name__ == '__main__':
    unittest.main()
