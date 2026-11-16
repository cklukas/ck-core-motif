#!/usr/bin/env python3
"""Run WP-47 Xrm semantic palette tokens and WCAG contrast guardrail tests under Xvfb."""
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
    parser.add_argument('--prefix', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--cc', default=os.environ.get('CC', 'cc'))
    args = parser.parse_args()
    if not args.prefix.is_absolute() or not args.out.is_absolute():
        parser.error('use absolute prefix and fresh out paths')
    output = args.out.resolve()
    if platform.system() == 'Darwin' and not any(
            base.parent.is_mount() and output.is_relative_to(base)
            for base in (Path('/Volumes/S8T/tmp'), Path('/Volumes/PRO-BLADE/tmp'))):
        parser.error('Mac results must be beneath a mounted project scratch volume')
    output.mkdir(parents=True, exist_ok=False)
    (output / 'tmp').mkdir()
    prefix = args.prefix.resolve(strict=True)
    root = Path(__file__).resolve().parents[2]
    source = root / 'tests/automated/check-semantic-palette-tokens.c'
    extension = '.9.dylib' if platform.system() == 'Darwin' else '.so'
    library = (prefix / 'lib' / ('libXm' + extension)).resolve(strict=True)
    env = dict(os.environ, TMPDIR=str(output / 'tmp'),
               XDG_CACHE_HOME=str(output / 'cache'),
               PYTHONDONTWRITEBYTECODE='1')
    variable = 'DYLD_LIBRARY_PATH' if platform.system() == 'Darwin' else 'LD_LIBRARY_PATH'
    env[variable] = str(prefix / 'lib')
    env['LC_ALL'] = 'en_US.UTF-8' if platform.system() == 'Darwin' else 'C.UTF-8'
    fingerprint = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
    receipt = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   scope='Isolated Xvfb test of WP-47 semantic palette tokens and WCAG contrast guardrails',
                   library=str(library), library_sha256=fingerprint(library),
                   source_sha256=fingerprint(source),
                   steps=[], success=False)
    began = time.monotonic()

    def run_cmd(name, command, run_env=None):
        result = subprocess.run(command, env=run_env or env, capture_output=True, text=True, timeout=60)
        (output / (name + '.log')).write_text(result.stdout + result.stderr)
        receipt['steps'].append(dict(name=name, command=[str(x) for x in command], exit_code=result.returncode))
        if result.returncode:
            raise RuntimeError(name + ': ' + result.stdout + result.stderr)
        if result.stdout:
            print(result.stdout.strip(), flush=True)

    try:
        binary = output / 'check-semantic-palette-tokens'
        flags = ['-std=c23', '-D_POSIX_C_SOURCE=200809L', '-Wall', '-Wextra', '-Werror',
                 '-I' + str(prefix / 'include'), '-L' + str(prefix / 'lib')]
        if platform.system() == 'Darwin':
            flags += ['-I/opt/X11/include', '-L/opt/X11/lib']
        else:
            flags += ['-Wl,-rpath,' + str(prefix / 'lib')]
        run_cmd('compile', shlex.split(args.cc) + flags + [str(source),
                '-lXm', '-lXt', '-lX11', '-lm', '-o', str(binary)])

        xvfb = shutil.which('Xvfb') or '/opt/X11/bin/Xvfb'
        for depth_label, screen_spec in (('24bit-truecolor', '1280x900x24'),
                                         ('8bit-pseudocolor', '1280x900x8')):
            server = None
            read_fd, write_fd = os.pipe()
            try:
                with (output / f'xvfb-{depth_label}.log').open('w') as log:
                    server = subprocess.Popen(
                        [xvfb, '-displayfd', str(write_fd), '-screen', '0',
                         screen_spec, '-noreset', '-nolisten', 'tcp'],
                        env=env, pass_fds=(write_fd,), stdout=log, stderr=subprocess.STDOUT)
            finally:
                os.close(write_fd)
            try:
                with os.fdopen(read_fd) as channel:
                    if not select.select([channel], [], [], 10)[0]:
                        raise RuntimeError(f'Xvfb ({depth_label}) did not publish display')
                    number = channel.readline().strip()
                    if not number.isdecimal():
                        raise RuntimeError(f'Xvfb ({depth_label}) returned invalid display')
                run_env = dict(env, DISPLAY=':' + number)
                run_cmd(f'run-{depth_label}', [str(binary)], run_env=run_env)
            finally:
                if server is not None:
                    server.terminate()
                    try:
                        server.wait(timeout=5)
                    except subprocess.TimeoutExpired:
                        server.kill()
                        server.wait()
        if fingerprint(library) != receipt['library_sha256']:
            raise RuntimeError('library changed during qualification')
        receipt['success'] = True
    finally:
        receipt.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                       elapsed_seconds=time.monotonic() - began)
        (output / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print('SKIP WP-47 semantic palette tokens: requires --prefix and --out')
    else:
        main()
