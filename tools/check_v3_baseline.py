#!/usr/bin/env python3
"""Check exact approved owned/prepared source; never compile or touch hardware."""
from pathlib import Path
import argparse
import hashlib
import json
ROOT=Path(__file__).resolve().parents[1]

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def load_manifest(root, manifest=None):
    root=Path(root)
    if manifest is None:
        current=root/'content/v3-current.json'
        manifest=current if current.exists() else root/'content/v3-baseline.json'
    return json.loads(Path(manifest).read_text())

def check_owned(root, manifest=None):
    manifest=load_manifest(root, manifest)
    names=manifest['owned_files']
    if (not isinstance(names,list) or len(names)!=17
            or any(not isinstance(name,str) or not name or name in ('.','..')
                   or '/' in name or '\\' in name for name in names)
            or len(set(names))!=17 or len(manifest['prepared_files'])!=29):
        raise ValueError('Invalid baseline source counts or owned source names')
    source=root/'trailguide_v3'
    if source.is_symlink():raise ValueError('Owned source must be a regular flat directory')
    entries=list(source.iterdir())
    if len(entries)!=17 or any(p.is_symlink() or not p.is_file() for p in entries):
        raise ValueError('Owned source contains unexpected count or non-regular entries')
    actual={p.name:digest(p) for p in entries}
    expected={name:manifest['prepared_files'][name] for name in names}
    if actual!=expected:raise ValueError('Owned source differs from approved baseline')
    return manifest

def check_prepared(root,prepared,manifest=None):
    manifest=check_owned(root,manifest)
    if prepared.is_symlink():raise ValueError('Prepared source must be a regular flat directory')
    entries=list(prepared.iterdir())
    if any(p.is_symlink() or not p.is_file() for p in entries):
        raise ValueError('Prepared source contains unexpected non-regular entries')
    actual={p.name:digest(p) for p in entries}
    if actual!=manifest['prepared_files']:
        raise ValueError('Prepared source differs from approved 29-file baseline')
    return manifest

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest',type=Path,help='Explicit source manifest; default: current if present, otherwise frozen baseline')
    parser.add_argument('--prepared',type=Path,help='Check an existing prepared sketch (does not generate it)')
    args=parser.parse_args()
    manifest=check_prepared(ROOT,args.prepared,args.manifest) if args.prepared else check_owned(ROOT,args.manifest)
    print(json.dumps({'version':manifest['version'],'owned_files':17,'prepared_files':29 if args.prepared else 'not checked','claim':'exact source only'}))
if __name__=='__main__':main()
