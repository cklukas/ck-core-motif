#!/usr/bin/env python3
"""Test the real gallery's Cascade appearance on an isolated Xvfb display."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import select
import shlex
import shutil
import subprocess
import sys
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--results', type=Path, required=True)
    parser.add_argument('--cc', default='cc')
    args = parser.parse_args()
    if not args.build.is_absolute() or not args.results.is_absolute():
        parser.error('use absolute build and fresh results paths')
    output = args.results.resolve()
    if platform.system() == 'Darwin' and not any(
            base.parent.is_mount() and output.is_relative_to(base)
            for base in (Path('/Volumes/S8T/tmp'), Path('/Volumes/PRO-BLADE/tmp'))):
        parser.error('Mac results must be beneath a mounted project scratch volume')
    output.mkdir(parents=True)
    (output / 'tmp').mkdir()
    build = args.build.resolve(strict=True)
    root = Path(__file__).resolve().parents[2]
    source = root / 'tests/automated/check-cascade-appearance.c'
    example = build / 'demos/programs/ckmotif-gallery/gallery_examples.o'
    extension = '.dylib' if platform.system() == 'Darwin' else '.so'
    library = (build / 'lib/Xm/.libs' / ('libXm' + extension)).resolve(strict=True)
    env = dict(os.environ, TMPDIR=str(output / 'tmp'), XDG_CACHE_HOME=str(output / 'cache'),
               PYTHONDONTWRITEBYTECODE='1')
    variable = 'DYLD_LIBRARY_PATH' if platform.system() == 'Darwin' else 'LD_LIBRARY_PATH'
    env[variable] = str(library.parent)
    env['LC_ALL'] = 'en_US.UTF-8' if platform.system() == 'Darwin' else 'C.UTF-8'
    fingerprint = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
    receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   scope='Isolated native gallery-factory test; no user desktop input',
                   library=str(library), library_sha256=fingerprint(library),
                   source_sha256=fingerprint(source), examples_object_sha256=fingerprint(example),
                   steps=[], success=False)
    server = None
    began = time.monotonic()

    def run(name, command):
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=60)
        (output / (name + '.log')).write_text(result.stdout + result.stderr)
        receipt['steps'].append(dict(name=name, command=command, exit_code=result.returncode))
        if result.returncode:
            raise RuntimeError(name + ': ' + result.stdout + result.stderr)
        if result.stdout:
            print(result.stdout.strip(), flush=True)

    try:
        binary = output / 'cascade-appearance'
        flags = ['-std=c23', '-Wall', '-Wextra', '-Werror',
                 '-I' + str(build / 'lib'), '-I' + str(root / 'lib'),
                 '-L' + str(library.parent)]
        if platform.system() == 'Darwin':
            flags += ['-I/opt/X11/include', '-L/opt/X11/lib']
        run('compile', shlex.split(args.cc) + flags + [str(source), str(example),
            '-lXm', '-lXt', '-lX11', '-o', str(binary)])
        xvfb = shutil.which('Xvfb') or '/opt/X11/bin/Xvfb'
        transport = ['-listen', 'tcp', '-nolisten', 'unix', '-nolisten', 'local'] if platform.system() == 'Darwin' else ['-nolisten', 'tcp']
        read_fd, write_fd = os.pipe()
        try:
            with (output / 'xvfb.log').open('w') as log:
                server = subprocess.Popen([xvfb, '-displayfd', str(write_fd), '-screen', '0',
                    '1600x1200x24', '-noreset', *transport], env=env, pass_fds=(write_fd,),
                    stdout=log, stderr=subprocess.STDOUT)
        finally:
            os.close(write_fd)
        with os.fdopen(read_fd) as channel:
            if not select.select([channel], [], [], 10)[0]:
                raise RuntimeError('Xvfb did not publish its isolated display')
            number = channel.readline().strip()
            if not number.isdecimal():
                raise RuntimeError('Xvfb returned an invalid display number')
            env['DISPLAY'] = ('127.0.0.1:' if platform.system() == 'Darwin' else ':') + number
        run('run', [str(binary)])
        if fingerprint(library) != receipt['library_sha256']:
            raise RuntimeError('library changed during qualification')
        receipt['success'] = True
    finally:
        if server is not None:
            server.terminate()
            try:
                server.wait(timeout=5)
            except subprocess.TimeoutExpired:
                server.kill()
                server.wait()
        receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                       elapsed_seconds=time.monotonic()-began)
        (output / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('SKIP Cascade appearance: requires explicit build and results paths')
    else:
        main()
