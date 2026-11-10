#!/usr/bin/env python3
"""Build the DRAFT Coaching handbook from verified native consumer strings.

PDF generation requires ReportLab, DejaVuSans/DejaVuSans-Bold TrueType fonts
under /usr/share/fonts/truetype/dejavu, and pdftotext for page validation.
The published Markdown and PDF can be read without these build dependencies.
Run: python tools/build_coaching_display_guide.py --repo . --native trailguide_v3 --output docs
The native directory may instead be a frozen installed-source snapshot.
No content, generated header, firmware or build output is modified.
"""
import argparse
from collections import Counter
import hashlib
import html
import importlib
import json
from pathlib import Path
import re
import subprocess
import sys


def compact(s):
    return re.sub(r'\s+', '', s)


def build(repo, native, out):
    sys.dont_write_bytecode = True
    sys.path.insert(0, str(repo / 'tools'))
    generator = importlib.import_module('build_v3_coaching')
    layer = json.loads((repo / 'content/coaching.json').read_text())
    catalog = json.loads((repo / 'content/cards.json').read_text())
    header = (native / 'coaching_generated.h').read_text()
    assert generator.render(layer, catalog) == header, 'Canonical/generated mismatch'
    entries = []
    for line in header.splitlines():
        if re.match(r'\s*\{"', line):
            values = [json.loads(s) for s in re.findall(r'"(?:\\.|[^"\\])*"', line)]
            assert len(values) == 4
            path_match = re.search(r'",(-?\d+),"', line)
            if path_match is None:
                raise ValueError('Malformed native Coaching entry path')
            path = int(path_match.group(1))
            entries.append(dict(zip(('id', 'type', 'text', 'guidance'), values), path=path))
    assert [e['id'] for e in entries] == [e['id'] for e in layer['entries']]
    assert len({e['id'] for e in entries}) == len(entries)
    labels = [p['label'] for p in layer['paths']]
    short = ['Progress', 'Stuck', 'Unclear', 'Explore', 'AI', 'Team']
    counts = dict(Counter(e['type'] for e in entries))
    ui = (native / 'coaching_ui.h').read_text()
    model = (native / 'coaching_model.h').read_text()
    for literal in ('"Try (optional)"', '"Question"', '"Paths"', '"Another opener"', '"Guide me - optional"', '"Private listening cues"', '"Next step - optional"', '"Shake ready"', '"Scroll for full text"', '6000', '2000'):
        assert literal in ui, literal
    assert 'nextIndex<0?firstIndex:nextIndex' in model
    out.mkdir(parents=True, exist_ok=True)
    sections = [
      ('Coaching display guide', [
       ('Scope and reading key', 'DRAFT | version 31. This is a screen-text and navigation reference, not new coaching copy or a change to startup. Classic remains the boot mode. Enter Coaching from the Classic compass: tap the compass centre, then Coaching. Coaching opens its compass, with the opener hidden. You may choose a path directly; an opener, a fixed number of questions and an experiment are not prerequisites.'),
       ('Six contexts, not six activities', 'The compass buttons, clockwise from the top, read Progress, Stuck, Unclear, Explore, AI and Team. The corresponding path headings on Question, Experiment and path Guide screens read Progress, Stuck, Unclear, Exploring, AI Results and Team. The wandering compass arrow is decorative; the user chooses the path.'),
       ('How this book is arranged', 'Each authored entry has its own page, in generated source order: openers, path questions, path experiments, closings, reminders. Main text and its matching Guide me wording stay together; Question and Experiment entries are separate pages. IDs, type headings and access instructions are document lookup aids, not text painted on the main screen. There is no question-to-experiment pairing by number: each path independently remembers its question and experiment.'),
       ('Inventory', ', '.join(f'{counts[k]} {k} entries' for k in ('opener','question','experiment','closing','reminder')) + f'; {len(entries)} total entries. Every resolved text and every nonempty guidance field is reproduced below. Native generated punctuation is preserved; line wrapping may differ from the device. PDF lookup: openers pages 4-9; questions 10-33; experiments 34-51; closings 52-54; reminders 55-58. Choose what would help. is a defensive no-entry fallback, not an authored card or a normal selected-entry screen.'),
       ('Walkthrough checks', '[ ] Enter Coaching without an opener. [ ] Choose all six paths. [ ] Compare each main text and matching Guide me. [ ] Scroll to every final line. [ ] Check Back and face restoration. [ ] Check Release and the reminder cycle. These are content/navigation checks, not a physical gesture-reliability claim.')]),
      ('Compass, opening and options', [
       ('Compass and optional opening', 'On the Coaching compass, the six path buttons are visible. Its centre has an unlabelled Options target; there is no painted Options caption on that compass. Tapping it opens Options. Another opener draws an opener and leaves the compass view. The opening card has Options plus the same six path buttons; choose a path whenever useful. Options > Guide me shows the saved current opener guidance, including when the opener was hidden behind the compass.'),
       ('Options: visible labels and effects', 'Back returns to the saved prior view. Another opener selects a fresh opener from a shuffled opener pool (no immediate repeat across pool boundaries when more than one opener exists), clears back history and starts the new-card reveal. Guide me opens guidance for the current entry; from Options its Back first returns to Options. Release opens the first closing. Classic exits Coaching. Instant ON / Instant OFF toggles animated opening reveals; returning from a toggle uses the saved Options context. Reminders opens the first reminder. Sound ON / Sound OFF toggles the shared optional sound cue; it is not recording or speech. No accounts or persistent coaching history are added.'),
       ('Reveal, reading hold and complete text', 'A new opener draw normally reveals over six seconds, followed by a two-second reading hold with navigation hidden. Instant skips the animation, not the reading hold. A non-drag tap during reveal completes the same opener and begins the hold; a settled non-drag tap can expose controls early. Missing presentation resources may leave instant/full-copy access instead. Opening, Guide, Back, path changes and Another within a path do not start a fresh-opener animation. Scroll for full text appears only when text extends below the viewport; scroll the body to reach the complete copy.'),
       ('Motion and S', 'An accepted shake or flip starts fresh with another opener from any Coaching view, cancelling the prior reveal/hold and clearing back history; it is not Another within a path. Touch/scroll contact suppresses these actions. S is a manual shake-request button on a visible opening card and on Question/Experiment screens, not the compass or Guide. It arms a bounded eight-second shake request when eligible; Shake ready is the temporary status. It does not select a path, switch faces or draw on the tap alone. Navigation/contact/expiry cancels the request. Qualified inversion may temporarily blank the screen; a cancelling touch restores the prior scene without forwarding that same click.')]),
      ('Path, Guide and finishing routes', [
       ('Question and Experiment controls', 'Both faces have Paths, Guide me and Release across the top. Question has Another and Try (optional) below; Experiment has Another and Question. Another cycles only the current path and current type in authored order, wrapping to its first entry. First use starts at Q01 or E01. Try (optional) restores that path\'s saved experiment (initially E01); Question restores its saved question. Switching does not pair entries by number, draw an opener, or push another back-history frame.'),
       ('Paths picker', 'Paths opens the six context buttons and the label Choose a path, plus Opening, Next step and Back. Back restores the prior face and scroll position. Choosing the current path returns to that face; choosing a different path restores that path\'s last Question/Experiment face and clears back history. Opening returns to the Coaching compass with the saved opener hidden and clears history. Next step opens the first closing; it does not advance a question quota.'),
       ('Guide me routes', 'Guide displays the current entry\'s guidance, not a newly drawn entry. A path Guide keeps the corresponding full path heading; opener Guide reads Guide me - optional. Visible controls are Back, Release, Opening and Reminders. Back restores its immediate origin (Question, Experiment or Options) and its saved scroll; Opening returns to the compass; Reminders opens the first reminder; Release opens the first closing. Guide is optional and does not consume Another. Nested returns use bounded RAM-only back history; a fresh opener or Opening clears it.'),
       ('Reminders and closing', 'Reminders is reached from Options or Guide. The screen heading is Private listening cues; Next cycles reminders in authored order and wraps. Release, or Next step in the path picker, opens CLOSE-01 with heading Next step - optional. Next cycles the three closings and wraps. Both kinds have Back, Release, Opening and Next. Back returns to the saved immediate origin; Opening returns to the compass. Release on an existing Closing leaves that closing unchanged. These screens do not offer Guide me; their stored guidance fields are empty. No final answer, experiment, recording or collected response is required.'),
       ('Saved state and access checklist', 'The model retains opener, per-path Question and Experiment positions, last face and scroll in RAM, not a persistent journal. Re-entering Coaching returns to its compass without forcing a new visible opener. [ ] Paths > Back restores the face. [ ] Guide me > Back preserves the message. [ ] Try (optional) > Question restores independent positions. [ ] Next wraps closings/reminders. [ ] Opening returns to compass. [ ] A fresh opener resets back history.')])
    ]
    def route(e):
        kind=e['type']
        if kind=='opener':
            return ('Options > Another opener; repeat opener draws until this ID\'s wording appears. Opener order is shuffled, not this book\'s order. For matching guidance: Options > Guide me. Back from Guide returns to Options, then Back restores the opener. The same six context buttons can be chosen directly.', 'Options; Progress; Stuck; Unclear; Explore; AI; Team; S. Conditional status: Scroll for full text / Shake ready.')
        if kind in ('question','experiment'):
            pool=[x for x in entries if x['type']==kind and x['path']==e['path']]
            index=pool.index(e)
            access=f"Choose {short[e['path']]} (screen heading: {labels[e['path']]}). "
            access+=('If this path restores an Experiment, tap Question. ' if kind=='question' else 'Tap Try (optional) on Question; if this path already restores an Experiment, stay on that face. ')
            access+=f"From the first entry, use Another {index} time(s); otherwise cycle until this wording appears. Another wraps in this face's authored order. Guide me opens exactly the guidance below; Back restores this same entry."
            options='Paths; Guide me; Release; Another; '+('Try (optional)' if kind=='question' else 'Question')+'; S. Conditional status: Scroll for full text / Shake ready.'
            return access,options
        group=[x for x in entries if x['type']==kind]
        index=group.index(e)
        return ((('Options or Guide > Reminders' if kind=='reminder' else 'Release from a non-Closing view, or Paths > Next step')+f' starts at {group[0]["id"]}. Tap Next {index} time(s) to reach this entry; Next wraps in authored order. Heading: '+('Private listening cues.' if kind=='reminder' else 'Next step - optional.')), 'Back; Release; Opening; Next. Conditional status: Scroll for full text. No Guide me control; the stored guidance is empty.')
    md=[]
    for title, blocks in sections:
        md.extend(['# '+title,''])
        for head,text in blocks:md.extend(['## '+head,'',text,''])
    for e in entries:
        access,options=route(e)
        md.extend(['# '+e['type'].upper()+' | '+e['id'],'','Lookup heading only; one entry per PDF page.','','## Access and navigation','',access,'','Visible options: '+options,'','## Main screen - exact display text','',e['text'],'','## Guide me - exact display text' if e['guidance'] else '## Guidance availability','',e['guidance'] or 'No Guide me control on this screen; stored guidance is empty.','','Checks: [ ] Main wording [ ] Matching guidance or no Guide control [ ] Full text reachable [ ] Navigation/return',''])
    hashes={name:hashlib.sha256((native/name).read_bytes()).hexdigest() for name in ('coaching_generated.h','coaching_model.h','coaching_ui.h','ui.h')}
    md.extend(['# Source and verification','','Canonical inputs: content/coaching.json and content/cards.json. Resolver: tools/build_compass_coaching.py. Native generator: tools/build_v3_coaching.py. Main/Guide binding: trailguide_v3/coaching_ui.h (entry->text / entry->guidance); sequencing and saved context: trailguide_v3/coaching_model.h. Classic entry route: trailguide_v3/ui.h. The builder requires canonical regeneration to equal the supplied native header and preserves entry IDs/order.','','Native-source SHA-256:',''])
    for name,digest in hashes.items():md.append(f'- `{name}`: `{digest}`')
    md.append('')
    md_text='\n'.join(md)
    (out/'COACHING-DISPLAY-GUIDE.md').write_text(md_text)
    for e in entries:
        block=md_text.split('# '+e['type'].upper()+' | '+e['id']+'\n',1)[1].split('\n# ',1)[0]
        assert '\n'+e['text']+'\n' in block, e['id']+' Markdown main'
        if e['guidance']:assert '\n'+e['guidance']+'\n' in block, e['id']+' Markdown guidance'
    from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, PageBreak
    from reportlab.lib.styles import ParagraphStyle
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont
    from reportlab.lib.pagesizes import A4
    from reportlab.lib.colors import HexColor
    fontdir=Path('/usr/share/fonts/truetype/dejavu')
    pdfmetrics.registerFont(TTFont('Guide',str(fontdir/'DejaVuSans.ttf')))
    pdfmetrics.registerFont(TTFont('GuideBold',str(fontdir/'DejaVuSans-Bold.ttf')))
    styles={k:ParagraphStyle(k,fontName='GuideBold' if k in ('title','heading','main') else 'Guide',fontSize=size,leading=lead,spaceAfter=after,textColor=HexColor('#17334a') if k in ('title','heading') else HexColor('#111111')) for k,size,lead,after in [('title',19,24,13),('heading',11,15,6),('body',11,15,9),('main',13,18,13),('small',9,12,8)]}
    def p(text,style='body'):
        return Paragraph(html.escape(text).replace('\n','<br/>'),styles[style])
    story=[]
    for title,blocks in sections:
        if story:story.append(PageBreak())
        story.append(p(title,'title'))
        for head,text in blocks:story.extend([p(head,'heading'),p(text)])
    for e in entries:
        access,options=route(e)
        story.extend([PageBreak(),p(e['type'].upper()+' | '+e['id'],'title'),p('Document lookup heading - not a main-screen caption.','small'),p('Access and navigation','heading'),p(access),p('Visible options: '+options,'small'),p('Main screen - exact display text','heading'),p(e['text'],'main'),p('Guide me - exact display text' if e['guidance'] else 'Guidance availability','heading'),p(e['guidance'] or 'No Guide me control on this screen; stored guidance is empty.'),Spacer(1,8),p('[ ] Main wording  [ ] Matching guidance / no Guide control','small'),p('[ ] Complete text reachable  [ ] Navigation and return','small'),p('Notes: __________________________________________________','small')])
    def footer(canvas,doc):
        canvas.setFont('Guide',8);canvas.drawString(42,23,'TrailGuide | Coaching display guide | DRAFT version 31');canvas.drawRightString(A4[0]-42,23,str(doc.page))
    pdf=out/'COACHING-DISPLAY-GUIDE.pdf'
    SimpleDocTemplate(str(pdf),pagesize=A4,leftMargin=42,rightMargin=42,topMargin=32,bottomMargin=40,title='Coaching Display Guide - DRAFT version 31',author='TrailGuide').build(story,onFirstPage=footer,onLaterPages=footer)
    result=subprocess.run(['pdftotext','-layout',str(pdf),'-'],check=True,capture_output=True,text=True)
    pages=[x for x in result.stdout.split('\f') if x.strip()]
    assert len(pages)==len(sections)+len(entries), ('Unexpected split/overflow',len(pages))
    coverage=[]
    for offset,e in enumerate(entries,len(sections)):
        page=pages[offset]
        assert e['type'].upper()+' | '+e['id'] in page
        assert compact(e['text']) in compact(page),e['id']+' main'
        if e['guidance']:assert compact(e['guidance']) in compact(page),e['id']+' guide'
        others=[x['id'] for x in entries if x['id']!=e['id'] and re.search(r'\| '+re.escape(x['id'])+r'\b',page)]
        assert not others,(e['id'],others)
        coverage.append({'id':e['id'],'type':e['type'],'page':offset+1,'main_exact':True,'guidance_exact':bool(e['guidance']),'stored_guidance_empty':not bool(e['guidance'])})
    report={'status':'PASS','version':'DRAFT version 31','entry_count':len(entries),'counts':counts,'pages':len(pages),'intro_pages':len(sections),'nonempty_guidance_blocks':sum(bool(e['guidance']) for e in entries),'all_entry_ids_and_order_verified':True,'canonical_regeneration_matches_native':True,'whitespace_only_pdf_comparison':True,'one_entry_per_page':True,'native_source_sha256':hashes,'coverage':coverage,'pdf_sha256':hashlib.sha256(pdf.read_bytes()).hexdigest()}
    (out/'COACHING-DISPLAY-GUIDE.coverage.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='coverage'},indent=2))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo',type=Path,default=Path(__file__).resolve().parents[1])
    parser.add_argument('--native',type=Path)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    build(args.repo.resolve(),(args.native or args.repo/'trailguide_v3').resolve(),(args.output or args.repo/'docs').resolve())


if __name__=='__main__':main()
