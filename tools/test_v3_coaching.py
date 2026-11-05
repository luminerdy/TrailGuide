#!/usr/bin/env python3
"""Focused actual LVGL coaching test; current-config verified objects; full host fallback if missing."""
from pathlib import Path
import argparse,subprocess
import v3_native_objects as native
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/v3-0.1.22'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--legacy',action='store_true',help='Also run existing Ui and turnover suites, writing only to this new output')
    args=parser.parse_args()
    out=OUT/'evidence';out.mkdir(parents=True,exist_ok=True)
    subprocess.run(['python3',str(ROOT/'tools/build_v3_coaching.py'),'--check'],check=True,cwd=ROOT)
    objects=native.current_objects()
    flags=['g++','-std=c++17','-Wall','-Wextra','-Werror',*native.flags()]
    names=['coaching','compact']+(['ui','turnover'] if args.legacy else [])
    for name in names:
        source=ROOT/('tests/v3_compact_ui_test.cpp' if name=='compact' else 'tests/v3_coaching_test.cpp')
        extra=[]
        if name not in ('coaching','compact'):
            # A source-only private copy redirects every legacy release path.
            # Never run an old fixture against its accepted artifact directory.
            text=(ROOT/f'tests/v3_app_{name}_test.cpp').read_text()
            import re
            text=re.sub(r'build/v3-0\.1\.\d+/', 'build/v3-0.1.22/evidence/legacy/',text)
            text=text.replace('../trailguide_v3/',str(ROOT/'trailguide_v3')+'/')
            source=out/f'legacy-{name}.cpp';source.write_text(text)
            for directory in ('frames','turnover-frames'):(out/'legacy'/directory).mkdir(parents=True,exist_ok=True)
            if name=='ui':extra=['-Wl,--wrap=lv_timer_create']
        binary=out/(name+'-test')
        print('COMPILE',name,'ASan/UBSan with 424 hash-verified read-only LVGL objects',flush=True)
        subprocess.run([*flags,*extra,str(source),*map(str,objects),'-lm','-o',str(binary)],check=True,cwd=ROOT)
        result=subprocess.run([str(binary)],cwd=ROOT,timeout=180)
        if result.returncode:return result.returncode
    print('PASS focused native coaching'+(' + classic Ui/turnover regression' if args.legacy else '')+'; no target build, hardware or network; current host cache rebuilt only if needed',flush=True)
    return 0
if __name__=='__main__':raise SystemExit(main())
