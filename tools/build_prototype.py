#!/usr/bin/env python3
"""Build the portable draft-only read-only browser preview; no dependencies."""
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
IDS = ('tg-001', 'tg-002', 'tg-004', 'tg-003', 'tg-011', 'tg-028', 'tg-031', 'tg-036')


def select_content(data):
    def text(value):
        return isinstance(value, str) and bool(value.strip())
    if not isinstance(data, dict) or type(data.get('schema_version')) is not int or data['schema_version'] != 1:
        raise ValueError('Expected content object with schema_version 1')
    themes = data.get('themes')
    valid_themes = {'explore', 'clarify', 'experiment', 'verify', 'reflect', 'unstick', 'include', 'decide'}
    if not isinstance(themes, list) or len(themes) != 8:
        raise ValueError('Expected exactly eight theme entries')
    if any(not isinstance(t, dict) or not text(t.get('theme_id')) or not text(t.get('label')) for t in themes):
        raise ValueError('Expected theme objects with nonempty theme_id and label')
    theme_ids = {t['theme_id'] for t in themes}
    if theme_ids != valid_themes:
        raise ValueError('Expected eight unique valid themes')
    cards = data.get('cards')
    if not isinstance(cards, list) or any(not isinstance(c, dict) or not text(c.get('card_id')) for c in cards):
        raise ValueError('Expected card objects with nonempty card_id')
    by_id = {c['card_id']: c for c in cards}
    if len(by_id) != len(cards):
        raise ValueError('Duplicate card IDs')
    if any(i not in by_id for i in IDS):
        raise ValueError('Missing representative ID')
    selected = [by_id[i] for i in IDS]
    if any(not text(c.get('theme')) or c['theme'] not in theme_ids for c in selected):
        raise ValueError('Invalid representative theme')
    if len({c['theme'] for c in selected}) != 8:
        raise ValueError('Expected eight distinct representative themes')
    for card in selected:
        f = card.get('facilitator')
        if not isinstance(f, dict):
            raise ValueError('Expected paired facilitator object')
        if any(state != 'draft' for state in (card.get('review_state'), f.get('review_state'), f.get('try_review_state'))):
            raise ValueError('Preview requires draft-only base, Ask and Try scopes')
        if type(card.get('revision')) is not int or card['revision'] < 1 or card['theme'] not in theme_ids:
            raise ValueError('Invalid identity/revision/theme')
        if not all(text(card.get(k)) for k in ('title', 'action', 'activity')) or not all(text(f.get(k)) for k in ('ask', 'try_invitation')):
            raise ValueError('Missing real paired copy; no runtime question fallback')
        for key in ('notice', 'follow_ups', 'try_notice', 'try_follow_ups'):
            values = f.get(key)
            if not isinstance(values, list) or not values or not all(text(v) for v in values) or ('follow_ups' in key and len(values) > 2):
                raise ValueError('Invalid facilitator ' + key)
    fields = ('card_id', 'revision', 'theme', 'title', 'action', 'activity', 'review_state', 'facilitator')
    return {'themes': themes, 'cards': [{k: c[k] for k in fields} for c in selected]}


def render(data, template):
    selected = select_content(data)
    encoded = json.dumps(selected, ensure_ascii=True, separators=(',', ':')).replace('<', '\\u003c').replace('>', '\\u003e').replace('&', '\\u0026')
    if template.count('__CONTENT_JSON__') != 1:
        raise ValueError('Expected exactly one template content marker')
    return template.replace('__CONTENT_JSON__', encoded)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--content', type=Path, default=ROOT / 'content/cards.json')
    parser.add_argument('--template', type=Path, default=ROOT / 'prototype/template.html')
    parser.add_argument('--output', type=Path, default=ROOT / 'prototype/index.html')
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    try:
        output = render(json.loads(args.content.read_text(encoding='utf-8')), args.template.read_text(encoding='utf-8'))
    except (ValueError, OSError) as error:
        parser.exit(1, 'Invalid preview content: ' + str(error) + '\n')
    if args.check:
        if not args.output.exists() or args.output.read_text(encoding='utf-8') != output:
            parser.exit(1, 'Generated prototype is stale; run tools/build_prototype.py\n')
        print('Prototype is current (eight draft pairs).')
    else:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(output, encoding='utf-8')
        print('Built read-only prototype (eight draft pairs).')


if __name__ == '__main__':
    main()
