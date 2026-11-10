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
    counts = dict(Counter(e['type'] for e in entries))
    ui = (native / 'coaching_ui.h').read_text()
    model = (native / 'coaching_model.h').read_text()
    for literal in ('"Try (optional)"', '"Question"', '"Paths"', '"Another opener"', '"Guide me - optional"', '"Private listening cues"', '"Next step - optional"', '"Shake ready"', '"Scroll for full text"', '6000', '2000'):
        assert literal in ui, literal
    assert 'nextIndex<0?firstIndex:nextIndex' in model
    out.mkdir(parents=True, exist_ok=True)
    sections = [
        ('Coaching display guide', [
            ('Start', 'From the Classic compass, tap the centre, then Coaching. Choose Progress, Stuck, Unclear, Explore, AI or Team. An opening question is optional: tap the Coaching compass centre, then Another opener.'),
            ('Questions and tries', 'Paths changes context. Another cycles through questions or experiments in the current path. Try (optional) switches to experiments; Question switches back. Each path remembers its question and experiment separately.'),
            ('Guide me', 'Guide me opens guidance for the current entry. Back returns to where you came from. For an opening card, use Options > Guide me. Guide also offers Release, Opening and Reminders.'),
            ('Finishing and reminders', 'Release opens the closing prompts. Options > Reminders opens the listening reminders. Both screens offer Back, Release, Opening and Next. Next cycles within that group; Opening returns to the Coaching compass. These screens have no Guide me.'),
            ('Options', 'Tap the compass centre, or Options on an opening card. The menu has Back, Another opener, Guide me, Release, Classic, Instant ON/OFF, Reminders and Sound ON/OFF. Classic changes mode; Instant skips the opening animation but not its reading hold; Sound controls the optional cue.'),
            ('Scrolling and motion', 'Scroll when Scroll for full text appears. S arms a temporary shake request; tapping S does not draw a card. Accepted shakes or flips draw an opener, not the next question. Coaching automatic-shake reliability remains unresolved; use the touch controls when needed.'),
            ('About the entries', 'Each following page contains the unchanged screen text and, where available, its Guide me text. Page headings help you browse; they are not extra screen text. Version 31. Content remains DRAFT.')])
    ]
    positions = Counter()
    headings = {}
    for e in entries:
        key = (e['type'], e['path'])
        positions[key] += 1
        number = positions[key]
        if e['type'] in ('question', 'experiment'):
            face = 'Question' if e['type'] == 'question' else 'Try'
            headings[e['id']] = f"{labels[e['path']]} · {face} {number}"
        else:
            name = {'opener': 'Opening', 'closing': 'Closing', 'reminder': 'Reminder'}[e['type']]
            headings[e['id']] = f'{name} {number}'
    md = []
    for title, blocks in sections:
        md.extend(['# ' + title, ''])
        for head, text in blocks:
            md.extend(['## ' + head, '', text, ''])
    for e in entries:
        md.extend(['# ' + headings[e['id']], '', '## On screen', '', e['text'], ''])
        if e['guidance']:
            md.extend(['## Guide me', '', e['guidance'], ''])
    hashes = {name: hashlib.sha256((native/name).read_bytes()).hexdigest() for name in ('coaching_generated.h', 'coaching_model.h', 'coaching_ui.h', 'ui.h')}
    md_text = '\n'.join(md)
    (out/'COACHING-DISPLAY-GUIDE.md').write_text(md_text)
    for e in entries:
        block = md_text.split('# ' + headings[e['id']] + '\n', 1)[1].split('\n# ', 1)[0]
        assert '\n' + e['text'] + '\n' in block, e['id'] + ' Markdown main'
        if e['guidance']:
            assert '\n' + e['guidance'] + '\n' in block, e['id'] + ' Markdown guidance'
    from reportlab.platypus import SimpleDocTemplate, Paragraph, PageBreak
    from reportlab.lib.styles import ParagraphStyle
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont
    from reportlab.lib.pagesizes import A4
    from reportlab.lib.colors import HexColor
    fontdir = Path('/usr/share/fonts/truetype/dejavu')
    pdfmetrics.registerFont(TTFont('Guide', str(fontdir/'DejaVuSans.ttf')))
    pdfmetrics.registerFont(TTFont('GuideBold', str(fontdir/'DejaVuSans-Bold.ttf')))
    styles = {k: ParagraphStyle(k, fontName='GuideBold' if k in ('title', 'heading', 'main') else 'Guide', fontSize=size, leading=lead, spaceAfter=after, textColor=HexColor('#17334a') if k in ('title', 'heading') else HexColor('#111111')) for k, size, lead, after in [('title', 19, 24, 16), ('heading', 11, 15, 6), ('body', 11, 15, 10), ('main', 14, 20, 18), ('guidance', 12, 17, 12)]}
    def p(text, style='body'):
        return Paragraph(html.escape(text).replace('\n', '<br/>'), styles[style])
    story = []
    for title, blocks in sections:
        if story:
            story.append(PageBreak())
        story.append(p(title, 'title'))
        for head, text in blocks:
            story.extend([p(head, 'heading'), p(text)])
    for e in entries:
        story.extend([PageBreak(), p(headings[e['id']], 'title'), p('On screen', 'heading'), p(e['text'], 'main')])
        if e['guidance']:
            story.extend([p('Guide me', 'heading'), p(e['guidance'], 'guidance')])
    def footer(canvas, doc):
        canvas.setFont('Guide', 8)
        canvas.drawString(42, 23, 'TrailGuide · Coaching')
        canvas.drawRightString(A4[0]-42, 23, str(doc.page))
    pdf = out/'COACHING-DISPLAY-GUIDE.pdf'
    SimpleDocTemplate(str(pdf), pagesize=A4, leftMargin=42, rightMargin=42, topMargin=32, bottomMargin=40, title='Coaching Display Guide - DRAFT version 31', author='TrailGuide').build(story, onFirstPage=footer, onLaterPages=footer)
    result = subprocess.run(['pdftotext', '-layout', str(pdf), '-'], check=True, capture_output=True, text=True)
    pages = [x for x in result.stdout.split('\f') if x.strip()]
    assert len(pages) == len(sections) + len(entries), ('Unexpected split/overflow', len(pages))
    coverage = []
    for offset, e in enumerate(entries, len(sections)):
        page = pages[offset]
        assert compact(headings[e['id']]) in compact(page)
        assert compact(e['text']) in compact(page), e['id'] + ' main'
        if e['guidance']:
            assert compact(e['guidance']) in compact(page), e['id'] + ' guide'
        other_headings = [heading for key, heading in headings.items() if key != e['id'] and heading in page]
        assert not other_headings, (e['id'], other_headings)
        coverage.append({'id': e['id'], 'type': e['type'], 'heading': headings[e['id']], 'page': offset+1, 'main_exact': True, 'guidance_exact': bool(e['guidance']), 'stored_guidance_empty': not bool(e['guidance'])})
    report = {'status': 'PASS', 'version': 'DRAFT version 31', 'entry_count': len(entries), 'counts': counts, 'pages': len(pages), 'intro_pages': len(sections), 'nonempty_guidance_blocks': sum(bool(e['guidance']) for e in entries), 'all_entry_ids_and_order_verified': True, 'canonical_regeneration_matches_native': True, 'whitespace_only_pdf_comparison': True, 'one_entry_per_page': True, 'native_source_sha256': hashes, 'coverage': coverage, 'pdf_sha256': hashlib.sha256(pdf.read_bytes()).hexdigest()}
    (out/'COACHING-DISPLAY-GUIDE.coverage.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k != 'coverage'}, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--native', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    build(args.repo.resolve(), (args.native or args.repo/'trailguide_v3').resolve(), (args.output or args.repo/'docs').resolve())


if __name__ == '__main__':
    main()
