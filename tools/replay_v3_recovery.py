#!/usr/bin/env python3
"""Replay a local private diagnostic window through the actual host Ui binary.

No network/device I/O. Raw input/derived vectors stay in ignored build output;
stdout reports counts only. Requires tools/test_v3_app.py to have built host tests.
"""
import argparse
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/v3-0.1.12/evidence'

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('capture', type=Path)
    p.add_argument('--start', type=int, required=True)
    p.add_argument('--end', type=int, required=True)
    p.add_argument('--expected-rows', type=int, required=True)
    a = p.parse_args()
    rows = []
    for line in a.capture.read_text().splitlines():
        if not line.startswith('TG_MOTION1 '):
            continue
        values = dict(re.findall(r'(\w+)=([^ ]+)', line))
        if 't' not in values:  # Session-end summaries are not sample rows.
            continue
        t = int(values['t'])
        if a.start <= t <= a.end:
            assert values['gates'] == '0,0,0,0,1'
            assert values['valid'] == '1' and values['rest'] == '1'
            assert values['phase'] == '4>4' and values['trigger'] == '0'
            rows.append(values)
    assert len(rows) == a.expected_rows, (len(rows), a.expected_rows)
    assert len({int(row['at']) for row in rows}) == len(rows)
    assert all(int(b['at']) > int(a['at']) for a, b in zip(rows, rows[1:]))
    origins = {row['origin_e6'] for row in rows}
    assert len(origins) == 1
    prefix, origin = origins.pop().split(':')
    assert prefix == '1'
    def scaled(value, scale):
        return ' '.join(format(int(x)/scale, '.9g') for x in value.split(','))
    output = [scaled(origin, 1e6)+' '+str(len(rows))]
    for row in rows:
        output.append(' '.join([row['t'], row['at'], row['valid'],
                               scaled(row['acc_g_e6'], 1e6),
                               scaled(row['gyro_dps_e3'], 1e3)]))
    OUT.mkdir(parents=True, exist_ok=True)
    data = OUT/'recovery-capture.private.csv'
    data.write_text('\n'.join(output)+'\n')
    binary = ROOT/'build/v3-0.1.12/host/v3-app-recovery-test'
    result = subprocess.run([str(binary), str(data)], capture_output=True, text=True)
    (OUT/'recovery-replay.private.log').write_text(result.stdout+result.stderr)
    print(result.stdout, end='')
    if result.returncode:
        print(result.stderr, end='')
    (OUT/'recovery-replay.private.json').write_text(json.dumps({
        'rows': len(rows), 'exit': result.returncode,
        'scope': 'actual Ui/sample/Gesture; seeded retained origin, forced Recover; recorded held window only; subsequent cycles synthetic; not physical acceptance'
    }, indent=2)+'\n')
    return result.returncode

if __name__ == '__main__':
    raise SystemExit(main())
