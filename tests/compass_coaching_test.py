"""Focused compass-coaching contracts; stdlib + actual system Chromium only.
Run: TMPDIR=$HOME/.hermes/cache/scratch python3 tests/compass_coaching_test.py -v
Never invokes firmware builders or existing prototype generators.
"""
import copy
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
EVIDENCE = ROOT / 'build/compass-coaching-preview/icons-evidence'
SCRATCH = Path.home() / '.hermes/cache/scratch'
os.environ['TMPDIR'] = str(SCRATCH)
SCRATCH.mkdir(parents=True, exist_ok=True)
tempfile.tempdir = str(SCRATCH)


def builder():
    spec = importlib.util.spec_from_file_location('compass_builder', ROOT/'tools/build_compass_coaching.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def layer():
    return json.loads((ROOT/'content/coaching.json').read_text())


def cards():
    return json.loads((ROOT/'content/cards.json').read_text())


class ContentTest(unittest.TestCase):
    def test_draft_layer_resolves_exact_canonical_references(self):
        self.assertTrue((ROOT/'tools/build_compass_coaching.py').exists(), 'coaching generator missing')
        data = layer()
        resolved = builder().validate(data, cards())
        self.assertEqual([e['id'] for e in resolved['entries'] if e['type']=='opener'], ['OPEN-%02d'%i for i in range(1,7)])
        self.assertEqual(len(resolved['paths']), 6)
        for path in resolved['paths']:
            for kind in ('question', 'experiment'):
                self.assertGreaterEqual(sum(e.get('path')==path['id'] and e['type']==kind for e in resolved['entries']), 2)
        canonical = {c['card_id']:c for c in cards()['cards']}
        references = []
        for source, result in zip(data['entries'], resolved['entries']):
            self.assertEqual(source['review_state'], 'draft')
            if 'text_ref' in source:
                ref=source['text_ref']; value=canonical[ref['card_id']]
                for key in ref['field'].split('.'):
                    value=value[key]
                self.assertEqual(result['text'], value)
                self.assertNotIn('text', source)
                references.append(ref['card_id'])
        self.assertTrue({'tg-028','tg-001','tg-026','tg-002','tg-010','tg-031','tg-032','tg-004','tg-003','tg-014'} <= set(references))


    def test_coaching_scenarios_keep_choice_conditions_and_human_only_route(self):
        entries=builder().validate(layer(),cards())['entries']; by={e['id']:e for e in entries}
        self.assertEqual(by['OPEN-01']['text'],'How’s the project going?')
        self.assertEqual(by['PROGRESS-Q01']['text'],'What’s working well?')
        self.assertIn('Do not manufacture a problem',by['PROGRESS-Q01']['guidance'])
        self.assertEqual(by['EXPLORING-Q01']['text'],'Which possibility would you like to explore?')
        self.assertEqual(by['AI-RESULTS-Q01']['text'],'How well does the output fit what you need?')
        self.assertIn('expected versus actual',by['AI-RESULTS-Q01']['guidance'])
        self.assertIn('Only if they report disappointing output',by['AI-RESULTS-Q01']['guidance'])
        self.assertNotIn('What part felt least useful?', [e['text'] for e in entries if e['type']=='question'])
        self.assertIn('Do not pick a solution',by['UNCLEAR-Q01']['guidance'])
        self.assertIn('Do not invent agreement or diagnose silence',by['TEAM-Q01']['guidance'])
        self.assertIn('passing, writing, drawing',by['TEAM-Q01']['guidance'])
        self.assertIn('human-only step; that is a complete choice',by['EXPLORING-E01']['guidance'])
        self.assertIn('No winner or AI trial is required',by['EXPLORING-E01']['guidance'])
        optional_cues = {'STUCK-E01': 'if wanted', 'EXPLORING-E01': 'Only if useful',
                         'AI-RESULTS-E02': 'If wanted'}
        for e in entries:
            if e['type']=='experiment':
                self.assertTrue(e['text'].startswith(('Would it help','Would a ','Would one ')),e['id'])
                self.assertIn(optional_cues.get(e['id'], 'optional'), e['guidance'])
                self.assertIn('self-critique is not proof',e['guidance'])
        self.assertEqual(sum(e['type']=='question' for e in entries),24)
        self.assertEqual(sum(e['type']=='experiment' for e in entries),18)


class BuildTest(unittest.TestCase):
    def test_rejects_invalid_layer_and_references_cleanly(self):
        for key,value in [('id','bad id'),('type','advice'),('review_state','approved'),('path','unknown'),('text','')]:
            d=layer(); e=next(e for e in d['entries'] if e['type']=='question' and 'text' in e); e[key]=value
            with self.subTest(key=key), self.assertRaises(ValueError): builder().validate(d,cards())
        for ref in [{'card_id':'tg-999','field':'facilitator.ask'}, {'card_id':'tg-028','field':'question'}, {'card_id':'tg-032','field':'facilitator.ask'}, None]:
            d=layer(); d['entries'][0].pop('text'); d['entries'][0]['text_ref']=ref
            with self.subTest(ref=ref), self.assertRaises(ValueError): builder().validate(d,cards())
        for d in [None,[],{},dict(layer(),schema_version=True),dict(layer(),review_state='reviewed')]:
            with self.assertRaises(ValueError): builder().validate(d,cards())
        d=layer(); d['entries'].append(copy.deepcopy(d['entries'][0]))
        with self.assertRaises(ValueError): builder().validate(d,cards())
        d=layer(); d['paths'][0]['clock']=10
        with self.assertRaises(ValueError): builder().validate(d,cards())

    def test_deterministic_safe_render_and_no_write_check(self):
        self.assertTrue((ROOT/'prototype/compass_coaching_template.html').exists(), 'standalone coaching template missing')
        template=(ROOT/'prototype/compass_coaching_template.html').read_text()
        d=layer(); d['entries'][0]['text']='</script><script>window.INJECTED=1</script> & < > FINAL QUALIFIER'
        output=builder().render(d,cards(),template)
        self.assertEqual(output,builder().render(copy.deepcopy(d),cards(),template))
        self.assertNotIn('<script>window.INJECTED',output)
        for forbidden in ('fetch(', 'XMLHttpRequest', 'WebSocket', 'localStorage', 'sessionStorage', 'indexedDB', 'setTimeout', 'setInterval', 'DeviceMotionEvent', 'DeviceOrientationEvent', 'https://', 'http://', '@import', 'src='):
            self.assertNotIn(forbidden,output)
        command=['python3',str(ROOT/'tools/build_compass_coaching.py')]
        with tempfile.TemporaryDirectory(prefix='cc-') as tmp:
            p=Path(tmp); source=p/'source.json'; out=p/'output.html'; source.write_text(json.dumps(d))
            args=command+['--content',str(source),'--output',str(out)]
            self.assertEqual(subprocess.run(args,capture_output=True).returncode,0)
            before=out.stat().st_mtime_ns
            self.assertEqual(subprocess.run(args+['--check'],capture_output=True).returncode,0)
            self.assertEqual(out.stat().st_mtime_ns,before)
            out.write_text('stale'); before=out.stat().st_mtime_ns
            self.assertNotEqual(subprocess.run(args+['--check'],capture_output=True).returncode,0)
            self.assertEqual(out.read_text(),'stale'); self.assertEqual(out.stat().st_mtime_ns,before)
            for value in ('null','{','{}'):
                source.write_text(value)
                result=subprocess.run(args,capture_output=True,text=True)
                self.assertNotEqual(result.returncode,0); self.assertNotIn('Traceback',result.stderr)
                self.assertEqual(out.read_text(),'stale')
            result=subprocess.run(command+['--content',str(p/'missing'),'--output',str(out)],capture_output=True,text=True)
            self.assertNotEqual(result.returncode,0)


class Chromium:
    """CDP pipe with an owned short disposable profile; no shared browser state."""
    def __enter__(self):
        import time
        self.time=time
        self.profile=tempfile.TemporaryDirectory(prefix='c-')
        self.chrome=subprocess.Popen(['/bin/sh','-c','exec "$@" 3<&0 4>&1','chromium','/usr/bin/chromium',
            '--headless','--no-sandbox','--disable-gpu','--disable-dev-shm-usage','--no-proxy-server',
            '--disable-background-networking','--disable-component-update','--no-first-run',
            '--user-data-dir='+self.profile.name,'--remote-debugging-pipe'],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
        self.sequence,self.pending,self.events=0,b'',[]
        try:
            target=self.call('Target.createTarget',{'url':'about:blank'})['targetId']
            self.session=self.call('Target.attachToTarget',{'targetId':target,'flatten':True})['sessionId']
            self.call('Runtime.enable',session=self.session); self.call('Log.enable',session=self.session)
            self.viewport(900)
            return self
        except BaseException:
            self.__exit__(); raise

    def call(self,method,params=None,session=None):
        import select
        self.sequence+=1
        message=dict(id=self.sequence,method=method,params=params or {})
        if session:message['sessionId']=session
        self.chrome.stdin.write(json.dumps(message).encode()+b'\0'); self.chrome.stdin.flush()
        deadline=self.time.monotonic()+25
        while self.time.monotonic()<deadline:
            if b'\0' not in self.pending:
                if not select.select([self.chrome.stdout],[],[],.1)[0]:continue
                chunk=os.read(self.chrome.stdout.fileno(),65536)
                if not chunk:raise AssertionError('Chromium CDP closed')
                self.pending+=chunk; continue
            raw,self.pending=self.pending.split(b'\0',1); r=json.loads(raw)
            if 'method' in r:self.events.append(r)
            if r.get('id')==self.sequence:
                if 'error' in r:raise AssertionError(r['error'])
                return r.get('result',{})
        raise AssertionError('CDP timeout '+method)

    def evaluate(self,expression):
        r=self.call('Runtime.evaluate',{'expression':expression,'returnByValue':True},self.session)
        if 'exceptionDetails' in r:raise AssertionError(r['exceptionDetails'])
        return r['result'].get('value')

    def viewport(self,width):
        self.call('Emulation.setDeviceMetricsOverride',dict(width=width,height=900,deviceScaleFactor=1,mobile=False),self.session)

    def load(self,path):
        self.call('Page.navigate',{'url':path.as_uri()},self.session)
        for _ in range(200):
            if self.evaluate("document.readyState==='complete' && !!window.compassCoaching"):return
            self.time.sleep(.01)
        raise AssertionError('compass preview did not initialize')

    def state(self):return self.evaluate('window.compassCoaching.snapshot()')
    def click(self,id):return self.evaluate('document.getElementById('+json.dumps(id)+').click()')
    def mouse(self,kind,x,y,held=False,count=1):
        self.call('Input.dispatchMouseEvent',dict(type=kind,x=x,y=y,button='none' if kind=='mouseMoved' else 'left',buttons=1 if held else 0,clickCount=0 if kind=='mouseMoved' else count),self.session)
    def tap(self,id):
        p=self.evaluate('(()=>{const r=document.getElementById('+json.dumps(id)+').getBoundingClientRect();return {x:r.x+r.width/2,y:r.y+r.height/2}})()')
        self.mouse('mousePressed',**p,held=True); self.mouse('mouseReleased',**p)
    def screenshot(self,name):
        import base64
        EVIDENCE.mkdir(parents=True,exist_ok=True)
        data=self.call('Page.captureScreenshot',{'format':'png'},self.session)['data']
        (EVIDENCE/(name+'.png')).write_bytes(base64.b64decode(data))
    def __exit__(self,*args):
        if getattr(self,'chrome',None):
            try:
                self.call('Browser.close')
                self.chrome.wait(timeout=10)
            except (AssertionError, BrokenPipeError, subprocess.TimeoutExpired):
                self.chrome.terminate()
                try:self.chrome.wait(timeout=10)
                except subprocess.TimeoutExpired:self.chrome.kill(); self.chrome.wait(timeout=5)
            self.chrome.stdin.close(); self.chrome.stdout.close()
        if getattr(self,'profile',None):
            for attempt in range(20):
                try:self.profile.cleanup(); break
                except OSError:
                    if attempt==19:raise
                    self.time.sleep(.1)


class BrowserTest(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory(prefix='cc-'); self.addCleanup(self.tmp.cleanup)
        self.page=Path(self.tmp.name)/'preview.html'
        self.page.write_text(builder().render(layer(),cards(),(ROOT/'prototype/compass_coaching_template.html').read_text()))
        self.c=Chromium().__enter__(); self.addCleanup(self.c.__exit__)
        self.c.load(self.page)
    def tearDown(self):
        if hasattr(self,'c'):
            self.c.evaluate('true')
            errors=[e for e in self.c.events if e['method']=='Runtime.exceptionThrown' or (e['method']=='Log.entryAdded' and e['params']['entry']['level']=='error')]
            self.assertEqual(errors,[],'browser console errors')

    def test_compact_path_icons_keep_full_names_safe_hit_masks_and_central_copy_room(self):
        c=self.c
        short=['Progress','Stuck','Unclear','Explore','AI','Team']
        paths=layer()['paths']
        def check_ring(selected=None):
            controls=c.evaluate('''[...document.querySelectorAll('#dial button[id^="path-"]')].map(b=>{
              const svg=b.querySelector('svg'),r=b.getBoundingClientRect(),style=getComputedStyle(b);
              return {id:b.id,label:b.textContent,aria:b.getAttribute('aria-label'),pressed:b.getAttribute('aria-pressed'),
                width:r.width,height:r.height,font:style.fontSize,background:style.backgroundColor,border:style.borderTopColor,
                icon:svg?{width:svg.getBoundingClientRect().width,height:svg.getBoundingClientRect().height,
                  hidden:svg.getAttribute('aria-hidden'),stroke:svg.getAttribute('stroke'),fill:svg.getAttribute('fill'),
                  visible:getComputedStyle(svg).visibility,opacity:getComputedStyle(svg).opacity,paths:svg.querySelectorAll('path').length}:null};
            })''')
            self.assertEqual(len(controls),6)
            for item,p,label in zip(controls,paths,short):
                self.assertIsNotNone(item['icon'],'each path needs a visible compact line icon')
                self.assertEqual(item['label'],label)
                self.assertEqual(item['aria'],p['label'])
                self.assertEqual(item['pressed'],str(p['id']==selected).lower())
                self.assertEqual(item['font'],'14px')
                self.assertGreaterEqual(item['width'],48);self.assertGreaterEqual(item['height'],48)
                self.assertEqual(item['background'],'rgba(0, 0, 0, 0)')
                self.assertEqual(item['border'],'rgba(0, 0, 0, 0)')
                self.assertEqual(item['icon']['width'],22);self.assertEqual(item['icon']['height'],22)
                self.assertEqual(item['icon']['hidden'],'true');self.assertEqual(item['icon']['stroke'],'currentColor')
                self.assertEqual(item['icon']['fill'],'none');self.assertEqual(item['icon']['visible'],'visible')
                self.assertEqual(item['icon']['opacity'],'1');self.assertGreater(item['icon']['paths'],0)
            errors=c.evaluate('''(()=>{
              const d=document.getElementById('dial'),r=d.getBoundingClientRect();
              const boxes=[...d.querySelectorAll('button,[data-envelope]')].map(e=>{
                const b=e.getBoundingClientRect(),f=e.matches('button,#body-copy')?4:0;
                return {id:e.id,left:b.left-f-r.left,right:b.right+f-r.left,top:b.top-f-r.top,bottom:b.bottom+f-r.top};
              }),errors=[];
              for(const b of boxes)for(const x of [b.left,b.right])for(const y of [b.top,b.bottom])
                if(Math.hypot(x-180,y-180)>168.01)errors.push('unsafe '+b.id);
              for(let i=0;i<boxes.length;i++)for(let j=i+1;j<boxes.length;j++){
                const a=boxes[i],b=boxes[j];
                if(a.left<b.right&&a.right>b.left&&a.top<b.bottom&&a.bottom>b.top)errors.push('overlap '+a.id+'/'+b.id);
              }
              return errors;
            })()''')
            self.assertEqual(errors,[])
        check_ring()
        room=c.evaluate("(()=>{const e=document.getElementById('body-copy'),r=e.getBoundingClientRect();return {width:r.width,height:r.height,font:getComputedStyle(e).fontSize}})()")
        self.assertGreaterEqual(room['width'],136);self.assertGreaterEqual(room['height'],120);self.assertEqual(room['font'],'20px')
        c.screenshot('compact-icons-opener')
        for width in (320,360,900):
            c.viewport(width);check_ring()
            self.assertEqual(c.evaluate("document.getElementById('dial').getBoundingClientRect().width"),360)
        # Real pointer input must qualify when its target is an SVG path child.
        def point(selector,edge=False):
            return c.evaluate('''(()=>{const e=document.querySelector(%s),r=e.getBoundingClientRect();
              const p={x:r.x+%s,y:r.y+%s};return {...p,target:document.elementFromPoint(p.x,p.y).tagName};})()''' %
              (json.dumps(selector),'3' if edge else 'r.width/2','r.height/2'))
        svg_point=c.evaluate('''(()=>{const path=document.querySelector('#path-progress svg path');
          const p=path.getPointAtLength(path.getTotalLength()*.2),q=new DOMPoint(p.x,p.y).matrixTransform(path.getScreenCTM());
          return {x:q.x,y:q.y,target:document.elementFromPoint(q.x,q.y).tagName};})()''')
        self.assertEqual(svg_point.pop('target').lower(),'path')
        before=c.state();c.mouse('mousePressed',**svg_point,held=True);c.mouse('mouseReleased',**svg_point)
        self.assertEqual((c.state()['view'],c.state()['path']),('question','progress'))
        self.assertEqual(c.state()['draws'],before['draws']+1)
        c.click('paths');check_ring('progress');c.screenshot('compact-icons-picker')
        # A genuinely blank part of the same large hit mask is still tappable.
        padding=point('#path-stuck',True);self.assertEqual(padding.pop('target'),'BUTTON')
        c.mouse('mousePressed',**padding,held=True);c.mouse('mouseReleased',**padding)
        self.assertEqual(c.state()['path'],'stuck')
        c.click('paths');check_ring('stuck')
        # Empty space just beyond the mask must not choose any nearby path.
        gap=c.evaluate("(()=>{const r=document.getElementById('path-stuck').getBoundingClientRect();return {x:r.left-5,y:r.top+r.height/2}})()")
        before=c.state();c.mouse('mousePressed',**gap,held=True);c.mouse('mouseReleased',**gap);self.assertEqual(c.state(),before)
        c.mouse('mousePressed',**padding,held=True,count=2);c.mouse('mouseReleased',**padding,count=2)
        self.assertEqual(c.state(),before,'duplicate path input must not dismiss the picker')
        drag=point('#path-unclear svg');drag.pop('target')
        c.mouse('mousePressed',**drag,held=True);c.mouse('mouseMoved',drag['x']-20,drag['y'],True)
        c.mouse('mouseMoved',**drag,held=True);c.mouse('mouseReleased',**drag)
        self.assertEqual(c.state(),before,'drag returning to an icon must not select it')
        c.evaluate("document.getElementById('path-ai-results').focus()")
        for kind in ('keyDown','keyUp'):
            c.call('Input.dispatchKeyEvent',dict(type=kind,key='Enter',code='Enter',text='\r' if kind=='keyDown' else '',windowsVirtualKeyCode=13),c.session)
        self.assertEqual(c.state()['path'],'ai-results')
        c.click('paths');check_ring('ai-results')
        # The icon child also remains reachable through actual touch compatibility input.
        touch=point('#path-team svg');touch.pop('target')
        c.call('Emulation.setTouchEmulationEnabled',{'enabled':True},c.session)
        c.call('Input.dispatchTouchEvent',{'type':'touchStart','touchPoints':[touch]},c.session)
        c.call('Input.dispatchTouchEvent',{'type':'touchEnd','touchPoints':[]},c.session)
        self.assertEqual(c.state()['path'],'team')
        c.call('Emulation.setTouchEmulationEnabled',{'enabled':False},c.session)
        c.click('paths');c.click('opening')
        # Visit the longest real opening deterministically; no content changes.
        openers=[e for e in builder().validate(layer(),cards())['entries'] if e['type']=='opener']
        longest=max(openers,key=lambda e:len(e['text']))
        if c.state()['entry_id']!=longest['id']:
            choices=[e['id'] for e in openers if e['id']!=c.state()['entry_id']]
            c.evaluate('Math.random=()=>'+str((choices.index(longest['id'])+.1)/len(choices)));c.click('simulate-turnover')
        self.assertEqual(c.state()['text'],longest['text']);check_ring()
        c.screenshot('compact-icons-longest-opener')
        c.evaluate("(()=>{const e=document.getElementById('body-copy');e.scrollTop=e.scrollHeight})()")
        self.assertTrue(c.evaluate("(()=>{const e=document.getElementById('body-copy');return e.scrollHeight-e.scrollTop-e.clientHeight<=1})()"))
        c.screenshot('compact-icons-longest-opener-end')
        (EVIDENCE/'longest-opener.json').write_text(json.dumps(longest,indent=2)+'\n')

    def test_return_to_opener_preserves_exact_content_scroll_and_path_context(self):
        c=self.c
        data=layer()
        for e in data['entries']:
            if e['type']=='opener':
                e['text']='SYNTHETIC opener overflow. '+('Keep the original question and return context. '*30)
        self.page.write_text(builder().render(data,cards(),(ROOT/'prototype/compass_coaching_template.html').read_text()))
        c.load(self.page)
        c.evaluate("document.getElementById('body-copy').scrollTop=250")
        original=c.state(); original['scroll']=c.evaluate("document.getElementById('body-copy').scrollTop")
        c.tap('path-progress'); c.click('experiment'); path=c.state()
        c.click('paths')
        self.assertTrue(c.evaluate("!!document.getElementById('opening')"),'path picker needs a return to the preserved opening question')
        c.tap('opening'); returned=c.state()
        for key in ('view','path','entry_id','text','scroll'):
            self.assertEqual(returned[key],original[key],key)
        self.assertEqual(returned['draws'],path['draws'],'returning must not consume another opener')
        c.tap('path-progress')
        for key in ('view','path','entry_id','text','scroll','draws'):
            self.assertEqual(c.state()[key],path[key],key)

    def test_release_from_path_picker_returns_to_exact_picker_context(self):
        c=self.c;c.click('path-team');c.click('paths');prior=c.state()
        self.assertTrue(c.evaluate("!!document.getElementById('release')"),'path picker must allow release without choosing another path')
        c.tap('release');self.assertEqual(c.state()['view'],'closing')
        c.click('back');after=c.state()
        for key in ('view','path','entry_id','text','scroll'):
            self.assertEqual(after[key],prior[key],key)
        c.click('back');self.assertEqual(c.state()['path'],'team')

    def test_all_paths_questions_experiments_and_context_returns(self):
        c=self.c
        resolved=builder().validate(layer(),cards())['entries']
        for p in layer()['paths']:
            path=p['id']
            with self.subTest(path=path):
                if c.state()['view']!='opener':c.click('paths')
                c.tap('path-'+path)
                self.assertEqual((c.state()['view'],c.state()['path']),('question',path))
                c.screenshot(path+'-question')
                expected={e['id']:e for e in resolved if e.get('path')==path and e['type']=='question'}
                # Fix the RNG sequence to visit each remaining choice; no probabilistic coverage.
                seen=set()
                for i in range(24):
                    state=c.state(); self.assertEqual(state['text'],expected[state['entry_id']]['text']); seen.add(state['entry_id'])
                    if set(expected)<=seen:break
                    choices=[key for key in expected if key!=state['entry_id']]
                    target=next(key for key in choices if key not in seen)
                    c.evaluate('Math.random=()=>'+str((choices.index(target)+.1)/len(choices))); c.click('another-question')
                    self.assertNotEqual(c.state()['entry_id'],state['entry_id'])
                self.assertEqual(seen,set(expected))
                before=c.state(); c.click('simulate-turnover'); self.assertEqual(c.state(),before)
                for route in ('guide','paths','release'):
                    before=c.state(); c.click(route)
                    if route=='paths':
                        self.assertEqual(c.evaluate('document.getElementById('+json.dumps('path-'+path)+').getAttribute("aria-pressed")'),'true')
                    c.click('simulate-turnover'); c.click('back')
                    self.assertEqual(c.state()['entry_id'],before['entry_id']); self.assertEqual(c.state()['path'],path)
                    self.assertEqual(c.state()['draws'],before['draws']+(1 if route=='release' else 0))
                c.click('experiment'); self.assertEqual(c.state()['view'],'experiment'); c.screenshot(path+'-experiment')
                expected={e['id']:e for e in resolved if e.get('path')==path and e['type']=='experiment'}
                seen=set()
                for i in range(18):
                    st=c.state(); seen.add(st['entry_id']); self.assertEqual(st['text'],expected[st['entry_id']]['text'])
                    if set(expected)<=seen:break
                    choices=[key for key in expected if key!=st['entry_id']]
                    target=next(key for key in choices if key not in seen)
                    c.evaluate('Math.random=()=>'+str((choices.index(target)+.1)/len(choices))); c.click('another-experiment')
                    self.assertNotEqual(c.state()['entry_id'],st['entry_id'])
                self.assertEqual(seen,set(expected))
                saved=c.state(); c.click('guide'); c.screenshot(path+'-help'); guide=c.state()
                c.click('reminders'); self.assertEqual(c.state()['view'],'reminders'); c.click('back'); self.assertEqual(c.state(),guide)
                c.click('release'); c.screenshot(path+'-release'); c.click('back'); self.assertEqual(c.state()['entry_id'],guide['entry_id']); self.assertEqual(c.state()['view'],'guide')
                c.click('back'); self.assertEqual(c.state()['entry_id'],saved['entry_id']); self.assertEqual(c.state()['view'],'experiment')
                c.click('paths'); c.click('path-'+path); self.assertEqual(c.state()['entry_id'],saved['entry_id'])
                # Switching away and back restores the exact per-path experiment.
                other='stuck' if path!='stuck' else 'progress'
                c.click('paths'); c.click('path-'+other); c.click('paths'); c.click('path-'+path)
                self.assertEqual((c.state()['view'],c.state()['entry_id']),('experiment',saved['entry_id']))
                c.click('question'); self.assertEqual(c.state()['view'],'question')

    def test_geometry_focus_targets_pointer_keyboard_and_narrow_pan(self):
        c=self.c
        def geometry():
            result=c.evaluate('''(()=>{
              const d=document.getElementById('dial'),r=d.getBoundingClientRect(),els=[...d.querySelectorAll('button,[data-envelope]')];
              const errors=[];
              for(const e of els){const b=e.getBoundingClientRect(),f=e.matches('button,#body-copy')?4:0;
                if([b.left-f,b.right+f].some(x=>[b.top-f,b.bottom+f].some(y=>Math.hypot(x-r.left-180,y-r.top-180)>168.01)))errors.push('unsafe '+e.id);
                if(e.tagName==='BUTTON'&&(b.width<48||b.height<48||e.scrollWidth>e.clientWidth||e.scrollHeight>e.clientHeight))errors.push('target/label '+e.id);
              }
              const copy=document.getElementById('body-copy');
              if(copy){const a=copy.getBoundingClientRect();
                for(const e of d.querySelectorAll('button')){const b=e.getBoundingClientRect();if(a.left-4<b.right+4&&a.right+4>b.left-4&&a.top-4<b.bottom+4&&a.bottom+4>b.top-4)errors.push('copy/target overlap '+e.id)}
                if(getComputedStyle(copy).fontSize!=='20px')errors.push('font');
              }
              return {errors,width:r.width,height:r.height,transform:getComputedStyle(d).transform};
            })()''')
            self.assertEqual(result,dict(errors=[],width=360,height=360,transform='none'))
        geometry(); c.click('options'); geometry(); c.click('guide'); geometry(); c.click('reminders'); geometry(); c.click('release'); geometry(); c.click('back'); c.click('back'); c.click('back'); c.click('back')
        for path in layer()['paths']:
            c.click('path-'+path['id']); geometry(); c.click('experiment'); geometry(); c.click('paths'); geometry()
        c.click('back'); c.click('question')
        for width in (320,360,900):
            c.viewport(width); geometry()
            self.assertTrue(c.evaluate('document.documentElement.scrollWidth===document.documentElement.clientWidth'))
            if width==320:
                self.assertTrue(c.evaluate('''(()=>{const e=document.getElementById('device-scroll');e.scrollLeft=0;const left=document.getElementById('dial').getBoundingClientRect().left;e.scrollLeft=e.scrollWidth;return left>=0&&e.scrollLeft>0&&document.getElementById('dial').getBoundingClientRect().right<=innerWidth&&e.tabIndex===0&&getComputedStyle(document.getElementById('viewport-note')).display!=='none'})()'''))
                c.evaluate("document.getElementById('device-scroll').scrollLeft=0")
        p=c.evaluate("(()=>{const b=document.getElementById('another-question').getBoundingClientRect(),d=document.getElementById('dial').getBoundingClientRect();return {x:b.x+b.width/2,y:b.y+b.height/2,out:d.right+20,cx:d.left+2,cy:d.top+2}})()")
        before=c.state()
        c.mouse('mousePressed',p['x'],p['y'],True); c.mouse('mouseMoved',p['out'],p['y'],True); c.mouse('mouseMoved',p['x'],p['y'],True); c.mouse('mouseReleased',p['x'],p['y'])
        self.assertEqual(c.state(),before)
        c.mouse('mousePressed',p['cx'],p['cy'],True); c.mouse('mouseReleased',p['cx'],p['cy']); self.assertEqual(c.state(),before)
        c.tap('another-question'); self.assertEqual(c.state()['draws'],before['draws']+1)
        c.mouse('mousePressed',p['x'],p['y'],True,count=2); c.mouse('mouseReleased',p['x'],p['y'],count=2)
        self.assertEqual(c.state()['draws'],before['draws']+1)
        c.evaluate("window.stale=document.getElementById('another-question');window.stale.click();window.stale.click()")
        self.assertEqual(c.state()['draws'],before['draws']+2)
        c.evaluate("document.getElementById('another-question').focus()")
        for kind in ('keyDown','keyUp'):
            c.call('Input.dispatchKeyEvent',dict(type=kind,key='Enter',code='Enter',text='\r' if kind=='keyDown' else '',windowsVirtualKeyCode=13),c.session)
        self.assertEqual(c.state()['draws'],before['draws']+3)
        c.call('Emulation.setTouchEmulationEnabled',{'enabled':True},c.session)
        c.call('Input.dispatchTouchEvent',{'type':'touchStart','touchPoints':[{'x':p['x'],'y':p['y']}]},c.session)
        c.call('Input.dispatchTouchEvent',{'type':'touchEnd','touchPoints':[]},c.session)
        self.assertEqual(c.state()['draws'],before['draws']+4)

    def test_long_literal_copy_scroll_context_and_single_empty_pools(self):
        c=self.c
        long='SYNTHETIC OVERFLOW — '+('Keep complete wording and readable type. '*20)+'</script><script>window.INJECTED=1</script> FINAL QUALIFIER'
        d=layer()
        for e in d['entries']:
            if e['type'] in ('opener','question','experiment'):
                e.pop('text_ref',None); e['text']=long; e['guidance']='PRIVATE HELP '+long
        self.page.write_text(builder().render(d,cards(),(ROOT/'prototype/compass_coaching_template.html').read_text())); c.load(self.page)
        def end():
            c.evaluate("(()=>{const e=document.getElementById('body-copy');e.focus();e.scrollTop=0})()")
            for kind in ('keyDown','keyUp'):
                c.call('Input.dispatchKeyEvent',dict(type=kind,key='End',code='End',windowsVirtualKeyCode=35),c.session)
            c.time.sleep(.25)
            self.assertTrue(c.evaluate("(()=>{const e=document.getElementById('body-copy');return e.scrollTop>0&&e.scrollHeight-e.scrollTop-e.clientHeight<=1})()"))
            self.assertIsNone(c.evaluate('window.INJECTED'))
            self.assertIn('FINAL QUALIFIER',c.state()['text'])
        for view in ('opener','question','experiment'):
            if view=='question':c.click('path-progress')
            if view=='experiment':c.click('experiment')
            end(); before=c.state()
            if view=='opener':c.click('options')
            c.click('guide'); end(); helpstate=c.state(); c.click('release'); c.click('back')
            self.assertEqual(c.state()['scroll'],helpstate['scroll']); c.click('back')
            if view=='opener':c.click('back')
            after=c.state()
            for key in ('view','path','entry_id','text','scroll'):self.assertEqual(after[key],before[key],key)
            if view!='opener':
                c.click('paths'); c.click('back'); self.assertEqual(c.state()['scroll'],before['scroll'])
                c.click('paths'); c.click('path-stuck'); c.click('paths'); c.click('path-progress'); self.assertEqual(c.state()['scroll'],before['scroll'])
        c.evaluate("document.getElementById('full-copy').open=true")
        self.assertEqual(c.evaluate("document.getElementById('complete-copy').textContent"),long)
        d=layer(); d['entries']=[e for e in d['entries'] if e['type'] not in ('opener','closing','experiment') and (e['type']!='question' or e['id']=='PROGRESS-Q01')]
        self.page.write_text(builder().render(d,cards(),(ROOT/'prototype/compass_coaching_template.html').read_text())); c.load(self.page)
        self.assertEqual(c.state()['entry_id'],None); self.assertIn('No opening',c.state()['text'])
        c.click('path-progress'); self.assertTrue(c.evaluate("document.getElementById('another-question').disabled && document.getElementById('experiment').disabled"))
        self.assertIn('Only one question',c.evaluate("document.getElementById('pool-note').textContent"))
        c.click('paths'); c.click('path-stuck'); self.assertIn('No questions',c.state()['text'])
        c.click('guide'); c.click('release'); self.assertIn('No closing',c.state()['text']); c.click('back'); c.click('back')

    def test_delivered_generated_artifact_is_current_and_runs_without_repair(self):
        output=ROOT/'prototype/compass-coaching.html'
        self.assertTrue(output.exists(),'portable generated HTML missing')
        before=output.stat().st_mtime_ns
        self.assertEqual(output.read_text(),builder().render(layer(),cards(),(ROOT/'prototype/compass_coaching_template.html').read_text()))
        check=subprocess.run(['python3',str(ROOT/'tools/build_compass_coaching.py'),'--check'],capture_output=True,text=True)
        self.assertEqual(check.returncode,0,check.stderr);self.assertEqual(output.stat().st_mtime_ns,before)
        c=self.c;c.load(output);self.assertEqual(c.state()['view'],'opener');c.screenshot('delivered-opener')
        c.tap('path-progress');self.assertEqual(c.state()['path'],'progress');c.screenshot('delivered-progress')

    def test_single_pools_disable_unavailable_draws_without_misleading_repeat(self):
        c=self.c; d=layer(); selected=[]; seen=set()
        for e in d['entries']:
            key=(e['type'],e.get('path'))
            if key not in seen:selected.append(e);seen.add(key)
        d['entries']=selected
        self.page.write_text(builder().render(d,cards(),(ROOT/'prototype/compass_coaching_template.html').read_text()));c.load(self.page)
        self.assertTrue(c.evaluate("document.getElementById('simulate-turnover').disabled"))
        before=c.state(); c.click('simulate-turnover');self.assertEqual(c.state(),before)
        c.click('options');self.assertTrue(c.evaluate("document.getElementById('another-opener').disabled"));c.click('back')
        c.click('path-progress');self.assertTrue(c.evaluate("document.getElementById('another-question').disabled"))
        c.click('experiment');self.assertTrue(c.evaluate("document.getElementById('another-experiment').disabled"))
        before=c.state();c.click('another-experiment');self.assertEqual(c.state(),before)
        c.click('release');self.assertTrue(c.evaluate("document.getElementById('another-closing').disabled"))

    def test_controlled_random_pools_openers_closings_and_first_path_draw(self):
        c=self.c
        openers=[e['id'] for e in layer()['entries'] if e['type']=='opener']
        seen={c.state()['entry_id']}
        while set(openers)-seen:
            previous=c.state()['entry_id']; choices=[i for i in openers if i!=previous]
            target=next(i for i in choices if i not in seen)
            c.evaluate('Math.random=()=>'+str((choices.index(target)+.1)/len(choices))); c.click('simulate-turnover')
            self.assertEqual(c.state()['entry_id'],target); seen.add(target)
        c.evaluate('Math.random=()=>.999999'); c.click('path-progress')
        self.assertEqual(c.state()['entry_id'],'PROGRESS-Q04','new path selects from its random pool, not a forced script')
        c.click('release'); closings=[e['id'] for e in layer()['entries'] if e['type']=='closing']; seen=set()
        for _ in range(6):
            st=c.state(); seen.add(st['entry_id']); c.evaluate('Math.random=()=>0'); c.click('another-closing')
            self.assertNotEqual(c.state()['entry_id'],st['entry_id'])
            # Always target an unvisited choice when one remains.
            choices=[i for i in closings if i!=c.state()['entry_id']]
            unseen=[i for i in choices if i not in seen]
            if unseen:
                c.evaluate('Math.random=()=>'+str((choices.index(unseen[0])+.1)/len(choices))); c.click('another-closing')
        self.assertEqual(seen,set(closings))

    def test_initial_random_opener_no_auto_progression_turnover_and_zero_question_release(self):
        c=self.c; s=c.state()
        self.assertEqual(s['view'],'opener'); self.assertTrue(s['entry_id'].startswith('OPEN-'))
        self.assertEqual(s['draws'],1)
        c.screenshot('opener')
        c.time.sleep(.2); self.assertEqual(c.state(),s)
        for _ in range(15):
            before=c.state(); c.tap('simulate-turnover')
            self.assertNotEqual(c.state()['entry_id'],before['entry_id'])
        c.click('options'); c.click('release'); self.assertEqual(c.state()['view'],'closing')
        self.assertEqual(c.state()['path'],None); c.screenshot('release-zero-questions')
        prior=c.state(); c.click('simulate-turnover'); self.assertEqual(c.state(),prior)
        c.click('back'); self.assertEqual(c.state()['view'],'options')
        c.click('back'); self.assertEqual(c.state()['view'],'opener')
        c.click('options'); c.click('another-opener'); self.assertEqual(c.state()['view'],'opener')
        self.assertTrue(c.evaluate('Object.isFrozen(window.compassCoaching) && Object.isFrozen(window.compassCoaching.snapshot())'))
        self.assertEqual(c.evaluate('performance.getEntriesByType("resource").length'),0)


if __name__ == '__main__':
    unittest.main()
