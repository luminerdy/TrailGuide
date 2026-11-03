"""Approved opening content and isolated V3 selection, not release approval."""
import copy
import json
from pathlib import Path
import sys
import unittest
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import build_v2_content as shared
from build_prototype import select_content

class V3ContentTest(unittest.TestCase):
    def setUp(self):
        self.data = json.loads((ROOT / 'content/cards.json').read_text())
        self.expected = json.loads((ROOT / 'tests/approved_openings.json').read_text())
        self.cards = {c['card_id']: c for c in self.data['cards']}

    def test_exact_approved_openers_private_cues_and_help(self):
        for card_id, ask, cue, follow, help_text in self.expected['cards']:
            with self.subTest(card=card_id):
                c = self.cards[card_id]
                f = c.get('facilitator', {})
                self.assertEqual(f.get('ask'), ask)
                self.assertEqual(f.get('notice'), [cue])
                self.assertEqual(f.get('follow_ups'), [follow])
                self.assertEqual(f.get('optional_ai_help'), help_text)
                self.assertEqual(c['review_state'], 'draft')
                self.assertEqual(f.get('review_state'), 'draft')
                self.assertEqual(f.get('try_review_state'), 'draft')
                guide, move = shared.build_guides(c)
                self.assertNotIn(ask, guide)
                self.assertIn(shared.display_copy(help_text), guide)
                self.assertIn(c['activity'], move)
                self.assertTrue(f['try_invitation'].endswith('?'))
        f = self.cards['tg-028']['facilitator']
        condition = self.expected['conditional_follow_up']
        self.assertEqual(f.get('conditional_follow_ups'), [{'when': condition['when'], 'question': condition['question']}])
        guide, _ = shared.build_guides(self.cards['tg-028'])
        self.assertIn(condition['when'] + ':\n' + condition['question'], guide)

    def test_v3_twelve_and_default_eight_are_separate_validated_selections(self):
        self.assertTrue((ROOT/'tools/build_v3_content.py').is_file(), 'V3 needs its own validated twelve-card generator')
        import build_v3_content as v3
        ids = tuple(row[0] for row in self.expected['cards'])
        self.assertEqual(v3.IDS, ids)
        selected = select_content(self.data, ids=ids)
        self.assertEqual([c['card_id'] for c in selected['cards']], list(ids))
        self.assertEqual(len(select_content(self.data)['cards']), 8)
        for builder, count in [(shared, 8), (v3, 12)]:
            output = builder.render(self.data)
            self.assertEqual(sum(line.startswith('  {') for line in output.splitlines()), count)
            self.assertIn('cardCount = sizeof(cards) / sizeof(cards[0])', output)
        self.assertEqual(v3.render(self.data), (ROOT/'trailguide_v3/cards_generated.h').read_text())
        for ids_bad in [(), ('tg-001', 'tg-001'), ('tg-099',)]:
            with self.assertRaises(ValueError):
                select_content(self.data, ids=ids_bad)
        for field, invalid in [('optional_ai_help', ''), ('optional_ai_help', 7), ('conditional_follow_ups', [{'question':'Ungated?'}]), ('conditional_follow_ups', [{'when':'', 'question':'Why?'}])]:
            bad = copy.deepcopy(self.data)
            next(c for c in bad['cards'] if c['card_id']=='tg-028')['facilitator'][field] = invalid
            with self.assertRaises(ValueError):
                v3.render(bad)
        bad = copy.deepcopy(self.data)
        next(c for c in bad['cards'] if c['card_id']=='tg-026')['facilitator']['review_state']='reviewed'
        with self.assertRaises(ValueError): v3.render(bad)

    def test_checking_and_human_only_activities_do_not_contradict_openers(self):
        checking = self.cards['tg-003']['activity']
        self.assertNotIn('independently of AI', checking)
        self.assertIn('AI', checking)
        self.assertIn('evidence', checking)
        human = self.cards['tg-014']
        self.assertIn('If useful', human['activity'])
        self.assertIn('complete', human['activity'])
        self.assertNotIn('Then name one place AI might help.', human['action'])

if __name__ == '__main__': unittest.main()
