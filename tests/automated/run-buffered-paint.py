#!/usr/bin/env python3
"""Test native toolkit painting against an explicit configured build/display."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build', type=Path, required=True)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
build = args.build.resolve(strict=True)
output = args.out.resolve()
if output.exists():
    parser.error('--out must be a fresh directory')
if platform.system() == 'Darwin':
    if not any(p.parent.is_mount() and output.is_relative_to(p) for p in
               (Path('/Volumes/S8T/tmp'), Path('/Volumes/PRO-BLADE/tmp'))):
        parser.error('Mac test output must use a mounted project temporary volume')
if not os.environ.get('DISPLAY'):
    parser.error('set DISPLAY to the intended test server')
output.mkdir(parents=True)
(output / 'tmp').mkdir()
source = root / 'tests/automated/check-buffered-paint.c'
paint = root / 'lib/Xm/Paint.c'
lines = root / 'tests/automated/check-text-source-lines.c'
library = next((build / 'lib/Xm/.libs').glob('libXm.*.dylib'), None) if platform.system() == 'Darwin' else (build / 'lib/Xm/.libs/libXm.so').resolve()
assert library and library.is_file()
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
env = dict(os.environ, TMPDIR=str(output / 'tmp'), PYTHONDONTWRITEBYTECODE='1')
key = 'DYLD_LIBRARY_PATH' if platform.system() == 'Darwin' else 'LD_LIBRARY_PATH'
env[key] = str(library.parent) + os.pathsep + env.get(key, '')
flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-D_POSIX_C_SOURCE=200809L',
         '-I' + str(root / 'lib'), '-I' + str(root / 'lib/Xm'),
         '-I' + str(build / 'lib'), '-I' + str(build / 'lib/Xm'), '-I' + str(build / 'include')]
if platform.system() == 'Darwin':
    flags += ['-I/opt/X11/include', '-L/opt/X11/lib']
flags += ['-L' + str(library.parent)]
libs = ['-lXm', '-lXt', '-lX11', '-lXext']
receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
               display=env['DISPLAY'], library=str(library), library_sha256=sha(library),
               sources={str(p.relative_to(root)): sha(p) for p in (source, paint, lines)},
               steps=[], success=False)
start = time.monotonic()

def run(name, command):
    result = subprocess.run([str(x) for x in command], env=env, capture_output=True,
                            text=True, timeout=60)
    (output / (name + '.log')).write_text(result.stdout + result.stderr)
    receipt['steps'].append(dict(name=name, command=[str(x) for x in command], exit=result.returncode))
    if result.returncode:
        raise RuntimeError(name + ': ' + result.stdout + result.stderr)

try:
    binary = output / 'native'
    run('compile', [os.environ.get('CC', 'cc'), *flags, source, *libs, '-o', binary])
    for mode in ('auto', 'pixmap', 'direct'):
        run(mode, [binary, '-xrm', '*paintBuffer: ' + mode, *([] if mode == 'auto' else [mode])])
    for name, definitions in (
        ('no-headers', []),
        ('no-extension', ['-DHAVE_CONFIG_H', '-DTEST_NO_DBE_EXTENSION']),
        ('allocation-error', ['-DHAVE_CONFIG_H', '-DTEST_DBE_ALLOC_FAILURE']),
    ):
        binary = output / name
        run('compile-' + name, [os.environ.get('CC', 'cc'), *flags, *definitions,
                               source, paint, *libs, '-o', binary])
        run(name, [binary, 'pixmap'])
    binary = output / 'source-lines'
    run('compile-source-lines', [os.environ.get('CC', 'cc'), *flags, lines, *libs, '-o', binary])
    run('source-lines', [binary])
    receipt['success'] = True
    print('PASS native paint backends: DBE, pixmap, direct, no headers, no extension, allocation failure')
finally:
    receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   elapsed_seconds=time.monotonic() - start)
    (output / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
