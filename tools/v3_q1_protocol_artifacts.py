"""Producer-bound synthetic recorder artifacts for the release integration tier.

No capture files are accepted or published. The canonical runner calls produce;
only successful fresh compilation and execution can seal the output manifest.
"""
from pathlib import Path
import hashlib
import json
import re
import v3_native_objects as native

INPUTS=('tests/v3_app_recorder_test.cpp','trailguide_v3/flight_recorder.h',
        'trailguide_v3/lv_conf.h','tools/test_v3_app.py',
        'tools/v3_native_objects.py','tools/v3_q1_protocol_artifacts.py')
OUTPUTS=('recorder.synthetic.wire','recorder.manual.synthetic.wire')
MANIFEST='recorder.synthetic.manifest.json'

def output(root): return root/'build/v3-0.1.28'
def binary(root): return output(root)/'host/v3-app-recorder-test'
def digest(path): return hashlib.sha256(path.read_bytes()).hexdigest()

def producer_inputs(root):
    pending: list[str]=list(INPUTS); result={}
    while pending:
        name=pending.pop();path=(root/name).resolve()
        if not path.is_relative_to(root.resolve()): raise ValueError('producer input outside source')
        name=str(path.relative_to(root.resolve()))
        if name in result:continue
        result[name]=digest(path)
        if path.suffix in ('.cpp','.h'):
            for include in re.findall(r'^\s*#include\s+"([^"]+)"',path.read_text(),re.M):
                child=(path.parent/include).resolve()
                pending.append(str(child.relative_to(root.resolve())))
    return result

def commands(root):
    flags=[s.replace(str(native.ROOT),str(root)) for s in native.flags()]
    compile_command=['g++','-std=c++17','-Wall','-Wextra','-Werror',*flags,
                     str(root/'tests/v3_app_recorder_test.cpp'),'-o',str(binary(root))]
    return compile_command,[str(binary(root))]

def configuration(root):
    return [[s.replace(str(root),'${SOURCE}') for s in command] for command in commands(root)]

def produce(root,run):
    directory=output(root);directory.mkdir(parents=True,exist_ok=True)
    binary(root).parent.mkdir(parents=True,exist_ok=True)
    # Invalidate before compiling/running; a failed writer cannot bless old bytes.
    for name in (*OUTPUTS,MANIFEST):(directory/name).unlink(missing_ok=True)
    before=producer_inputs(root)
    compile_command,writer_command=commands(root)
    run(compile_command,cwd=root,check=True)
    run(writer_command,cwd=root,check=True)
    if before!=producer_inputs(root):raise ValueError('producer inputs changed during execution')
    payload={'schema':1,'synthetic_only':True,'producer_inputs':before,
             'configuration':configuration(root),'binary_sha256':digest(binary(root)),
             'outputs':{name:digest(directory/name) for name in OUTPUTS}}
    temp=directory/(MANIFEST+'.tmp');temp.write_text(json.dumps(payload,indent=2)+'\n');temp.replace(directory/MANIFEST)
    return validate(root)

def validate(root):
    try:
        manifest=json.loads((output(root)/MANIFEST).read_text())
        if manifest.get('schema')!=1 or manifest.get('synthetic_only') is not True:
            raise ValueError('invalid synthetic producer manifest')
        if manifest.get('producer_inputs')!=producer_inputs(root) or manifest.get('configuration')!=configuration(root):
            raise ValueError('stale producer source/configuration')
        if not re.fullmatch('[0-9a-f]{64}',manifest.get('binary_sha256','')):
            raise ValueError('invalid producer binary digest')
        if manifest.get('outputs')!={name:digest(output(root)/name) for name in OUTPUTS}:
            raise ValueError('stale synthetic output digest')
        return manifest
    except (OSError,KeyError,TypeError,json.JSONDecodeError) as exc:
        raise ValueError('missing/invalid producer-bound synthetic artifacts: '+str(exc)) from exc
