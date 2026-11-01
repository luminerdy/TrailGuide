#!/usr/bin/env python3
"""Generate the separate portable V3 draft experience study (not firmware)."""
import argparse
import json
from pathlib import Path

from build_prototype import IDS, select_content

ROOT = Path(__file__).resolve().parents[1]


def render(data, template):
    selected = select_content(data)
    assert tuple(c['card_id'] for c in selected['cards']) == IDS
    if template.count('__CONTENT_JSON__') != 1:
        raise ValueError('Expected exactly one template content marker')
    encoded = json.dumps(selected, ensure_ascii=True, separators=(',', ':'))
    for character, escaped in (('<', '\\u003c'), ('>', '\\u003e'), ('&', '\\u0026')):
        encoded = encoded.replace(character, escaped)
    return template.replace('__CONTENT_JSON__', encoded)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--content', type=Path, default=ROOT / 'content/cards.json')
    parser.add_argument('--template', type=Path, default=ROOT / 'prototype/v3_experience_template.html')
    parser.add_argument('--output', type=Path, default=ROOT / 'prototype/v3-experience.html')
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    try:
        output = render(json.loads(args.content.read_text(encoding='utf-8')), args.template.read_text(encoding='utf-8'))
    except (ValueError, OSError) as error:
        parser.exit(1, 'Invalid experience content: ' + str(error) + '\n')
    if args.check:
        if not args.output.exists() or args.output.read_text(encoding='utf-8') != output:
            parser.exit(1, 'Generated experience is stale; run tools/build_v3_experience.py\n')
        print('V3 experience is current (eight draft pairs; simulated shake only).')
    else:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output, encoding='utf-8')
        print('Built V3 draft experience study (eight draft pairs; simulated shake only).')


if __name__ == '__main__':
    main()
