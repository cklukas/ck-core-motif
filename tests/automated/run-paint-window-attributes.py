#!/usr/bin/env python3
"""Buffered paint with unavailable window attributes must not request a back buffer.

    run-paint-window-attributes.py --build BUILD --out OUT

BUILD is a configured and built Motif tree; OUT is a fresh directory. DISPLAY
names the test server. The test replaces Xlib functions at link time through
ELF symbol lookup, so it runs on Linux only.
"""
import argparse
import os
import platform
from pathlib import Path
import shlex
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build', type=Path, required=True)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
if platform.system() != 'Linux':
    parser.error('the test interposes Xlib functions through ELF symbol lookup: run it on Linux')
if not os.environ.get('DISPLAY'):
    parser.error('set DISPLAY to the intended test server')
root = Path(__file__).resolve().parents[2]
build = args.build.resolve(strict=True)
out = args.out.resolve()
if out.exists():
    parser.error('--out must be a fresh directory')
out.mkdir(parents=True)
library = (build / 'lib/Xm/.libs/libXm.so').resolve()
if not library.is_file():
    parser.error(f'{library} not found: build the tree first')
source = root / 'tests/automated/check-paint-window-attributes.c'
exe = out / 'check-paint-window-attributes'
flags = ['-std=c11', '-Wall', '-Wextra', '-Werror', '-D_POSIX_C_SOURCE=200809L',
         '-I' + str(root / 'lib'), '-I' + str(root / 'lib/Xm'),
         '-I' + str(build / 'lib'), '-I' + str(build / 'lib/Xm'),
         '-I' + str(build / 'include')]
if '#define USE_XFT 1' in (build / 'include/config.h').read_text():
    flags += shlex.split(subprocess.check_output(['pkg-config', '--cflags', 'xft'], text=True))
flags += ['-L' + str(library.parent)]
libs = ['-lXm', '-lXt', '-lX11', '-lXext', '-ldl']
subprocess.run(['cc', *flags, str(source), '-o', str(exe), *libs], check=True)
env = dict(os.environ, TMPDIR=str(out),
           LD_LIBRARY_PATH=str(library.parent) + os.pathsep + os.environ.get('LD_LIBRARY_PATH', ''))
result = subprocess.run([str(exe)], env=env, cwd=out, stdout=subprocess.PIPE,
                        stderr=subprocess.STDOUT, text=True, check=False)
(out / 'run.log').write_text(result.stdout)
print(result.stdout, end='')
sys.exit(result.returncode)
