#!/usr/bin/env python3
"""Build native installed-library transfer/image regressions on isolated Xvfb."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import signal
import subprocess
import time

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('prefix', type=Path)
p.add_argument('results', type=Path)
p.add_argument('--cc', choices=('gcc', 'clang'), default='gcc')
p.add_argument('--sanitize', action='store_true')
p.add_argument('--xt-interposition', action='store_true',
               help='Sanitizer ODR level 1 for same-size Motif/Xt VendorShell globals')
p.add_argument('--only', choices=('clipboard', 'merge', 'bounds', 'lifetime', 'xpm', 'buttons', 'xt-callbacks'))
a = p.parse_args()
if platform.system() != 'Linux' or not a.prefix.is_absolute() or not a.results.is_absolute():
    p.error('use absolute paths in a Linux task workspace')
if a.xt_interposition and not a.sanitize:
    p.error('--xt-interposition requires --sanitize')
a.results.mkdir(parents=True, exist_ok=False)
for name in ('tmp', 'cache', 'config', 'data'):
    (a.results / name).mkdir()
env = dict(os.environ, TMPDIR=str(a.results / 'tmp'),
           XDG_CACHE_HOME=str(a.results / 'cache'),
           XDG_CONFIG_HOME=str(a.results / 'config'), XDG_DATA_HOME=str(a.results / 'data'),
           LD_LIBRARY_PATH=str(a.prefix / 'lib'), LC_ALL='C.UTF-8')
if a.sanitize:
    env.update(ASAN_OPTIONS='detect_leaks=1:halt_on_error=1',
               UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    if a.xt_interposition:
        env['ASAN_OPTIONS'] += ':detect_odr_violation=1'
source = Path(__file__).resolve().parent
record = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
              prefix=str(a.prefix), compiler=a.cc, architecture=platform.machine(),
              sanitize=a.sanitize, preload=env.get('LD_PRELOAD'),
              runtime_options={k: env[k] for k in ('ASAN_OPTIONS', 'UBSAN_OPTIONS') if k in env},
              runner_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              compiler_version=subprocess.check_output([a.cc, '--version'], text=True).strip(),
              steps=[], sources={}, libraries={})
begin = time.monotonic()

def run(name, command):
    start = time.monotonic()
    proc = subprocess.Popen(list(map(str, command)), env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, start_new_session=True)
    try:
        output, _ = proc.communicate(timeout=90)
        code = proc.returncode
    except subprocess.TimeoutExpired:
        os.killpg(proc.pid, signal.SIGTERM)
        try:
            output, _ = proc.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL)
            output, _ = proc.communicate()
        output += b'\nTIMEOUT: fixture process group terminated\n'
        code = 124
    (a.results / (name + '.log')).write_bytes(output)
    record['steps'].append(dict(name=name, command=list(map(str, command)), exit=code,
                               seconds=round(time.monotonic() - start, 3)))
    print(name, code, flush=True)
    return code == 0

ok = True
try:
    for library in sorted((a.prefix / 'lib').glob('*.so.*')):
        if not library.is_symlink():
            record['libraries'][str(library)] = hashlib.sha256(library.read_bytes()).hexdigest()
    flags = ['-D_GNU_SOURCE', '-std=c11', '-Wall', '-Wextra', '-Werror', '-g', '-O2',
             '-I' + str(a.prefix / 'include'), '-L' + str(a.prefix / 'lib'),
             '-Wl,-rpath,' + str(a.prefix / 'lib')]
    if a.sanitize:
        flags += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    for name, modes in [('transfer-storage', ('clipboard', 'merge', 'bounds', 'lifetime')),
                        ('xpm-input', ('xpm',)), ('button-timers', ('buttons',)),
                        ('xt-callback-lifetime', ('xt-callbacks',))]:
        modes = [mode for mode in modes if not a.only or mode == a.only]
        if not modes:
            continue
        src = source / ('check-' + name + '.c')
        record['sources'][src.name] = hashlib.sha256(src.read_bytes()).hexdigest()
        executable = a.results / name
        if not run('compile-' + name, [a.cc, *flags, src, '-lXm', '-lXt', '-lX11', '-ldl', '-o', executable]):
            ok = False
            continue
        if not run('link-' + name, ['ldd', executable]):
            ok = False
            continue
        if str(a.prefix / 'lib/libXm.so.') not in (a.results / ('link-' + name + '.log')).read_text():
            raise RuntimeError('loaded a different libXm prefix')
        if (a.prefix / 'lib/libXt.so').exists() and str(a.prefix / 'lib/libXt.so.') not in (a.results / ('link-' + name + '.log')).read_text():
            raise RuntimeError('loaded a different libXt prefix')
        for mode in modes:
            arguments = [a.results / 'data'] if mode == 'xpm' else [] if mode in ('buttons', 'scroll', 'xt-callbacks') else [mode]
            if not run(mode, ['xvfb-run', '-a', executable, *arguments]):
                ok = False
    for name, expected in record['libraries'].items():
        if hashlib.sha256(Path(name).read_bytes()).hexdigest() != expected:
            raise RuntimeError('installed library changed during regression: ' + name)
    record['success'] = ok
finally:
    record['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    record['seconds'] = round(time.monotonic() - begin, 3)
    (a.results / 'receipt.json').write_text(json.dumps(record, indent=2) + '\n')
raise SystemExit(0 if ok else 1)
