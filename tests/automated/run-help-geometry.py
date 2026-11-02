#!/usr/bin/env python3
"""Compile and exercise the shared demo help geometry on an isolated X server."""
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
out.mkdir(parents=True, exist_ok=False)
(out / 'tmp').mkdir(exist_ok=True)
env = dict(os.environ, TMPDIR=str(out / 'tmp'),
           DYLD_LIBRARY_PATH=str(a.prefix / 'lib'),
           LD_LIBRARY_PATH=str(a.prefix / 'lib'))
library = a.prefix / 'lib/libXm.9.dylib'
receipt = {'library_sha256': hashlib.sha256(library.read_bytes()).hexdigest(),
           'target': 'help-geometry native help geometry regression',
           'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'sources': {}, 'commands': [], 'success': False,
           'baseline': 'No comparable standalone help-geometry fixture build duration recorded.'}
for path in [root / 'tests/automated/check-help-geometry.c',
             root / 'tests/automated/run-help-geometry.py',
             root / 'demos/lib/Xmd/Help.c', root / 'demos/lib/Xmd/HelpP.h', root / 'demos/lib/Xmd/Help.h', root / 'demos/programs/filemanager/filemanager.help']:
    receipt['sources'][str(path.relative_to(root))] = hashlib.sha256(path.read_bytes()).hexdigest()

def run(cmd):
    start = time.monotonic()
    result = subprocess.run(list(map(str, cmd)), cwd=out, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, timeout=60)
    receipt['commands'].append({'command': list(map(str, cmd)),
                                'seconds': time.monotonic() - start,
                                'exit': result.returncode, 'output': result.stdout})
    assert result.returncode == 0, result.stdout
    return result.stdout

server = None
try:
    run(['cc', '-std=gnu17', '-Wno-deprecated-non-prototype',
         '-ftrivial-auto-var-init=pattern', '-I' + str(a.prefix / 'include'),
         '-I/opt/X11/include', root / 'tests/automated/check-help-geometry.c', '-L' + str(a.prefix / 'lib'), '-L/opt/X11/lib',
         '-lXm', '-lXt', '-lX11', '-lXtst', '-o', out / 'check-help-geometry'])
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
    receipt['executable_sha256'] = hashlib.sha256((out / 'check-help-geometry').read_bytes()).hexdigest()
    for profile in ['classic', 'modern-light', 'modern-dark']:
        output = run([out / 'check-help-geometry', str(root / 'demos/programs/filemanager/%N.help'), '-xrm', 'ckCoreMotif.appearanceProfile: ' + profile])
        assert 'PASS:' in output, output
        print(profile + ': ' + output.strip())
    receipt['success'] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=10)
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
