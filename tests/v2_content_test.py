import importlib.util
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class V2ContentTest(unittest.TestCase):
    def test_vendor_panel_extraction_is_explicit_and_rejects_unknown_format(self):
        path = ROOT / 'tools/prepare_v2_panel.py'
        self.assertTrue(path.exists(), 'V2 panel preparation is not implemented')
        spec = importlib.util.spec_from_file_location('v2_panel', path)
        assert spec is not None and spec.loader is not None
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        function = 'void lcd_reg_init(void) {\n  bus->batchOperation(nullptr, 0);\n}\n'
        self.assertIn(function, module.extract(function + '\nuint32_t screenWidth;'))
        with self.assertRaises(ValueError):
            module.extract('unknown source')

    def test_guidance_adds_help_instead_of_repeating_the_card(self):
        path = ROOT / 'tools/build_v2_content.py'
        spec = importlib.util.spec_from_file_location('v2_guidance', path)
        assert spec is not None and spec.loader is not None
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        self.assertTrue(hasattr(module, 'build_guides'), 'No progressive guidance formatter')
        data = json.loads((ROOT / 'content/cards.json').read_text())
        cards = {c['card_id']: c for c in data['cards']}
        for card_id in module.IDS:
            c = cards[card_id]
            ask, move = module.build_guides(c)
            self.assertTrue(ask.startswith(c['title'] + '\n\nWhat to listen for\n'))
            self.assertNotIn(c['facilitator']['ask'], ask)
            self.assertTrue(move.startswith(c['title'] + '\n\nHow to run it\n' + c['activity']))
            self.assertNotIn('\n\nSay\n', ask)
            self.assertLess(move.index(c['activity']), move.index(c['facilitator']['try_invitation']))
            for text in [*c['facilitator']['notice'], *c['facilitator']['follow_ups']]:
                self.assertIn(text, ask)
            for text in [*c['facilitator']['try_notice'], *c['facilitator']['try_follow_ups']]:
                self.assertIn(text, move)
            if c['action'] != c['activity']:
                self.assertNotIn(c['action'], move)
            row = next(line for line in module.render(data).splitlines() if line.startswith('  {' + json.dumps(card_id) + ','))
            self.assertIn(json.dumps(ask), row)
            self.assertIn(json.dumps(move), row)
        duplicate = dict(cards[module.IDS[0]])
        duplicate['facilitator'] = dict(duplicate['facilitator'])
        duplicate['facilitator']['try_invitation'] = duplicate['action']
        self.assertNotIn('\n\nSuggested invitation\n', module.build_guides(duplicate)[1])

    def test_exact_canonical_pairs_generated_without_copy_loss(self):
        path = ROOT / 'tools/build_v2_content.py'
        self.assertTrue(path.exists(), 'V2 firmware content builder is not implemented')
        spec = importlib.util.spec_from_file_location('v2_builder', path)
        assert spec is not None and spec.loader is not None
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        data = json.loads((ROOT / 'content/cards.json').read_text())
        result = module.render(data)
        self.assertEqual(result, module.render(data))
        cards = {c['card_id']: c for c in data['cards']}
        for card_id in module.IDS:
            c = cards[card_id]
            for text in [card_id, c['title'], c['action'], c['facilitator']['ask']]:
                self.assertIn(json.dumps(text), result)
            for text in [c['activity'], c['facilitator']['try_invitation'], *c['facilitator']['notice'], *c['facilitator']['follow_ups'], *c['facilitator']['try_notice'], *c['facilitator']['try_follow_ups']]:
                self.assertIn(json.dumps(text)[1:-1], result)
        self.assertEqual(8, len(module.IDS))
        self.assertIn('DRAFT', result)
        self.assertEqual(result, (ROOT / 'trailguide_v2/cards_generated.h').read_text())
