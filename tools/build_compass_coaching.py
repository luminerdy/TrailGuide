#!/usr/bin/env python3
"""Generate the standalone DRAFT compass navigation study (stdlib only)."""
import argparse
import copy
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PATHS = ('progress', 'stuck', 'unclear', 'exploring', 'ai-results', 'team')
CLOCKS = (12, 2, 4, 6, 8, 10)
TYPES = {'opener', 'question', 'experiment', 'closing', 'reminder'}
# Legacy question is intentionally not an allowed paired Ask source.
FIELDS = {'facilitator.ask', 'facilitator.try_invitation', 'activity', 'action'}


def validate(layer, catalog):
    """Resolve exact, explicitly allowed references without changing either input."""
    def require(ok, message):
        if not ok:
            raise ValueError(message)
    def text(v):
        return isinstance(v, str) and bool(v.strip())
    require(isinstance(layer, dict) and type(layer.get('schema_version')) is int and layer['schema_version']==1,
            'Expected coaching schema_version 1')
    require(set(layer)=={'schema_version', 'review_state', 'paths', 'entries'} and layer.get('review_state')=='draft', 'Expected draft-only coaching object')
    require(isinstance(catalog, dict) and isinstance(catalog.get('cards'), list), 'Expected canonical cards')
    by_id = {}
    for c in catalog['cards']:
        require(isinstance(c, dict) and isinstance(c.get('card_id'),str) and re.fullmatch(r'tg-\d{3}', c['card_id']) and c['card_id'] not in by_id, 'Invalid canonical card identity')
        by_id[c['card_id']] = c
    paths=layer['paths']
    require(isinstance(paths,list) and len(paths)==6, 'Expected six compass paths')
    for p, name, clock in zip(paths, PATHS, CLOCKS):
        require(isinstance(p,dict) and set(p)=={'id','label','clock'} and p['id']==name and type(p['clock']) is int and p['clock']==clock and text(p['label']), 'Invalid path/clock')
    require(isinstance(layer['entries'],list), 'Expected entries list')
    result=copy.deepcopy(layer)
    seen=set()
    for e in result['entries']:
        require(isinstance(e,dict), 'Expected entry object')
        require(set(e)<={'id','type','path','text','text_ref','guidance','related_cards','review_state'}, 'Unknown entry field')
        identity=e.get('id')
        require(isinstance(identity,str) and re.fullmatch(r'[A-Z][A-Z0-9]*(?:-[A-Z0-9]+)+',identity) and identity not in seen, 'Malformed/duplicate entry ID')
        seen.add(identity)
        require(isinstance(e.get('type'),str) and e['type'] in TYPES and e.get('review_state')=='draft', 'Invalid type or non-draft entry')
        if e['type'] in {'question','experiment'}:
            require(isinstance(e.get('path'),str) and e['path'] in PATHS, 'Missing/invalid path')
        else:
            require('path' not in e, 'Unexpected path')
        require(('text' in e) != ('text_ref' in e), 'Choose text OR text_ref')
        if 'text_ref' in e:
            ref=e['text_ref']
            require(isinstance(ref,dict) and set(ref)=={'card_id','field'} and isinstance(ref['card_id'],str) and ref['card_id'] in by_id and isinstance(ref['field'],str) and ref['field'] in FIELDS, 'Unknown reference or disallowed field')
            c=by_id[ref['card_id']]
            require(c.get('review_state')=='draft', 'Referenced card must remain draft')
            value=c
            for key in ref['field'].split('.'):
                require(isinstance(value,dict) and key in value, 'Missing reference field')
                value=value[key]
            if ref['field'].startswith('facilitator.'):
                f=c['facilitator']
                require(f.get('review_state')=='draft', 'Referenced Ask is not draft')
                if ref['field'].endswith('try_invitation'):
                    require(f.get('try_review_state')=='draft', 'Referenced Try is not draft')
            require(text(value), 'Reference must resolve to text')
            e['text']=value
        require(text(e.get('text')), 'Missing entry text')
        require('guidance' not in e or text(e['guidance']), 'Invalid guidance')
        if 'related_cards' in e:
            require(isinstance(e['related_cards'],list) and all(isinstance(v,str) and v in by_id for v in e['related_cards']), 'Unknown related card')
    return result


def render(layer, catalog, template):
    content=validate(layer,catalog)
    if template.count('__COACHING_JSON__')!=1:
        raise ValueError('Expected exactly one coaching marker')
    encoded=json.dumps(content,ensure_ascii=True,separators=(',',':')).replace('<','\\u003c').replace('>','\\u003e').replace('&','\\u0026')
    return template.replace('__COACHING_JSON__',encoded)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--content',type=Path,default=ROOT/'content/coaching.json')
    parser.add_argument('--cards',type=Path,default=ROOT/'content/cards.json')
    parser.add_argument('--template',type=Path,default=ROOT/'prototype/compass_coaching_template.html')
    parser.add_argument('--output',type=Path,default=ROOT/'prototype/compass-coaching.html')
    parser.add_argument('--check',action='store_true')
    args=parser.parse_args()
    try:
        output=render(json.loads(args.content.read_text(encoding='utf-8')),json.loads(args.cards.read_text(encoding='utf-8')),args.template.read_text(encoding='utf-8'))
        if args.check:
            if not args.output.exists() or args.output.read_text(encoding='utf-8')!=output:
                parser.exit(1,'Generated compass preview is stale; run tools/build_compass_coaching.py\n')
            print('Compass coaching preview is current (DRAFT; read-only).')
        else:
            args.output.parent.mkdir(parents=True,exist_ok=True)
            args.output.write_text(output,encoding='utf-8')
            print('Built standalone DRAFT compass coaching preview; no firmware changed.')
    except (ValueError,OSError) as error:
        parser.exit(1,'Invalid coaching preview: '+str(error)+'\n')


if __name__=='__main__':
    main()
