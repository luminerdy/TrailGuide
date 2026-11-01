"""Portable V3 experience study: canonical build and real Chromium contracts."""
import copy
import importlib.util
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from build_prototype import IDS, select_content


def builder():
    spec = importlib.util.spec_from_file_location('v3_builder', ROOT / 'tools/build_v3_experience.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class BuildTest(unittest.TestCase):
    def test_cli_check_detects_stale_output_and_invalid_content(self):
        self.assertTrue((ROOT / 'prototype/v3-experience.html').exists())
        command = ['python3', str(ROOT / 'tools/build_v3_experience.py')]
        generated = (ROOT / 'prototype/v3-experience.html').read_bytes()
        self.assertEqual(subprocess.run(command + ['--check'], capture_output=True).returncode, 0)
        self.assertEqual(subprocess.run(command, capture_output=True).returncode, 0)
        self.assertEqual((ROOT / 'prototype/v3-experience.html').read_bytes(), generated)
        (ROOT / 'build').mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as directory:
            output, source = Path(directory) / 'study.html', Path(directory) / 'invalid.json'
            output.write_text('stale')
            check = subprocess.run(command + ['--output', str(output), '--check'], capture_output=True, text=True)
            self.assertNotEqual(check.returncode, 0)
            self.assertIn('stale', check.stderr)
            for invalid in ('{', 'null', '{}'):
                source.write_text(invalid)
                run = subprocess.run(command + ['--content', str(source), '--output', str(output)], capture_output=True, text=True)
                self.assertNotEqual(run.returncode, 0)
                self.assertNotIn('Traceback', run.stderr)
                self.assertEqual(output.read_text(), 'stale')

    def test_strict_canonical_deterministic_safe_build(self):
        self.assertTrue((ROOT / 'tools/build_v3_experience.py').exists(), 'V3 experience builder missing')
        module = builder()
        data = json.loads((ROOT / 'content/cards.json').read_text())
        template = '<script id="content-data" type="application/json">__CONTENT_JSON__</script>'
        rendered = module.render(data, template)
        self.assertEqual(rendered, module.render(copy.deepcopy(data), template))
        embedded = json.loads(re.search(r'application/json">(.*?)</script>', rendered).group(1))
        self.assertEqual(embedded, select_content(data))
        self.assertEqual([c['card_id'] for c in embedded['cards']], list(IDS))
        for scope in ('base', 'ask', 'try'):
            invalid = copy.deepcopy(data)
            card = next(c for c in invalid['cards'] if c['card_id'] == IDS[0])
            target = card if scope == 'base' else card['facilitator']
            target['try_review_state' if scope == 'try' else 'review_state'] = 'reviewed'
            with self.assertRaises(ValueError):
                module.render(invalid, template)
        data['cards'][0]['facilitator']['ask'] = '</script> & < >'
        self.assertNotIn('</script> &', module.render(data, template))
        for bad in ('no marker', '__CONTENT_JSON____CONTENT_JSON__'):
            with self.assertRaises(ValueError):
                module.render(data, bad)


class Chromium:
    """System Chromium pipe CDP, matching the existing prototype test transport."""
    def __enter__(self):
        import time
        self.time = time
        self.chrome = subprocess.Popen([
            '/bin/sh', '-c', 'exec "$@" 3<&0 4>&1', 'chromium', '/usr/bin/chromium',
            '--headless', '--no-sandbox', '--disable-gpu', '--disable-dev-shm-usage',
            '--no-proxy-server', '--remote-debugging-pipe'], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        self.sequence, self.pending, self.events = 0, b'', []
        target = self.call('Target.createTarget', {'url': 'about:blank'})['targetId']
        self.session = self.call('Target.attachToTarget', {'targetId': target, 'flatten': True})['sessionId']
        self.call('Runtime.enable', session=self.session)
        self.call('Log.enable', session=self.session)
        self.call('Emulation.setDeviceMetricsOverride', dict(width=900, height=760, deviceScaleFactor=1, mobile=False), self.session)
        return self

    def call(self, method, params=None, session=None):
        import select
        self.sequence += 1
        message = dict(id=self.sequence, method=method, params=params or {})
        if session:
            message['sessionId'] = session
        self.chrome.stdin.write(json.dumps(message).encode() + b'\0')
        self.chrome.stdin.flush()
        deadline = self.time.monotonic() + 20
        while self.time.monotonic() < deadline:
            if b'\0' not in self.pending:
                if not select.select([self.chrome.stdout], [], [], .1)[0]:
                    continue
                chunk = os.read(self.chrome.stdout.fileno(), 65536)
                if not chunk:
                    raise AssertionError('Chromium pipe closed')
                self.pending += chunk
                continue
            raw, self.pending = self.pending.split(b'\0', 1)
            response = json.loads(raw)
            if 'method' in response:
                self.events.append(response)
            if response.get('id') == self.sequence:
                if 'error' in response:
                    raise AssertionError(response['error'])
                return response.get('result', {})
        raise AssertionError('CDP timeout: ' + method)

    def evaluate(self, expression):
        result = self.call('Runtime.evaluate', {'expression': expression, 'returnByValue': True, 'awaitPromise': True}, self.session)
        if 'exceptionDetails' in result:
            raise AssertionError(result['exceptionDetails'])
        return result['result'].get('value')

    def load(self, reduced=False, page=None):
        self.call('Emulation.setEmulatedMedia', {'features': [{'name': 'prefers-reduced-motion', 'value': 'reduce' if reduced else 'no-preference'}]}, self.session)
        self.call('Page.navigate', {'url': (page or ROOT / 'prototype/v3-experience.html').as_uri()}, self.session)
        for _ in range(200):
            if self.evaluate("document.readyState==='complete' && !!window.v3Experience"):
                return
            self.time.sleep(.01)
        raise AssertionError('V3 experience did not initialize')

    def click(self, identifier):
        return self.evaluate('document.getElementById(' + json.dumps(identifier) + ').click()')

    def state(self):
        return self.evaluate('window.v3Experience.snapshot()')

    def __exit__(self, *args):
        self.chrome.terminate()
        self.chrome.wait(timeout=10)
        self.chrome.stdin.close()
        self.chrome.stdout.close()


@unittest.skipUnless(Path('/usr/bin/chromium').exists(), 'system Chromium unavailable')
class BrowserTest(unittest.TestCase):
    def setUp(self):
        self.assertTrue((ROOT / 'prototype/v3_experience_template.html').exists(), 'separate V3 template missing')
        run = subprocess.run(['python3', str(ROOT / 'tools/build_v3_experience.py')], capture_output=True, text=True)
        self.assertEqual(run.returncode, 0, run.stderr)
        self.chrome = Chromium().__enter__()
        self.addCleanup(self.chrome.__exit__)
        self.chrome.load()

    def tearDown(self):
        if hasattr(self, 'chrome'):
            self.chrome.evaluate('true')  # Drain prior runtime/log events.
            errors = [e for e in self.chrome.events if e['method'] == 'Runtime.exceptionThrown' or
                      (e['method'] == 'Log.entryAdded' and e['params']['entry']['level'] == 'error')]
            self.assertEqual(errors, [], 'browser script/console errors')

    def test_fair_cycles_both_modes_complete_canonical_text_and_boundary(self):
        c = self.chrome
        canonical = {x['card_id']: x for x in select_content(json.loads((ROOT / 'content/cards.json').read_text()))['cards']}
        c.click('instant')
        count = 0
        for mode in ('ask', 'try'):
            c.evaluate('Math.random=()=>0')
            c.click(mode)
            for cycle in range(4):
                seen = []
                for index in range(8):
                    if cycle or index:
                        c.click('simulate-shake' if index % 2 else 'another')
                    s = c.state()
                    count += 1
                    self.assertEqual((s['mode'], s['draw_count']), (mode, count))
                    seen.append(s['card_id'])
                    text = canonical[s['card_id']]['facilitator']['ask'] if mode == 'ask' else canonical[s['card_id']]['action']
                    self.assertEqual(s['visible_text'], text)
                    self.assertEqual(c.evaluate("document.getElementById('body-copy').textContent"), text)
                    self.assertEqual(c.evaluate("document.getElementById('complete-copy').textContent"), text)
                    self.assertTrue(c.evaluate("(()=>{const e=document.getElementById('body-copy');e.scrollTop=e.scrollHeight;return e.scrollHeight-e.clientHeight-e.scrollTop<=1})()"))
                    if cycle or index:
                        self.assertNotEqual(s['card_id'], last, 'immediate repeat at a cycle boundary')
                    last = s['card_id']
                self.assertEqual(set(seen), set(IDS))
                self.assertEqual(len(seen), len(set(seen)))
                # First shuffle ends with the original first ID; identity shuffle would
                # repeat it here unless the explicit boundary safeguard runs.
                c.evaluate('Math.random=()=>0.999999')
    def test_complete_overflow_fixture_reachable_by_keyboard(self):
        c = self.chrome
        data = json.loads((ROOT / 'content/cards.json').read_text())
        long_text = ('SYNTHETIC OVERFLOW FIXTURE — NOT A DECK CARD. ' +
                     'Read the whole instruction without reducing the font. ' * 18 +
                     'Participation is optional; keep this FINAL QUALIFIER.')
        for card in data['cards']:
            if card['card_id'] in IDS:
                card['action'] = long_text
        (ROOT / 'build').mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=ROOT / 'build') as directory:
            page = Path(directory) / 'synthetic-overflow.html'
            page.write_text(builder().render(data, (ROOT / 'prototype/v3_experience_template.html').read_text()))
            c.load(reduced=True, page=page)
            c.click('try')
            self.assertEqual(c.state()['full_text'], long_text)
            dimensions = c.evaluate("(()=>{const e=document.getElementById('body-copy');e.scrollTop=0;e.focus();return {full:e.scrollHeight,visible:e.clientHeight,font:getComputedStyle(e).fontSize}})()")
            self.assertEqual(dimensions['font'], '20px')
            self.assertGreater(dimensions['full'], dimensions['visible'])
            for kind in ('keyDown', 'keyUp'):
                c.call('Input.dispatchKeyEvent', dict(type=kind,key='End',code='End',windowsVirtualKeyCode=35), c.session)
            c.time.sleep(.3)
            self.assertTrue(c.evaluate("(()=>{const e=document.getElementById('body-copy');return e.scrollTop>0 && e.scrollHeight-e.scrollTop-e.clientHeight<=1})()"), 'complete prompt scrolls to its final qualifier by keyboard')
            c.evaluate("document.querySelector('#full-copy summary').click()")
            self.assertTrue(c.evaluate("document.getElementById('full-copy').open"))
            self.assertEqual(c.evaluate("document.getElementById('complete-copy').textContent"), long_text)

    def test_circle_geometry_narrow_pan_and_real_pointer_keyboard(self):
        c = self.chrome
        c.click('instant')
        c.click('ask')
        for width in (360, 320, 900):
            c.call('Emulation.setDeviceMetricsOverride', dict(width=width, height=900, deviceScaleFactor=1, mobile=False), c.session)
            geometry = c.evaluate('''(() => {
              const d=document.getElementById('dial'),r=d.getBoundingClientRect(),scroll=document.getElementById('device-scroll');
              const buttons=[...d.querySelectorAll('button')];
              return {width:r.width,height:r.height,left:r.left,transform:getComputedStyle(d).transform,
                targets:buttons.every(b=>{const z=b.getBoundingClientRect();return z.width>=48 && z.height>=48 &&
                  [z.left-4,z.right+4].every(x=>[z.top-4,z.bottom+4].every(y=>Math.hypot(x-r.left-180,y-r.top-180)<=168)) &&
                  b.scrollWidth<=b.clientWidth && b.scrollHeight<=b.clientHeight}),
                font:getComputedStyle(document.getElementById('body-copy')).fontSize,
                noPageOverflow:document.documentElement.scrollWidth===document.documentElement.clientWidth,
                scrollable:scroll.scrollWidth>scroll.clientWidth,tab:scroll.tabIndex,
                hint:getComputedStyle(document.getElementById('viewport-note')).display};
            })()''')
            self.assertEqual((geometry['width'], geometry['height'], geometry['transform'], geometry['font']), (360, 360, 'none', '20px'))
            self.assertGreaterEqual(geometry['left'], 0)
            self.assertTrue(geometry['targets'], geometry)
            self.assertTrue(geometry['noPageOverflow'])
            if width == 320:
                self.assertTrue(geometry['scrollable'])
                self.assertEqual(geometry['tab'], 0)
                self.assertNotEqual(geometry['hint'], 'none')
                self.assertTrue(c.evaluate("(()=>{const e=document.getElementById('device-scroll');e.scrollLeft=e.scrollWidth;return e.scrollLeft>0 && document.getElementById('dial').getBoundingClientRect().right<=innerWidth})()"))
                c.evaluate("document.getElementById('device-scroll').scrollLeft=0")
        point = c.evaluate("(()=>{const b=document.getElementById('another').getBoundingClientRect(),d=document.getElementById('dial').getBoundingClientRect();return {x:b.x+b.width/2,y:b.y+b.height/2,out:d.right+12,cornerX:d.left+2,cornerY:d.top+2}})()")
        def mouse(kind, x, y, held=False):
            c.call('Input.dispatchMouseEvent', dict(type=kind,x=x,y=y,button='none' if kind=='mouseMoved' else 'left',buttons=1 if held else 0,clickCount=0 if kind=='mouseMoved' else 1),c.session)
        before = c.state()['draw_count']
        mouse('mousePressed', point['x'], point['y'], True)
        mouse('mouseMoved', point['out'], point['y'], True)
        mouse('mouseMoved', point['x'], point['y'], True)
        mouse('mouseReleased', point['x'], point['y'])
        self.assertEqual(c.state()['draw_count'], before, 'drag out and back must not draw')
        mouse('mousePressed', point['cornerX'], point['cornerY'], True)
        mouse('mouseReleased', point['cornerX'], point['cornerY'])
        self.assertEqual(c.state()['draw_count'], before)
        mouse('mousePressed', point['x'], point['y'], True)
        mouse('mouseReleased', point['x'], point['y'])
        self.assertEqual(c.state()['draw_count'], before + 1)
        c.evaluate("document.getElementById('another').focus()")
        for kind in ('keyDown', 'keyUp'):
            c.call('Input.dispatchKeyEvent', dict(type=kind,key='Enter',code='Enter',text='\r' if kind=='keyDown' else '',windowsVirtualKeyCode=13), c.session)
        self.assertEqual(c.state()['draw_count'], before + 2)
        # Actual touch events, not DOM button stubs.
        c.call('Emulation.setTouchEmulationEnabled', {'enabled':True}, c.session)
        c.call('Input.dispatchTouchEvent', {'type':'touchStart','touchPoints':[{'x':point['x'],'y':point['y']}]}, c.session)
        c.call('Input.dispatchTouchEvent', {'type':'touchEnd','touchPoints':[]}, c.session)
        self.assertEqual(c.state()['draw_count'], before + 3)

    def test_bounded_reveal_busy_gate_and_same_card_skip(self):
        c = self.chrome
        c.click('ask')
        first = c.state()
        self.assertEqual((first['phase'], first['visible_text'], first['draw_count']), ('settling', '', 1))
        self.assertTrue(c.evaluate("document.getElementById('another').disabled && !document.getElementById('reveal-all').disabled"))
        c.evaluate("for(let i=0;i<20;i++){document.getElementById('simulate-shake').click();document.getElementById('another').click()}")
        c.time.sleep(.4)
        progress = c.state()
        self.assertEqual(progress['phase'], 'revealing')
        self.assertTrue(progress['full_text'].startswith(progress['visible_text']))
        self.assertGreater(len(progress['visible_text']), 0)
        self.assertLess(len(progress['visible_text']), len(progress['full_text']))
        self.assertEqual(progress['draw_count'], 1)
        c.click('reveal-all')
        complete = c.state()
        self.assertEqual((complete['card_id'], complete['draw_count']), (first['card_id'], 1))
        self.assertEqual(complete['visible_text'], complete['full_text'])
        self.assertEqual(complete['phase'], 'ready')
        start = c.time.monotonic()
        c.click('simulate-shake')
        self.assertEqual(c.state()['draw_count'], 2)
        while c.state()['phase'] != 'ready' and c.time.monotonic() - start < 4:
            c.time.sleep(.04)
        elapsed = c.time.monotonic() - start
        self.assertGreater(elapsed, 1)
        self.assertLess(elapsed, 3.4)
        self.assertEqual(c.state()['visible_text'], c.state()['full_text'])
        c.time.sleep(.25)
        self.assertEqual(c.state()['draw_count'], 2, 'busy shakes must not queue')

    def test_cancel_mode_change_stale_callbacks_and_instant_toggle(self):
        c = self.chrome
        c.click('ask')
        c.time.sleep(.3)
        c.click('try')
        destination = c.state()
        self.assertEqual((destination['mode'], destination['draw_count'], destination['phase']), ('try', 2, 'settling'))
        c.click('instant')
        instant = c.state()
        self.assertEqual((instant['card_id'], instant['draw_count']), (destination['card_id'], 2))
        self.assertEqual((instant['phase'], instant['visible_text']), ('ready', instant['full_text']))
        c.time.sleep(3.3)
        self.assertEqual(c.state(), instant, 'canceled Ask and Try callbacks cannot overwrite completed text')
        c.click('instant')
        self.assertEqual(c.state()['draw_count'], 2)
        self.assertEqual(c.state()['visible_text'], instant['full_text'])
        c.click('another')
        c.click('cancel-run')
        canceled = c.state()
        self.assertEqual(canceled['phase'], 'canceled')
        c.evaluate("for(let i=0;i<10;i++)document.getElementById('simulate-shake').click()")
        c.time.sleep(3.3)
        self.assertEqual(c.state(), canceled, 'canceled run cannot draw or continue revealing')
        c.click('try')
        self.assertEqual((c.state()['phase'], c.state()['draw_count']), ('settling', 4))

    def test_os_reduced_motion_is_immediate_without_extra_draw(self):
        c = self.chrome
        c.load(reduced=True)
        self.assertTrue(c.state()['reduced'])
        self.assertTrue(c.evaluate("document.getElementById('instant-input').checked"))
        c.click('ask')
        s = c.state()
        self.assertEqual((s['phase'], s['draw_count'], s['visible_text']), ('ready', 1, s['full_text']))
        c.click('instant')
        self.assertEqual(c.state()['draw_count'], 1)
        self.assertEqual(c.state()['visible_text'], s['full_text'])

    def test_fresh_mode_draw_and_canonical_main_copy(self):
        c = self.chrome
        self.assertEqual(c.state(), dict(phase='idle', mode=None, card_id=None, draw_count=0,
                                        full_text='', visible_text='', reduced=False))
        c.click('simulate-shake')
        self.assertEqual(c.state()['draw_count'], 0)
        canonical = {x['card_id']: x for x in select_content(json.loads((ROOT / 'content/cards.json').read_text()))['cards']}
        c.click('instant')
        for count, mode in enumerate(('ask', 'try'), 1):
            c.click(mode)
            s = c.state()
            self.assertEqual((s['mode'], s['draw_count'], s['phase']), (mode, count, 'ready'))
            card = canonical[s['card_id']]
            self.assertEqual(s['full_text'], card['facilitator']['ask'] if mode == 'ask' else card['action'])
            self.assertEqual(s['visible_text'], s['full_text'])
            self.assertEqual(c.evaluate("document.getElementById('body-copy').textContent"), s['full_text'])
        self.assertTrue(c.evaluate('Object.isFrozen(window.v3Experience) && Object.isFrozen(window.v3Experience.snapshot())'))
        self.assertTrue(c.evaluate("document.body.textContent.includes('Draft experience study') && document.body.textContent.includes('not V3 firmware')"))
        html = (ROOT / 'prototype/v3-experience.html').read_text()
        for forbidden in ('fetch(', 'XMLHttpRequest', 'WebSocket', 'localStorage', 'sessionStorage', 'indexedDB',
                          'DeviceMotionEvent', 'DeviceOrientationEvent', 'Accelerometer', 'Gyroscope', 'https://', 'http://', '@import', 'src='):
            self.assertNotIn(forbidden, html)
        self.assertEqual(c.evaluate('performance.getEntriesByType("resource").length'), 0)


if __name__ == '__main__':
    unittest.main()
