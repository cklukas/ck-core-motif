#!/usr/bin/env python3
"""Qualify installed demo assets with this task's build-tree demos unavailable.

Use only an idle, task-owned build: its demos directory is temporarily renamed.
"""
import argparse
import hashlib
import json
import os
import signal
import re
from pathlib import Path
import subprocess
import sys
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-root', required=True, type=Path)
parser.add_argument('--prefix', required=True, type=Path)
parser.add_argument('--out', required=True, type=Path)
a = parser.parse_args()
if sys.platform == 'darwin':
    parser.error('This native Xvfb qualification runs on Linux')
build, prefix, out = a.build_root.resolve(), a.prefix.resolve(), a.out.resolve()
if out.exists(): parser.error('--out must be fresh')
out.mkdir(parents=True)
for name in ('tmp', 'home'): (out/name).mkdir()
env = dict(os.environ, TMPDIR=str(out/'tmp'), HOME=str(out/'home'),
           LD_LIBRARY_PATH=str(prefix/'lib'))
programs = build/'demos/programs'
hidden = programs.with_name('programs.install-test-hidden')
if hidden.exists(): parser.error('build demos already hidden')
gallery = prefix/'bin/ckmotif-gallery'
processes = []
receipt = {'gallery_sha256': hashlib.sha256(gallery.read_bytes()).hexdigest(), 'results': {}, 'identical_build_outputs': {}}
for built, installed in (
    (build/'clients/mwm/.libs/mwm', prefix/'bin/mwm'),
    (build/'lib/Xm/.libs/libXm.so.9.0.0', prefix/'lib/libXm.so.9.0.0'),
    (build/'demos/programs/workspace/.libs/wsm', prefix/'share/Xm/wsm/wsm'),
    (build/'demos/programs/ckmotif-gallery/.libs/ckmotif-gallery', gallery),
):
    digest=hashlib.sha256(installed.read_bytes()).hexdigest()
    if hashlib.sha256(built.read_bytes()).hexdigest()!=digest:
        raise RuntimeError('installed output differs from verified build: '+str(installed))
    receipt['identical_build_outputs'][str(installed)]=digest

# Installed data must match this build's intended asset bytes, including locale UIDs.
entries=re.findall(r'\{"([^"]+)", "([^"]+)", "([^"]+)"',
    (programs/'ckmotif-gallery/gallery_demos.h').read_text())
asset_hashes={}
installed_names={}
for _,directory,name in entries: installed_names.setdefault(directory,set()).add(name)
for build_dir,install_dir,binary in entries:
    installed_dir=prefix/'share/Xm'/install_dir
    for asset in installed_dir.rglob('*'):
        if not asset.is_file() or (asset.parent==installed_dir and asset.name in installed_names[install_dir]): continue
        relative=asset.relative_to(installed_dir)
        original=programs/build_dir/relative
        if not original.is_file():
            raise RuntimeError('installed asset has no build input: '+str(asset))
        digest=hashlib.sha256(asset.read_bytes()).hexdigest()
        if hashlib.sha256(original.read_bytes()).hexdigest()!=digest:
            raise RuntimeError('installed asset bytes differ: '+str(asset))
        asset_hashes[str(asset.relative_to(prefix))]=digest
receipt['asset_sha256']=asset_hashes

def run_program(command, timeout):
    process = subprocess.Popen(list(map(str,command)), env=env, cwd=out,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, start_new_session=True)
    try:
        stdout, stderr = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        os.killpg(process.pid, signal.SIGTERM)
        try: process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL); process.communicate()
        raise
    return subprocess.CompletedProcess(process.args, process.returncode, stdout, stderr)
try:
    read_fd, write_fd = os.pipe()
    server = subprocess.Popen(['Xvfb', '-displayfd', str(write_fd), '-screen', '0', '1280x900x24', '-nolisten', 'tcp'],
        env=env, pass_fds=(write_fd,), stdout=(out/'xvfb.log').open('w'), stderr=subprocess.STDOUT)
    processes.append(server); os.close(write_fd)
    with os.fdopen(read_fd) as pipe: env['DISPLAY'] = ':'+pipe.readline().strip()
    manager_path=prefix/'bin/mwm'
    manager=subprocess.Popen([str(manager_path),'-xrm','Mwm*showFeedback: none',
        '-xrm','Mwm*keyboardFocusPolicy: explicit'],env=env,
        stdout=(out/'mwm.log').open('w'),stderr=subprocess.STDOUT)
    processes.append(manager);time.sleep(1)
    if manager.poll() is not None or Path('/proc/'+str(manager.pid)+'/exe').resolve()!=manager_path:
        raise RuntimeError('installed MWM startup/identity failed')
    receipt['manager']={'pid':manager.pid,'executable':str(manager_path)}
    programs.rename(hidden)
    wsm=prefix/'share/Xm/wsm/wsm'
    result=run_program([wsm,'--integration-test','-file',out/'workspace.db','-workspaceList','One,Two'],40)
    (out/'wsm-ui.log').write_text(result.stdout+result.stderr)
    if result.returncode: raise RuntimeError('installed Wsm UI failed')
    receipt['results']['wsm-ui']=0
    for mode in ('--integration-all-demos', '--integration-demos'):
        result = run_program([gallery,mode], 100)
        (out/(mode[2:]+'.log')).write_text(result.stdout+result.stderr)
        receipt['results'][mode] = result.returncode
        if result.returncode: raise RuntimeError(mode+' failed; see evidence log')
    hello = prefix/'share/Xm/hellomotif/hellomotif'
    absent = hello.with_name('hellomotif.install-test-hidden')
    hello.rename(absent)
    try:
        result = run_program([gallery,'--integration-demos'], 30)
        (out/'missing.log').write_text(result.stdout+result.stderr)
        if result.returncode or 'PASS gallery missing demo notice' not in result.stdout:
            raise RuntimeError('installed missing-app notice failed')
        receipt['results']['missing'] = 0
    finally: absent.rename(hello)
    if manager.poll() is not None: raise RuntimeError('installed MWM exited during demo tests')
    receipt['success'] = True
    print('PASS installed MWM/Wsm and gallery: identical binaries, 33 demo startups, UID assets, Open and missing-app notice without build-tree demos')
finally:
    if hidden.exists(): hidden.rename(programs)
    for process in reversed(processes):
        if process.poll() is None:
            process.terminate()
            try: process.wait(timeout=5)
            except subprocess.TimeoutExpired: process.kill(); process.wait()
    (out/'receipt.json').write_text(json.dumps(receipt, indent=2)+'\n')
