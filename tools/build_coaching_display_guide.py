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
    intro = [
        ('Start', 'From the Classic compass, tap the centre, then Coaching. Choose Progress, Stuck, Unclear, Explore, AI or Team. An opener is optional: tap the Coaching compass centre, then Another opener.'),
        ('Controls', 'Paths changes context. Another cycles within the current path and question/try type. Try (optional) and Question switch types; each remembers its own place. Guide me opens the current guidance; Back returns to where you came from. For an opener, use Options > Guide me.'),
        ('Finishing and reminders', 'Release opens the closing prompts. Options > Reminders opens the listening reminders. Next cycles within either group; Opening returns to the compass. Both screens also offer Back and Release, but no Guide me. Guide offers Back, Release, Opening and Reminders.'),
        ('Options', 'The centre of the compass, or Options on an opener, opens the menu: Back, Another opener, Guide me, Release, Classic, Instant ON/OFF, Reminders and Sound ON/OFF. Classic changes mode; Instant skips the opening animation but not its reading hold; Sound controls the optional cue.'),
        ('Scrolling and motion', 'Scroll when Scroll for full text appears. S arms a temporary shake request; the tap does not draw a card. Accepted shakes or flips draw an opener. Coaching automatic-shake reliability remains unresolved; use touch controls when needed.')
    ]
    reading_key = 'Version 31 · DRAFT. Numbered items are screen text; indented paragraphs are their optional Guide me guidance. Questions and tries are separate choices, not paired activities.'
    def pool(kind, path=None):
        return [e for e in entries if e['type'] == kind and (path is None or e['path'] == path)]
    groups = [('Opening questions', [('', pool('opener'))])]
    groups.extend((label, [('Questions', pool('question', path)), ('Tries', pool('experiment', path))]) for path, label in enumerate(labels))
    groups.extend([('Closing prompts', [('', pool('closing'))]), ('Listening reminders', [('', pool('reminder'))])])
    ordered = [e for _, subgroups in groups for _, items in subgroups for e in items]
    assert len(ordered) == len(entries) and {e['id'] for e in ordered} == {e['id'] for e in entries}
    md = ['# Coaching display guide', '', reading_key, '']
    for heading, text in intro:
        md.extend(['**' + heading + '.** ' + text, ''])
    md_blocks = {}
    md_guidance = {}
    shared_guidance_groups = []
    headings = {}
    for title, subgroups in groups:
        md.extend(['## ' + title, ''])
        for subtitle, items in subgroups:
            if subtitle:
                md.extend(['### ' + subtitle, ''])
            shared_guidance = bool(items) and bool(items[0]['guidance']) and len(items) > 1 and len({e['guidance'] for e in items}) == 1
            for number, e in enumerate(items, 1):
                headings[e['id']] = ' / '.join(x for x in (title, subtitle, str(number)) if x)
                block = '**' + str(number) + '.** ' + e['text'] + '\n\n'
                md_guidance[e['id']] = '\n'.join('> ' + line for line in e['guidance'].split('\n')) if e['guidance'] else ''
                if e['guidance'] and not shared_guidance:
                    block += md_guidance[e['id']] + '\n\n'
                md_blocks[e['id']] = block
                md.append(block.rstrip() + '\n')
            if shared_guidance:
                md.extend(['*Guide me for all ' + subtitle.lower() + ' in this section:*', '', md_guidance[items[0]['id']], ''])
                shared_guidance_groups.append({'section': title, 'subsection': subtitle, 'entry_ids': [e['id'] for e in items]})
    md_text = '\n'.join(md)
    (out/'COACHING-DISPLAY-GUIDE.md').write_text(md_text)
    for e in ordered:
        block = md_blocks[e['id']]
        assert block.rstrip() in md_text, e['id'] + ' Markdown grouping'
        assert e['text'] in block, e['id'] + ' Markdown main'
        if e['guidance']:
            quoted = md_guidance[e['id']]
            assert quoted in md_text, e['id'] + ' Markdown guidance presence'
            assert re.sub(r'^> ?', '', quoted, flags=re.MULTILINE) == e['guidance'], e['id'] + ' Markdown guidance'
    hashes = {name: hashlib.sha256((native/name).read_bytes()).hexdigest() for name in ('coaching_generated.h', 'coaching_model.h', 'coaching_ui.h', 'ui.h')}
    from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, KeepTogether, Flowable
    from reportlab.lib.styles import ParagraphStyle
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont
    from reportlab.lib.pagesizes import A4
    from reportlab.lib.colors import HexColor
    fontdir = Path('/usr/share/fonts/truetype/dejavu')
    pdfmetrics.registerFont(TTFont('Guide', str(fontdir/'DejaVuSans.ttf')))
    pdfmetrics.registerFont(TTFont('GuideBold', str(fontdir/'DejaVuSans-Bold.ttf')))
    styles = {k: ParagraphStyle(k, fontName='GuideBold' if k in ('title', 'group', 'subgroup', 'main') else 'Guide', fontSize=size, leading=lead, spaceAfter=after, textColor=HexColor('#17334a') if k in ('title', 'group', 'subgroup') else HexColor('#111111'), keepWithNext=k in ('group', 'subgroup')) for k, size, lead, after in [('title', 19, 24, 12), ('group', 16, 21, 8), ('subgroup', 12, 16, 7), ('body', 11, 15, 8), ('main', 13, 18, 5), ('guidance', 11.5, 16, 8)]}
    styles['group'].spaceBefore = 14
    styles['subgroup'].spaceBefore = 5
    styles['guidance'].leftIndent = 16
    styles['guidance'].textColor = HexColor('#40494f')
    def p(text, style='body'):
        return Paragraph(html.escape(text).replace('\n', '<br/>'), styles[style])
    entry_pages = {}
    class EntryParagraph(Paragraph):
        entry_id: str
    class GuideDocument(SimpleDocTemplate):
        def afterFlowable(self, flowable):
            if isinstance(flowable, EntryParagraph):
                entry_pages[flowable.entry_id] = self.page
    story: list[Flowable] = [p('Coaching display guide', 'title'), p(reading_key)]
    for heading, text in intro:
        story.append(Paragraph('<b>' + html.escape(heading) + '.</b> ' + html.escape(text), styles['body']))
    for title, subgroups in groups:
        pending_headings: list[Flowable] = [p(title, 'group')]
        for subtitle, items in subgroups:
            if subtitle:
                pending_headings.append(p(subtitle, 'subgroup'))
            for number, e in enumerate(items, 1):
                main = EntryParagraph(html.escape(str(number) + '. ' + e['text']).replace('\n', '<br/>'), styles['main'])
                main.entry_id = e['id']
                entry_flowables: list[Flowable] = [*pending_headings, main]
                pending_headings = []
                if e['guidance']:
                    entry_flowables.append(p(e['guidance'], 'guidance'))
                entry_flowables.append(Spacer(1, 5))
                story.append(KeepTogether(entry_flowables))
    def footer(canvas, doc):
        canvas.setFont('Guide', 8)
        canvas.drawString(42, 23, 'TrailGuide · Coaching')
        canvas.drawRightString(A4[0]-42, 23, str(doc.page))
    pdf = out/'COACHING-DISPLAY-GUIDE.pdf'
    GuideDocument(str(pdf), pagesize=A4, leftMargin=42, rightMargin=42, topMargin=32, bottomMargin=40, title='Coaching Display Guide - DRAFT version 31', author='TrailGuide').build(story, onFirstPage=footer, onLaterPages=footer)
    result = subprocess.run(['pdftotext', '-layout', str(pdf), '-'], check=True, capture_output=True, text=True)
    pages = [x for x in result.stdout.split('\f') if x.strip()]
    assert list(entry_pages) == [e['id'] for e in ordered], 'PDF entry order or missing entries'
    coverage = []
    for e in ordered:
        page_number = entry_pages[e['id']]
        page = pages[page_number-1]
        assert compact(e['text']) in compact(page), e['id'] + ' main'
        if e['guidance']:
            assert compact(e['guidance']) in compact(page), e['id'] + ' guidance split or missing'
        coverage.append({'id': e['id'], 'type': e['type'], 'heading': headings[e['id']], 'page': page_number, 'main_exact': True, 'guidance_exact': bool(e['guidance']), 'stored_guidance_empty': not bool(e['guidance'])})
    report = {'status': 'PASS', 'version': 'DRAFT version 31', 'entry_count': len(entries), 'counts': counts, 'pages': len(pages), 'layout': 'continuous; grouped by path, with separate question and try sections', 'nonempty_guidance_blocks': sum(bool(e['guidance']) for e in entries), 'all_entry_ids_verified': True, 'canonical_source_order_verified': True, 'document_entry_order': [e['id'] for e in ordered], 'canonical_regeneration_matches_native': True, 'whitespace_only_pdf_comparison': True, 'one_entry_per_page': False, 'all_entries_kept_together': True, 'native_source_sha256': hashes, 'coverage': coverage, 'pdf_sha256': hashlib.sha256(pdf.read_bytes()).hexdigest()}
    report['markdown_shared_guidance_groups'] = shared_guidance_groups
    (out/'COACHING-DISPLAY-GUIDE.coverage.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: v for k, v in report.items() if k not in ('coverage', 'document_entry_order')}, indent=2))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--native', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    build(args.repo.resolve(), (args.native or args.repo/'trailguide_v3').resolve(), (args.output or args.repo/'docs').resolve())


if __name__ == '__main__':
    main()
