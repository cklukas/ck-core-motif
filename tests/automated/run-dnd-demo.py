#!/usr/bin/env python3
"""Build DNDDemo and exercise native pointer transfers on an isolated X server."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import select
import subprocess
import time

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--prefix', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
root = Path(__file__).resolve().parents[2]
out = a.out.resolve()
if os.uname().sysname == 'Darwin':
    allowed = [Path('/Volumes/S8T'), Path('/Volumes/PRO-BLADE')]
    assert any(v.is_mount() and out.is_relative_to(v / 'tmp') for v in allowed), 'Use a mounted project temporary volume'
out.mkdir(parents=True, exist_ok=False)
(out / 'tmp').mkdir(exist_ok=True)
env = dict(os.environ, TMPDIR=str(out / 'tmp'),
           DYLD_LIBRARY_PATH=str(a.prefix / 'lib'),
           LD_LIBRARY_PATH=str(a.prefix / 'lib'))
receipt = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'sources': {}, 'commands': [], 'success': False,
           'baseline': 'No comparable focused DNDDemo fixture build duration recorded.'}
for path in [root / 'tests/automated/check-dnd-demo.c',
             root / 'lib/Xm/Transfer.c',
             *sorted((root / 'demos/programs/drag_and_drop').glob('DND*.[ch]'))]:
    receipt['sources'][str(path.relative_to(root))] = hashlib.sha256(path.read_bytes()).hexdigest()

def run(cmd):
    start = time.monotonic()
    began = datetime.datetime.now(datetime.timezone.utc).isoformat()
    result = subprocess.run(list(map(str, cmd)), cwd=out, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, timeout=60)
    receipt['commands'].append({'command': list(map(str, cmd)),
                                'seconds': time.monotonic() - start,
                                'start': began,
                                'end': datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                'exit': result.returncode, 'output': result.stdout})
    assert result.returncode == 0, result.stdout
    return result.stdout

receipt['runtime_library'] = {'path': str(a.prefix / 'lib/libXm.9.dylib'),
    'sha256': hashlib.sha256((a.prefix / 'lib/libXm.9.dylib').read_bytes()).hexdigest()} if os.uname().sysname == 'Darwin' else {}
server = None
try:
    run(['cc', '-std=gnu17', '-Wno-deprecated-non-prototype',
         '-I' + str(a.prefix / 'include'), '-I' + str(root / 'demos/lib'),
         '-I/opt/X11/include', root / 'tests/automated/check-dnd-demo.c',
         root / 'demos/programs/drag_and_drop/DNDDraw.c', '-L' + str(a.prefix / 'lib'), '-L/opt/X11/lib',
         '-lXm', '-lXt', '-lX11', '-lXtst', '-o', out / 'check-dnd-demo'])
    receipt['executable_sha256'] = hashlib.sha256((out / 'check-dnd-demo').read_bytes()).hexdigest()
    rd, wr = os.pipe()
    with (out / 'xvfb.log').open('w') as log:
        server = subprocess.Popen(['Xvfb', '-displayfd', str(wr), '-screen', '0',
                                   '1024x768x24', '-listen', 'tcp', '-nolisten',
                                   'unix', '-nolisten', 'local'], env=env,
                                  pass_fds=(wr,), stdout=log, stderr=log)
    os.close(wr)
    with os.fdopen(rd) as pipe:
        assert select.select([pipe], [], [], 10)[0], 'Xvfb did not start'
        env['DISPLAY'] = '127.0.0.1:' + pipe.readline().strip()
    for profile in ('classic', 'modern-light', 'modern-dark'):
        output = run([out / 'check-dnd-demo', '-xrm',
                      'ckCoreMotif.appearanceProfile: ' + profile])
        assert 'PASS:' in output, output
        print(profile + ': ' + output.strip().splitlines()[-1])
    receipt['success'] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=10)
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
