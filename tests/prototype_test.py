"""Read-only prototype contract; standard library and optional system Chromium."""
import copy
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
import os

ROOT = Path(__file__).resolve().parents[1]
IDS = ('tg-001', 'tg-002', 'tg-004', 'tg-003', 'tg-011', 'tg-028', 'tg-031', 'tg-036')
THEMES = ('explore', 'clarify', 'experiment', 'verify', 'reflect', 'unstick', 'include', 'decide')


def fixture():
    return {'schema_version': 1, 'themes': [dict(theme_id=t, label=t.title()) for t in THEMES],
            'cards': [dict(card_id=i, revision=n+3, theme=t, title='SYNTHETIC TITLE',
                           action='Try one small step.', activity='Choose a reversible step.', review_state='draft',
                           facilitator=dict(ask='What could we try?', notice=['Listen for one possibility.'],
                                            follow_ups=['What might change?'], try_invitation='Would a small step help?',
                                            try_notice=['Notice the result.'], try_follow_ups=['What did it show?'],
                                            review_state='draft', try_review_state='draft'))
                      for n, (i, t) in enumerate(zip(IDS, THEMES))]}


def builder():
    path = ROOT / 'tools/build_prototype.py'
    spec = importlib.util.spec_from_file_location('prototype_builder', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class PrototypeTest(unittest.TestCase):
    def test_builder_selects_exact_representatives(self):
        self.assertTrue((ROOT / 'tools/build_prototype.py').exists(), 'standard-library prototype builder missing')
        data = fixture()
        data['cards'].append(dict(data['cards'][0], card_id='tg-040'))
        selected = builder().select_content(data)
        self.assertEqual([c['card_id'] for c in selected['cards']], list(IDS))
        self.assertEqual(len({c['theme'] for c in selected['cards']}), 8)
        self.assertEqual(selected['cards'][2]['revision'], 5)

    def test_rejects_duplicate_and_malformed_theme_metadata(self):
        cases = []
        data = fixture()
        data['themes'].append(copy.deepcopy(data['themes'][0]))
        cases.append(('duplicate ninth entry', data))
        for themes in (None, {}, 'explore', [], fixture()['themes'][:-1]):
            data = fixture()
            data['themes'] = themes
            cases.append(('invalid themes container/count', data))
        for entry in (None, [], 'explore', {}, {'theme_id': 'explore'},
                      {'theme_id': [], 'label': 'Explore'},
                      {'theme_id': 'unknown', 'label': 'Unknown'},
                      {'theme_id': 'explore', 'label': ''},
                      {'theme_id': 'explore', 'label': '  '},
                      {'theme_id': 'explore', 'label': 4}):
            data = fixture()
            data['themes'][0] = entry
            cases.append(('malformed theme entry', data))
        for label, data in cases:
            with self.subTest(case=label), self.assertRaises(ValueError):
                builder().select_content(data)

    def test_rejects_invalid_overall_metadata_cleanly(self):
        cases = [None, [], {}, {'schema_version': 1}, dict(fixture(), schema_version=2),
                 dict(fixture(), schema_version=True)]
        for cards in (None, {}, 'cards', [None], [{'card_id': []}]):
            cases.append(dict(fixture(), cards=cards))
        for field, value in (('facilitator', None), ('theme', []), ('card_id', None)):
            data = fixture()
            data['cards'][0][field] = value
            cases.append(data)
        for index, data in enumerate(cases):
            with self.subTest(case=index), self.assertRaises(ValueError):
                builder().select_content(data)
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as temp:
            source, output = Path(temp)/'invalid.json', Path(temp)/'output.html'
            invalid = fixture()
            invalid['themes'].append(copy.deepcopy(invalid['themes'][0]))
            for content in (json.dumps(invalid), 'null', '{'):
                source.write_text(content)
                run = subprocess.run(['python3', str(ROOT/'tools/build_prototype.py'),
                                      '--content', str(source), '--output', str(output)],
                                     capture_output=True, text=True)
                self.assertNotEqual(run.returncode, 0)
                self.assertNotIn('Traceback', run.stderr)
                self.assertIn('Invalid preview content', run.stderr)
                self.assertFalse(output.exists())

    def test_draft_only_and_complete_content_required(self):
        for location in ('base', 'ask', 'try'):
            for state in ('reviewed', 'retired', 'unknown', None):
                data = fixture()
                card = data['cards'][0]
                target, key = (card, 'review_state') if location == 'base' else (card['facilitator'], 'review_state' if location == 'ask' else 'try_review_state')
                target[key] = state
                with self.subTest(location=location, state=state), self.assertRaises(ValueError):
                    builder().select_content(data)
        for key in ('ask', 'notice', 'follow_ups', 'try_invitation', 'try_notice', 'try_follow_ups'):
            data = fixture()
            del data['cards'][0]['facilitator'][key]
            with self.subTest(key=key), self.assertRaises(ValueError):
                builder().select_content(data)

    def test_safe_reproducible_offline_render(self):
        module = builder()
        self.assertTrue(hasattr(module, 'render'), 'portable renderer missing')
        data = fixture()
        data['cards'][0]['facilitator']['ask'] = '</script><script>bad()</script> & < > \u2028'
        template = (ROOT / 'prototype/template.html').read_text()
        html = module.render(data, template)
        self.assertEqual(html, module.render(copy.deepcopy(data), template))
        match = re.search(r'<script id="content-data" type="application/json">(.*?)</script>', html, re.S)
        self.assertIsNotNone(match)
        decoded = json.loads(match.group(1))
        self.assertEqual(decoded['cards'][0]['facilitator']['ask'], data['cards'][0]['facilitator']['ask'])
        self.assertNotIn('<script>bad()', html)
        for forbidden in ('fetch(', 'XMLHttpRequest', 'WebSocket', 'localStorage', 'sessionStorage', 'indexedDB', 'setTimeout', 'setInterval', 'https://', 'http://', '@import', 'src='):
            self.assertNotIn(forbidden, html)
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as temp:
            source, output = Path(temp)/'source.json', Path(temp)/'output.html'
            source.write_text(json.dumps(fixture()))
            command = ['python3', str(ROOT/'tools/build_prototype.py'), '--content', str(source), '--output', str(output)]
            self.assertEqual(subprocess.run(command, capture_output=True).returncode, 0)
            self.assertEqual(subprocess.run(command+['--check'], capture_output=True).returncode, 0)
            output.write_text('stale')
            self.assertNotEqual(subprocess.run(command+['--check'], capture_output=True).returncode, 0)

    @unittest.skipUnless(Path('/usr/bin/chromium').exists(), 'system Chromium unavailable')
    def test_browser_navigation_geometry_and_fixtures(self):
        template = (ROOT/'prototype/template.html').read_text()
        probe = r'''<script>
        try {
          const check=(v,m)=>{if(!v)throw Error(m)};
          const click=id=>{const b=document.getElementById(id);check(b,'missing '+id);b.click()};
          const state=()=>window.preview.snapshot();
          check(document.getElementById('dial'),'360px circular interface missing');
          check(state().view==='home' && state().draws===0,'fresh Home draws nothing');
          click('ask'); check(state().mode==='ask' && state().draws===1,'explicit Ask draw');
          const first=state();
          click('team');click('team');click('back');
          check(JSON.stringify(state())===JSON.stringify(first),'double team open Back preserves original card/mode/draw');
          click('guide'); check(state().view==='guide','one-tap Guide');
          click('next');const guideBefore=state();
          click('team');click('team');click('choice-1');
          check(JSON.stringify(state())===JSON.stringify({...guideBefore,team:'T1'}),'guide team change preserves page/card/mode/draw');
          click('previous');
          check(getComputedStyle(document.getElementById('body-copy')).fontSize==='20px','readable guide font');
          check(getComputedStyle(document.getElementById('body-copy')).lineHeight==='26px','guide lines');
          click('next'); click('back'); check(state().card===first.card && state().draws===1,'guide preserves card');
          click('more'); check(document.getElementById('record').disabled && document.getElementById('save').disabled,'no recording or saves');
          click('related'); check(state().mode==='try' && state().card===first.card && state().revision===first.revision && state().draws===1,'same revision related without draw');
          click('more'); click('related');
          const seen=new Set([state().card]);
          for(let i=0;i<7;i++){click('another');check(!seen.has(state().card),'no repeat within mode pool');seen.add(state().card)}
          const last=state().card;click('another');check(state().card!==last,'cycle boundary repeat');
          click('more');click('page');click('themes');click('choice-1');
          check(state().filter==='explore','theme choice');
          const themed=state();click('more');click('page');click('themes');click('choice-1');
          check(state().card===themed.card && state().draws===themed.draws,'reselect cancels draw');
          click('team');click('choice-1');check(state().team==='T1' && state().card===themed.card,'synthetic team only');
          click('guide');click('next');click('team');click('choice-2');click('back');
          check(state().team==='T2' && state().filter==='explore' && state().card===themed.card,'guide return context');
          click('another');check(state().card===themed.card,'one-card pool may repeat');
          const before=JSON.stringify(state());
          const dial=document.getElementById('dial'), rect=dial.getBoundingClientRect();
          for(const type of ['pointerdown','pointerup','click'])dial.dispatchEvent(new MouseEvent(type,{bubbles:true,clientX:rect.x+1,clientY:rect.y+1,detail:1}));
          check(JSON.stringify(state())===before,'corner tap does nothing');
          const geometry=()=>{for(const e of dial.querySelectorAll('button,[data-envelope]')){const r=e.getBoundingClientRect();for(const x of [r.left-rect.left,r.right-rect.left])for(const y of [r.top-rect.top,r.bottom-rect.top])check(Math.hypot(x-180,y-180)<=168.1,'unsafe envelope '+e.id);if(e.tagName==='BUTTON')check(e.scrollWidth<=e.clientWidth && e.scrollHeight<=e.clientHeight,'button label fit '+e.id)}};
          geometry();click('more');geometry();click('page');geometry();click('themes');geometry();click('page');geometry();click('page');geometry();click('back');click('back');
          click('fixture-long');check(state().fixture==='long','separate long fixture');
          check(document.getElementById('continuation'),'explicit full text route');click('guide');
          let all=[];let total=window.preview.guidePages().length;
          for(let i=0;i<total;i++){geometry();all.push(document.getElementById('body-copy').textContent);if(i<total-1)click('next')}
          check(all.join(' ').includes('FINAL QUALIFIER'),'complete long-copy pagination');
          click('back');click('more');check(document.getElementById('record').disabled,'fixture not recordable');
          click('fixture-unavailable');click('more');check(document.getElementById('related').disabled,'unavailable related disabled');
          click('fixture-empty');click('ask');check(state().view==='empty' && state().card===null,'empty pool honest');
          click('reset');check(state().view==='home' && state().draws===0 && state().team===null,'reset ephemeral');
          click('trail');check(document.getElementById('body-copy').textContent.includes('unavailable'),'honest Team Trail');
          document.body.insertAdjacentHTML('beforeend','<pre id="result">BROWSER PASS</pre>');
        } catch(e) {document.body.insertAdjacentHTML('beforeend','<pre id="result">FAIL '+e.message+'</pre>')}
        </script>'''
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as temp:
            page = Path(temp)/'browser.html'
            rendered = builder().render(fixture(), template)
            page.write_text(rendered.replace('</body>',probe+'</body>') if '</body>' in rendered else rendered+probe)
            run = subprocess.run(['/usr/bin/chromium','--headless','--no-sandbox','--disable-gpu','--disable-dev-shm-usage','--no-proxy-server','--dump-dom',page.as_uri()],capture_output=True,text=True,timeout=60)
            from html.parser import HTMLParser
            class ResultParser(HTMLParser):
                value = ''
                active = False
                def handle_starttag(self, tag, attrs):
                    self.active = tag == 'pre' and ('id', 'result') in attrs
                def handle_data(self, data):
                    if self.active:
                        self.value += data
                def handle_endtag(self, tag):
                    if tag == 'pre':
                        self.active = False
            result = ResultParser()
            result.feed(run.stdout)
            self.assertEqual(result.value,'BROWSER PASS', run.stderr[-1500:])

    @unittest.skipUnless(Path('/usr/bin/chromium').exists(), 'system Chromium unavailable')
    def test_viewport_edges_reachable_without_scaling(self):
        import select
        import time
        # Pipe CDP avoids Chromium's 500px minimum headless window width.
        command = ['/bin/sh', '-c', 'exec "$@" 3<&0 4>&1', 'chromium',
                   '/usr/bin/chromium', '--headless', '--no-sandbox', '--disable-gpu',
                   '--disable-dev-shm-usage', '--remote-debugging-pipe']
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as temp:
            page = Path(temp)/'responsive.html'
            page.write_text(builder().render(fixture(), (ROOT/'prototype/template.html').read_text()))
            with subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                  stderr=subprocess.DEVNULL) as chrome:
                assert chrome.stdin is not None and chrome.stdout is not None
                sequence, pending = 0, b''
                def call(method, params=None, session=None):
                    nonlocal sequence, pending
                    sequence += 1
                    message = dict(id=sequence, method=method, params=params or {})
                    if session:
                        message['sessionId'] = session
                    chrome.stdin.write(json.dumps(message).encode()+b'\0')
                    chrome.stdin.flush()
                    deadline = time.monotonic()+30
                    while time.monotonic() < deadline:
                        if b'\0' not in pending:
                            self.assertTrue(select.select([chrome.stdout], [], [], 1)[0] or chrome.poll() is None, 'Chromium exited')
                            if not select.select([chrome.stdout], [], [], 0)[0]:
                                continue
                            chunk = os.read(chrome.stdout.fileno(), 65536)
                            self.assertTrue(chunk, 'CDP pipe closed')
                            pending += chunk
                            continue
                        raw, pending = pending.split(b'\0', 1)
                        response = json.loads(raw)
                        if response.get('id') == sequence:
                            self.assertNotIn('error', response)
                            return response.get('result', {})
                    self.fail('CDP response timeout: '+method)
                try:
                    target = call('Target.createTarget', {'url': 'about:blank'})['targetId']
                    session = call('Target.attachToTarget', {'targetId': target, 'flatten': True})['sessionId']
                    for width in (320, 360, 390, 1280):
                        with self.subTest(width=width):
                            call('Emulation.setDeviceMetricsOverride', dict(width=width, height=900, deviceScaleFactor=1, mobile=False), session)
                            call('Page.navigate', {'url': page.as_uri()}, session)
                            for _ in range(100):
                                ready = call('Runtime.evaluate', {'expression': "document.readyState==='complete' && !!window.preview", 'returnByValue': True}, session)
                                if ready['result'].get('value'):
                                    break
                                time.sleep(.01)
                            probe = r'''(() => {
                              const check=(v,m)=>{if(!v)throw Error(m)};
                              check(innerWidth===WIDTH,'actual viewport '+innerWidth);
                              const dial=document.getElementById('dial'), box=dial.getBoundingClientRect();
                              check(box.width===360 && box.height===360,'preserve logical size');
                              check(box.left>=0,'left circle edge unreachable: '+box.left);
                              check(getComputedStyle(dial).transform==='none','no scale-down');
                              const review=document.querySelector('.review');
                              for(const e of [review,...review.querySelectorAll('button')]){
                                const r=e.getBoundingClientRect();
                                check(r.left>=0 && r.right<=innerWidth,'review outside viewport');
                                check(e.scrollWidth<=e.clientWidth,'review text overflow');
                              }
                              if(innerWidth<360){
                                const hint=document.getElementById('viewport-note');
                                check(hint && getComputedStyle(hint).display!=='none' && hint.textContent.includes('360') && /scroll/i.test(hint.textContent),'visible narrow viewport explanation');
                                const scroll=document.getElementById('device-scroll');
                                check(scroll && scroll.clientWidth<=innerWidth && scroll.scrollWidth>=360,'horizontal scroll container');
                                check(getComputedStyle(scroll).overflowX==='auto' || getComputedStyle(scroll).overflowX==='scroll','scroll enabled');
                                check([dial,...dial.querySelectorAll('button')].every(e=>getComputedStyle(e).touchAction==='pan-x'),'touch horizontal scrolling enabled');
                                check(scroll.tabIndex===0 && scroll.getAttribute('aria-describedby')==='viewport-note','accessible scroll explanation');
                                scroll.scrollLeft=scroll.scrollWidth;
                                const right=dial.getBoundingClientRect();
                                check(scroll.scrollLeft>0 && right.right<=innerWidth,'right circle edge reachable');
                                scroll.scrollLeft=0;
                                check(dial.getBoundingClientRect().left>=0,'left edge reachable after scroll');
                              }else check(box.right<=innerWidth,'circle fits normal viewport');
                              check(document.documentElement.scrollWidth===document.documentElement.clientWidth,'no page-level horizontal overflow');
                              return 'VIEWPORT PASS';
                            })()'''.replace('WIDTH', str(width))
                            result = call('Runtime.evaluate', {'expression': probe, 'returnByValue': True}, session)
                            self.assertNotIn('exceptionDetails', result, result.get('exceptionDetails'))
                            self.assertEqual(result['result'].get('value'), 'VIEWPORT PASS')
                    # Real mouse hit-testing: outside movement does not bubble through the dial.
                    def evaluate(expression):
                        result = call('Runtime.evaluate', {'expression': expression, 'returnByValue': True}, session)
                        self.assertNotIn('exceptionDetails', result, result.get('exceptionDetails'))
                        return result['result'].get('value')
                    points = evaluate('''(() => {
                      document.getElementById('ask').click();
                      const b=document.getElementById('another').getBoundingClientRect();
                      const d=document.getElementById('dial').getBoundingClientRect();
                      return {x:b.x+b.width/2,y:b.y+b.height/2,outsideX:d.right+20};
                    })()''')
                    before = evaluate('window.preview.snapshot().draws')
                    def mouse(kind, x, y, down=False):
                        call('Input.dispatchMouseEvent', dict(type=kind, x=x, y=y,
                             button='left' if kind!='mouseMoved' else 'none',
                             buttons=1 if down else 0, clickCount=1 if kind!='mouseMoved' else 0), session)
                    mouse('mouseMoved', points['x'], points['y'])
                    mouse('mousePressed', points['x'], points['y'], down=True)
                    mouse('mouseMoved', points['outsideX'], points['y'], down=True)
                    mouse('mouseMoved', points['x'], points['y'], down=True)
                    mouse('mouseReleased', points['x'], points['y'])
                    self.assertEqual(evaluate('window.preview.snapshot().draws'), before,
                                     'mouse drag outside dial and back must not draw')
                    mouse('mousePressed', points['x'], points['y'], down=True)
                    mouse('mouseReleased', points['x'], points['y'])
                    self.assertEqual(evaluate('window.preview.snapshot().draws'), before+1,
                                     'normal mouse tap must still draw')
                    evaluate("document.getElementById('another').focus()")
                    for kind in ('keyDown', 'keyUp'):
                        call('Input.dispatchKeyEvent', dict(type=kind, key='Enter', code='Enter',
                             text='\r' if kind=='keyDown' else '', windowsVirtualKeyCode=13), session)
                    self.assertEqual(evaluate('window.preview.snapshot().draws'), before+2,
                                     'keyboard activation must still draw')
                finally:
                    chrome.terminate()
                    chrome.wait(timeout=10)

    @unittest.skipUnless(Path('/usr/bin/chromium').exists(), 'system Chromium unavailable')
    def test_all_real_copy_fits_and_pointer_safeguards(self):
        data = json.loads((ROOT/'content/cards.json').read_text())
        # Canonical paired content may be authored independently of this artifact.
        if not all(c.get('facilitator') for c in data['cards'] if c['card_id'] in IDS):
            self.skipTest('canonical representative pairs not authored yet')
        probe = r'''<script>
        try {
         const check=(v,m)=>{if(!v)throw Error(m)};
         const click=id=>document.getElementById(id).click();
         const fit=()=>{for(const b of document.querySelectorAll('#dial button'))check(b.scrollHeight<=b.clientHeight && b.scrollWidth<=b.clientWidth,'clipped button: '+b.textContent+' '+b.scrollHeight+'/'+b.clientHeight)};
         fit();click('ask');
         for(let i=0;i<8;i++){
          const id=window.preview.snapshot().card;
          for(let mode=0;mode<2;mode++){
           fit();click('more');fit();click('related');fit();click('guide');
           let pages=window.preview.guidePages();
           for(let p=0;p<pages.length;p++){fit();const body=document.getElementById('body-copy');check(body.scrollHeight<=body.clientHeight,'guide clipped '+id);if(p+1<pages.length)click('next')}
           click('back');
          }
          click('another');
         }
         const b=document.getElementById('another'),r=b.getBoundingClientRect();
         const point={x:r.x+r.width/2,y:r.y+r.height/2};
         const pointer=(type,x,y)=>b.dispatchEvent(new PointerEvent(type,{bubbles:true,pointerId:1,clientX:x,clientY:y}));
         const activate=(x,y,detail=1)=>b.dispatchEvent(new MouseEvent('click',{bubbles:true,clientX:x,clientY:y,detail}));
         let before=window.preview.snapshot().draws;
         pointer('pointerdown',point.x,point.y);pointer('pointermove',point.x+15,point.y);pointer('pointerup',point.x,point.y);activate(point.x,point.y);
         check(window.preview.snapshot().draws===before,'drag becomes draw');
         const d=document.getElementById('dial').getBoundingClientRect();
         pointer('pointerdown',d.x+1,d.y+1);pointer('pointerup',d.x+1,d.y+1);activate(d.x+1,d.y+1);
         check(window.preview.snapshot().draws===before,'corner forged target draw');
         pointer('pointerdown',point.x,point.y);pointer('pointerup',point.x,point.y);activate(point.x,point.y,2);
         check(window.preview.snapshot().draws===before,'duplicate click draw');
         pointer('pointerdown',point.x,point.y);pointer('pointerup',point.x,point.y);activate(point.x,point.y);
         check(window.preview.snapshot().draws===before+1,'valid pointer release draws');
         b.click();check(window.preview.snapshot().draws===before+1,'stale detached target queues duplicate draw');
         document.body.insertAdjacentHTML('beforeend','<pre id="real-result">REAL COPY PASS</pre>');
        }catch(e){document.body.insertAdjacentHTML('beforeend','<pre id="real-result">FAIL '+e.message+'</pre>')}
        </script>'''
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as temp:
            page=Path(temp)/'real.html'
            page.write_text(builder().render(data,(ROOT/'prototype/template.html').read_text()).replace('</body>',probe+'</body>'))
            run=subprocess.run(['/usr/bin/chromium','--headless','--no-sandbox','--disable-gpu','--disable-dev-shm-usage','--dump-dom',page.as_uri()],capture_output=True,text=True,timeout=60)
            # Match an actual output element at the end, not a script string literal.
            match=re.search(r'<pre id="real-result">([^<]*)</pre>\s*</body>',run.stdout)
            self.assertIsNotNone(match, run.stdout[-1500:])
            self.assertEqual(match.group(1),'REAL COPY PASS')


if __name__ == '__main__':
    unittest.main()
