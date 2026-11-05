"""Keep release runtime cases and actual composed-loop extraction in the canonical runner."""
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import test_v3_app as runner

class ReliabilityRunner(unittest.TestCase):
    def test_composition_matrix_keeps_both_owners_and_fault_recording_modes(self):
        self.assertTrue(callable(getattr(runner, 'reliability_cases', None)), 'Canonical runtime matrix is missing')
        cases = list(runner.reliability_cases())
        self.assertEqual(len(cases), 76)
        self.assertEqual(len({tuple(case) for case in cases}), 76)
        self.assertEqual(sum(case[0] == 'fault' for case in cases), 40)
        for owner in ('0', '1'):
            self.assertIn(['deadline', owner], cases)
            for recording in ('0', '1'):
                self.assertIn(['notready', owner, '0', recording], cases)
                for domain in range(10):
                    self.assertIn(['fault', owner, str(domain), recording], cases)

    def test_loop_extraction_includes_real_audio_service_after_motion_and_lvgl(self):
        self.assertTrue(callable(getattr(runner, 'reliability_loop', None)), 'Actual composed-loop extractor is missing')
        sketch = (ROOT / 'trailguide_v3/trailguide_v3.ino').read_text()
        text = runner.reliability_loop(sketch)
        self.assertEqual(text, sketch[sketch.index('  if (ready) Lvgl_Loop();'):sketch.index('  if (uint32_t(now-last_heartbeat)>=5000)')])
        self.assertLess(text.index('Lvgl_Loop();'), text.index('imu.tick(now);'))
        self.assertLess(text.index('ui.sample('), text.index('v3audio::service('))
        self.assertIn('ui.sound.valid()', text)
        code = (ROOT / 'tools/test_v3_app.py').read_text()
        for filename in ('v3_reliability_fault_dispatch_test.cpp', 'v3_reliability_framing_test.cpp', 'v3_reliability_cue_composition_test.cpp'):
            self.assertIn(filename, code)

if __name__ == '__main__':
    unittest.main()
