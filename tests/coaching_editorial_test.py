"""Approved seven-field Coaching editorial scope; no device/build preparation."""
import copy
import hashlib
import json
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from build_compass_coaching import validate, render as browser_render
from build_v3_coaching import render as native_render


def digest(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':'),
                                    ensure_ascii=False).encode()).hexdigest()


class CoachingEditorial(unittest.TestCase):
    def setUp(self):
        self.expected = json.loads((ROOT / 'tests/coaching_editorial_expected.json').read_text())
        self.layer = json.loads((ROOT / 'content/coaching.json').read_text())
        self.cards = json.loads((ROOT / 'content/cards.json').read_text())
        self.entries = {entry['id']: entry for entry in self.layer['entries']}

    def test_exact_approved_seven_fields(self):
        changes = self.expected['changes']
        self.assertEqual(len(changes), 7)
        self.assertEqual(len({change['id'] for change in changes}), 7)
        self.assertEqual(sum(change['field'] == 'text' for change in changes), 3)
        self.assertEqual(sum(change['field'] == 'guidance' for change in changes), 4)
        for change in changes:
            with self.subTest(entry=change['id'], field=change['field']):
                self.assertEqual(self.entries[change['id']][change['field']], change['after'])

    def test_every_other_field_and_canonical_card_is_unchanged(self):
        restored = copy.deepcopy(self.entries)
        for change in self.expected['changes']:
            restored[change['id']][change['field']] = change['before']
        self.assertEqual([entry['id'] for entry in self.layer['entries']],
                         list(self.expected['entry_hashes']))
        self.assertEqual({key: digest(value) for key, value in restored.items()},
                         self.expected['entry_hashes'])
        metadata = {key: value for key, value in self.layer.items() if key != 'entries'}
        self.assertEqual(digest(metadata), self.expected['metadata_hash'])
        self.assertEqual(hashlib.sha256((ROOT / 'content/cards.json').read_bytes()).hexdigest(),
                         self.expected['cards_file_sha256'])
        self.assertEqual(len(self.entries), 55)
        self.assertEqual(sum('text_ref' in entry for entry in self.entries.values()), 11)
        self.assertTrue(all(entry['review_state'] == 'draft' for entry in self.entries.values()))

    def test_both_generated_consumers_are_current(self):
        resolved = validate(self.layer, self.cards)
        self.assertEqual(len(resolved['entries']), 55)
        self.assertEqual((ROOT / 'trailguide_v3/coaching_generated.h').read_text(),
                         native_render(self.layer, self.cards))
        template = (ROOT / 'prototype/compass_coaching_template.html').read_text()
        self.assertEqual((ROOT / 'prototype/compass-coaching.html').read_text(),
                         browser_render(self.layer, self.cards, template))


if __name__ == '__main__':
    unittest.main()
