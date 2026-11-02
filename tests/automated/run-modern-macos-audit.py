#!/usr/bin/env python3
"""Qualify modern native controls, colors and interaction on macOS."""
from pathlib import Path
import os, subprocess, json, hashlib, time, select
import argparse, sys, platform, datetime, shlex
parser = argparse.ArgumentParser(description='Run native modern appearance regressions on an isolated Xvfb display.')
parser.add_argument('--prefix', type=Path, required=True)
parser.add_argument('--build', type=Path, required=True)
parser.add_argument('--out', type=Path, required=True)
a = parser.parse_args()
prefix = a.prefix.resolve(strict=True)
build = a.build.resolve(strict=True)
o = a.out.resolve()
source = Path(__file__).resolve().parents[2]
if platform.system() != 'Darwin':
    parser.error('this runner qualifies native macOS/XQuartz libraries')
if o.exists():
    parser.error('--out must be fresh')
if not any((base.parent.is_mount() and o.is_relative_to(base) for base in [Path('/Volumes/S8T/tmp'), Path('/Volumes/PRO-BLADE/tmp')])):
    parser.error('use a mounted Mac project temporary volume')
o.mkdir(parents=True)
(o / 'tmp').mkdir()
e = dict(os.environ, TMPDIR=str(o / 'tmp'), DYLD_LIBRARY_PATH=str(prefix / 'lib'), LC_ALL='en_US.UTF-8', PATH='/opt/homebrew/bin:/opt/X11/bin:' + os.environ['PATH'])
rd, wr = os.pipe()
server = subprocess.Popen(['/opt/X11/bin/Xvfb', '-displayfd', str(wr), '-noreset', '-screen', '0', '1920x1440x24', '-listen', 'tcp', '-nolisten', 'unix', '-nolisten', 'local'], env=e, pass_fds=(wr,), stdout=(o / 'xvfb.log').open('w'), stderr=subprocess.STDOUT)
os.close(wr)
with os.fdopen(rd) as f:
    assert select.select([f], [], [], 10)[0]
    e['DISPLAY'] = '127.0.0.1:' + f.readline().strip()
names = ['appearance-visual-object-lifetime', 'live-appearance', 'live-appearance-gadget', 'display-scale-reentrant', 'display-scale-override', 'modern-audit-controls', 'modern-status-colors', 'xmstring-disjoint-clip', 'tab-color-state', 'scroll-lifetime', 'progress-bar', 'progress-ring', 'calendar', 'date-picker', 'time-picker', 'modern-fields', 'modern-buttons', 'modern-arrows', 'modern-toggles', 'modern-borders', 'modern-scrollbars', 'composite-appearance-fonts', 'cascade-appearance', 'container-appearance', 'avatar-modern', 'breadcrumb-modern', 'font-selector-modern', 'appearance-roles', 'modern-scale', 'modern-light-profile', 'modern-dark-profile']
r = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(), 'scope': 'native isolated Xvfb; no user desktop input', 'library_path': str(prefix / 'lib/libXm.9.dylib'), 'library': hashlib.sha256((prefix / 'lib/libXm.9.dylib').read_bytes()).hexdigest(), 'cases': []}
try:
    uid = o / 'appearance-roles.uid'
    p = subprocess.run([str(prefix / 'bin/uil'), '-o', str(uid), str(source / 'tests/automated/appearance-roles.uil')], env=e, capture_output=True, text=True)
    (o / 'uil.log').write_text(p.stdout + p.stderr)
    if p.returncode:
        raise RuntimeError('appearance roles UID compilation failed')
    for n in names:
        s = source / 'tests/automated' / ('check-' + n + '.c')
        b = o / ('check-' + n)
        start = time.monotonic()
        row = {'name': n, 'source_sha256': hashlib.sha256(s.read_bytes()).hexdigest()}
        cmd = ['clang', '-std=c23', '-O1', '-DUSE_XFT', '-I/opt/X11/include/freetype2', '-I' + str(build / 'include'), '-I' + str(build / 'lib/Xm'), '-I' + str(source / 'lib'), '-I' + str(prefix / 'include'), '-I/opt/X11/include', '-L' + str(prefix / 'lib'), '-L/opt/X11/lib', '-Wl,-rpath,' + str(prefix / 'lib'), str(s), *([str(build / 'demos/programs/ckmotif-gallery/gallery_examples.o')] if n in ['cascade-appearance', 'container-appearance'] else []), '-lMrm', '-lXm', '-lXt', '-lX11', '-lXft', '-lfontconfig', '-o', str(b)]
        with (o / (n + '.log')).open('w') as f:
            p = subprocess.run(cmd, env=e, stdout=f, stderr=subprocess.STDOUT)
            row['compile'] = p.returncode
            if not p.returncode:
                try:
                    arguments = [str(b), *(['-xrm', 'ModernLightTest*XmToggleButton.background: #fff4cc'] if n == 'modern-light-profile' else ['-xrm', '*ordinary.foreground: #00ff00'] if n == 'live-appearance' else [])]
                    profiles = ['light', 'dark', 'contrast'] if n == 'appearance-roles' else [None]
                    row['run'] = 0
                    for profile in profiles:
                        p = subprocess.run([*arguments, *([profile, str(uid)] if profile else [])], env=e, stdout=f, stderr=subprocess.STDOUT, timeout=180)
                        if p.returncode:
                            row['run'] = p.returncode
                            break
                except subprocess.TimeoutExpired:
                    row['run'] = 124
        row['seconds'] = time.monotonic() - start
        r['cases'].append(row)
        (o / 'receipt.json').write_text(json.dumps(r, indent=2))
        print(row, flush=True)
finally:
    server.terminate()
    server.wait()
    r['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    r['success'] = all((x.get('run') == 0 for x in r['cases'])) and len(r['cases']) == len(names)
    (o / 'receipt.json').write_text(json.dumps(r, indent=2))
if not r['success']:
    raise SystemExit(1)
if hashlib.sha256((prefix / 'lib/libXm.9.dylib').read_bytes()).hexdigest() != r['library']:
    raise RuntimeError('library changed during qualification')
print('PASS native modern macOS audit', flush=True)
