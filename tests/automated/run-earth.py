#!/usr/bin/env python3
"""Compile and exercise the earth demo with native Xt timer and rendering checks."""
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
out.mkdir(parents=True, exist_ok=True)
(out / 'tmp').mkdir(exist_ok=True)
env = dict(os.environ, TMPDIR=str(out / 'tmp'),
           DYLD_LIBRARY_PATH=str(a.prefix / 'lib'),
           LD_LIBRARY_PATH=str(a.prefix / 'lib'))
receipt = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'sources': {}, 'commands': [], 'success': False,
           'baseline': 'Focused Earth native fixture compilation previously took about 0.2 seconds; no separate optimized demo baseline.'}
for path in [root / 'tests/automated/check-earth.c',
             root / 'demos/programs/earth/earth.c',
             root / 'demos/programs/earth/terre.xbm',
             root / 'demos/programs/earth/globe.h',
             root / 'demos/programs/earth/world-map.xpm']:
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
    run(['cc', '-std=gnu17', '-Wno-deprecated-non-prototype', '-O2',
         '-I' + str(a.prefix / 'include'), '-I' + str(root / 'demos/lib'),
         '-I/opt/X11/include',
         '-DEARTH_MAP_FILE="' + str(a.prefix / 'share/Xm/earth/world-map.xpm') + '"',
         '-DEARTH_SOURCE_MAP_FILE="' + str(root / 'demos/programs/earth/world-map.xpm') + '"', root / 'tests/automated/check-earth.c', '-L' + str(a.prefix / 'lib'), '-L/opt/X11/lib',
         '-lXm', '-lXt', '-lX11', '-lXext', '-lXtst', '-lm', '-o', out / 'check-earth'])
    run(['cc', '-std=gnu17', '-Wno-deprecated-non-prototype', '-O2',
         '-I' + str(a.prefix / 'include'), '-I/opt/X11/include',
         '-DEARTH_MAP_FILE="' + str(a.prefix / 'share/Xm/earth/world-map.xpm') + '"',
         '-DEARTH_SOURCE_MAP_FILE="' + str(root / 'demos/programs/earth/world-map.xpm') + '"',
         root / 'demos/programs/earth/earth.c', '-L' + str(a.prefix / 'lib'),
         '-L/opt/X11/lib', '-lXm', '-lXt', '-lX11', '-lm', '-o', out / 'earth'])
    receipt['executable_sha256'] = hashlib.sha256((out / 'earth').read_bytes()).hexdigest()
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
    for profile in ['classic', 'modern-light', 'modern-dark']:
        output = run([out / 'check-earth', '-xrm', 'ckCoreMotif.appearanceProfile: ' + profile])
        assert 'PASS:' in output, output
        print(profile + ': ' + output.strip())
        if (out / 'controls.ppm').exists():
            (out / 'controls.ppm').rename(out / (profile + '-controls.ppm'))
        if (out / 'globe.ppm').exists():
            (out / 'globe.ppm').rename(out / (profile + '.ppm'))
    for options in [['-speed', '0'], ['-fps', '60'],
                    ['-speed', '2147483647', '-fps', '999'],
                    ['-speed', '-2147483648', '-fps', '0']]:
        output = run([out / 'check-earth', '-xrm',
                      'ckCoreMotif.appearanceProfile: modern-light', *options])
        assert 'PASS:' in output, output
    receipt['success'] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=10)
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
