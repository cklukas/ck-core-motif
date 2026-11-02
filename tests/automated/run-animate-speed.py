#!/usr/bin/env python3
"""Compile both animation UI definitions and test the native demo callbacks."""
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
p.add_argument('--xmd-library', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
a.prefix = a.prefix.resolve()
a.xmd_library = a.xmd_library.resolve()
root = Path(__file__).resolve().parents[2]
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
(out / 'tmp').mkdir(exist_ok=True)
env = dict(os.environ, TMPDIR=str(out / 'tmp'),
           DYLD_LIBRARY_PATH=str(a.prefix / 'lib'),
           LD_LIBRARY_PATH=str(a.prefix / 'lib'))
receipt = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'sources': {}, 'commands': [], 'success': False,
           'baseline': 'No comparable focused UID/fixture build duration recorded.'}
for path in [root / 'tests/automated/check-animate-speed.c',
             *sorted((root / 'demos/programs/animate').glob('*.[cu]*'))]:
    receipt['sources'][str(path.relative_to(root))] = hashlib.sha256(path.read_bytes()).hexdigest()

def run(cmd, expected_exit=0):
    start = time.monotonic()
    result = subprocess.run(list(map(str, cmd)), cwd=out, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, timeout=60)
    receipt['commands'].append({'command': list(map(str, cmd)),
                                'seconds': time.monotonic() - start,
                                'exit': result.returncode, 'output': result.stdout})
    assert result.returncode == expected_exit, result.stdout
    return result.stdout

server = None
try:
    compile_command = ['cc', '-std=c23', '-Wall', '-Wextra', '-Wno-unused-parameter',
         '-I' + str(a.prefix / 'include'), '-I' + str(root / 'demos/lib'),
         '-I/opt/X11/include', root / 'tests/automated/check-animate-speed.c',
         a.xmd_library, '-L' + str(a.prefix / 'lib'), '-L/opt/X11/lib',
         '-lMrm', '-lXm', '-lXt', '-lX11', '-o', out / 'check-animate-speed']
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
    demo = root / 'demos/programs/animate'
    run([a.prefix / 'bin/uil', '-o', out / 'dog.uid', demo / 'dog.uil'])
    for name in ('animate', 'xmanimate'):
        run(compile_command + (['-DTEST_LEGACY_ANIMATE'] if name == 'animate' else []))
        run([a.prefix / 'bin/uil', '-o', out / (name + '.uid'), demo / (name + '.uil')])
        output = run([out / 'check-animate-speed'])
        assert 'PASS:' in output, output
        print(name + ': ' + output.strip())
        for args in (['-speed'], *(['-speed', value] for value in
                     ('0', '-1', '101', 'abc', '1.5', '9999999999999999999999'))):
            output = run([out / 'check-animate-speed', *args], expected_exit=1)
            assert '-speed requires an integer from 1 to 100' in output
    receipt['success'] = True
finally:
    if server:
        server.terminate()
        server.wait(timeout=10)
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
