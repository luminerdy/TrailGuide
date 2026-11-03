"""Current-config native LVGL cache shared by full and focused runners."""
from pathlib import Path
import hashlib
import json
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/v3-0.1.14'

def flags():
    return ['-O0', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
            '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_CONF_PATH='+str(ROOT/'trailguide_v3/lv_conf.h'),
            '-I'+str(ROOT/'trailguide_v3'), '-I'+str(OUT/'libraries/lvgl')]

def config_key():
    return hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()+repr(flags()).encode()).hexdigest()[:12]

def object_path(source):
    return OUT/'host'/(config_key()+'-'+hashlib.sha256(str(source).encode()+source.read_bytes()).hexdigest()[:16]+'.o')

def seal(objects):
    payload={'flags':flags(), 'config_sha256':hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(),
             'objects':{str(Path(p).relative_to(OUT)):hashlib.sha256(Path(p).read_bytes()).hexdigest() for p in objects}}
    (OUT/'host/native-objects.json').write_text(json.dumps(payload,indent=2)+'\n')

def verified_objects():
    manifest=json.loads((OUT/'preparation.json').read_text())
    from prepare_v3 import ARCHIVE_SHA256
    assert manifest['archive_sha256']==ARCHIVE_SHA256, 'Unpinned LVGL archive'
    library=OUT/'libraries/lvgl'
    for relative,digest in manifest['library_files'].items():
        assert hashlib.sha256((library/relative).read_bytes()).hexdigest()==digest, relative
    sources=sorted((library/'src').rglob('*.c'))
    assert len(sources)==424, 'Expected complete pinned LVGL source set'
    objects=[object_path(p) for p in sources]
    pin=json.loads((OUT/'host/native-objects.json').read_text())
    assert pin['flags']==flags(), 'Native compiler flags changed'
    assert pin['config_sha256']==hashlib.sha256((ROOT/'trailguide_v3/lv_conf.h').read_bytes()).hexdigest(), 'Native font/config changed'
    assert set(pin['objects'])=={str(p.relative_to(OUT)) for p in objects}, 'Native object/source set changed'
    for p in objects:
        assert hashlib.sha256(p.read_bytes()).hexdigest()==pin['objects'][str(p.relative_to(OUT))], p
    return list(map(str,objects))

def current_objects():
    try:
        return verified_objects()
    except (OSError,KeyError,ValueError,AssertionError):
        # Full runner never calls this function: no recursive build invocation.
        # No accepted-library/object tree is used with a different font config.
        subprocess.run([sys.executable,str(ROOT/'tools/test_v3_app.py')],cwd=ROOT,check=True)
        return verified_objects()
