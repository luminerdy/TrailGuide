#!/usr/bin/env python3
"""Focused actual LVGL coaching test; immutable verified objects, no preparer."""
from pathlib import Path
import argparse,hashlib,json,subprocess
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/v3-0.1.12'
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--legacy',action='store_true',help='Also run existing Ui and turnover suites, writing only to this new output')
    args=parser.parse_args()
    out=OUT/'evidence';out.mkdir(parents=True,exist_ok=True)
    subprocess.run(['python3',str(ROOT/'tools/build_v3_coaching.py'),'--check'],check=True,cwd=ROOT)
    base=ROOT/'build/v3-0.1.10'
    inv=json.loads((ROOT/'build/v3-motion-investigation/installed-v3-0.1.10-files.private.json').read_text())
    def verify(p,key):
        assert hashlib.sha256(p.read_bytes()).hexdigest()==inv[key],p
    verify(ROOT/'trailguide_v3/lv_conf.h','prepared/trailguide_v3/lv_conf.h')
    objects=sorted((base/'host').glob('*.o'));assert len(objects)==424
    for p in objects:verify(p,str(p.relative_to(base)))
    flags=['g++','-std=c++17','-Wall','-Wextra','-Werror','-O0','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DLV_CONF_INCLUDE_SIMPLE','-DLV_CONF_PATH='+str(ROOT/'trailguide_v3/lv_conf.h'),'-I'+str(base/'libraries/lvgl'),'-I'+str(base/'prepared/trailguide_v3')]
    names=['coaching']+(['ui','turnover'] if args.legacy else [])
    for name in names:
        source=ROOT/'tests/v3_coaching_test.cpp'
        extra=[]
        if name!='coaching':
            # A source-only private copy redirects every legacy release path.
            # Never run an old fixture against its accepted artifact directory.
            text=(ROOT/f'tests/v3_app_{name}_test.cpp').read_text()
            import re
            text=re.sub(r'build/v3-0\.1\.\d+/', 'build/v3-0.1.12/evidence/legacy/',text)
            text=text.replace('../trailguide_v3/',str(ROOT/'trailguide_v3')+'/')
            source=out/f'legacy-{name}.cpp';source.write_text(text)
            for directory in ('frames','turnover-frames'):(out/'legacy'/directory).mkdir(parents=True,exist_ok=True)
            if name=='ui':extra=['-Wl,--wrap=lv_timer_create']
        binary=out/(name+'-test')
        print('COMPILE',name,'ASan/UBSan with 424 hash-verified read-only LVGL objects',flush=True)
        subprocess.run([*flags,*extra,str(source),*map(str,objects),'-lm','-o',str(binary)],check=True,cwd=ROOT)
        result=subprocess.run([str(binary)],cwd=ROOT,timeout=180)
        if result.returncode:return result.returncode
    print('PASS focused native coaching'+(' + classic Ui/turnover regression' if args.legacy else '')+'; no preparer, target build, hardware or network',flush=True)
    return 0
if __name__=='__main__':raise SystemExit(main())
