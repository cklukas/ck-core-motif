#!/usr/bin/env python3
"""Build probes and test the actual MWM/Wsm processes on an isolated Xvfb display."""
import argparse
import importlib.util
import datetime
import hashlib
import json
import os
import signal
import re
from pathlib import Path
import subprocess
import time

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build-root',required=True,type=Path)
parser.add_argument('--out',required=True,type=Path)
args=parser.parse_args()
source=Path(__file__).resolve().parents[2]
build=args.build_root.resolve()
out=args.out.resolve()
if out.exists():parser.error('--out must be a fresh evidence directory')
out.mkdir(parents=True)
(out/'tmp').mkdir()
(out/'home').mkdir()
env=dict(os.environ,TMPDIR=str(out/'tmp'),HOME=str(out/'home'),PYTHONDONTWRITEBYTECODE='1')
receipt={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
         'source_sha256':{},'build_root':str(build),'results':{},'processes':[]}
for pattern in ('clients/mwm/WmWorkspace.*','clients/mwm/WmWsmLib/*.[ch]',
                'demos/programs/workspace/*.[ch]','tests/automated/*workspace*.c','lib/Xm/Transfer.c','lib/Xm/Traversal.c'):
    for path in source.glob(pattern):receipt['source_sha256'][str(path.relative_to(source))]=hashlib.sha256(path.read_bytes()).hexdigest()
processes=[]
def run(name,command,expected=0,timeout=60):
    start=time.monotonic()
    process=subprocess.Popen(list(map(str,command)),env=env,cwd=out,stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,text=True,start_new_session=True)
    try:
        stdout,stderr=process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        os.killpg(process.pid,signal.SIGTERM)
        try:stdout,stderr=process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid,signal.SIGKILL);stdout,stderr=process.communicate()
        (out/(name+'.log')).write_text(stdout+stderr)
        raise RuntimeError(name+' exceeded its watchdog; test process group stopped')
    result=subprocess.CompletedProcess(command,process.returncode,stdout,stderr)
    (out/(name+'.log')).write_text(result.stdout+result.stderr)
    receipt['results'][name]={'command':list(map(str,command)),'exit':result.returncode,'seconds':time.monotonic()-start}
    if result.returncode!=expected:raise RuntimeError(name+' failed: '+(result.stdout+result.stderr)[-4000:])
    return result
try:
    run('compile-probe',['cc','-std=c99','-Wall','-Wextra','-Werror',source/'tests/automated/probe-mwm-workspace.c','-lX11','-o',out/'probe'])
    run('compile-protocol',['cc','-std=c99','-Wall','-Wextra','-Werror',
        '-I'+str(build/'lib'),'-I'+str(source/'lib'),'-I'+str(source/'clients/mwm/WmWsmLib'),
        source/'tests/automated/mwm-workspace-integration.c',build/'clients/mwm/WmWsmLib/libWsm.a',
        '-L'+str(build/'lib/Xm/.libs'),'-lXm','-lXt','-lX11','-o',out/'protocol'])
    run('compile-traversal',['cc','-std=c99','-Wall','-Wextra','-Werror',
        '-I'+str(build/'lib'),'-I'+str(source/'lib'),source/'tests/automated/traversal-root-shell.c',
        '-L'+str(build/'lib/Xm/.libs'),'-lXm','-lXt','-lX11','-o',out/'traversal'])
    env['LD_LIBRARY_PATH']=str(build/'lib/Xm/.libs')+':'+env.get('LD_LIBRARY_PATH','')
    read_fd,write_fd=os.pipe()
    server=subprocess.Popen(['Xvfb','-displayfd',str(write_fd),'-screen','0','2560x1600x24','-screen','1','2560x1600x24','-nolisten','tcp'],
        env=env,pass_fds=(write_fd,),stdout=(out/'xvfb.log').open('w'),stderr=subprocess.STDOUT)
    processes.append(server);os.close(write_fd)
    with os.fdopen(read_fd) as pipe:env['DISPLAY']=':'+pipe.readline().strip()
    mwm_path=build/'clients/mwm/.libs/mwm'
    newest=max(path.stat().st_mtime_ns for pattern in ('clients/mwm/*.[ch]', 'clients/mwm/WmWsmLib/*.[ch]')
               for path in source.glob(pattern))
    if mwm_path.stat().st_mtime_ns < newest:raise RuntimeError('MWM executable predates its source inputs')
    manager=subprocess.Popen([str(mwm_path),'-multiscreen','-xrm','Mwm*keyboardFocusPolicy: explicit','-xrm','Mwm*showFeedback: none'],
        env=env,stdout=(out/'mwm.log').open('w'),stderr=subprocess.STDOUT)
    processes.append(manager)
    time.sleep(1)
    if manager.poll() is not None:raise RuntimeError('MWM exited during startup')
    actual=Path('/proc/'+str(manager.pid)+'/exe').resolve()
    if actual!=mwm_path:raise RuntimeError('manager PID is not the verified executable')
    receipt['processes'].append({'pid':manager.pid,'executable':str(actual),'sha256':hashlib.sha256(actual.read_bytes()).hexdigest()})
    run('traversal',[out/'traversal'])
    run('targets',[out/'probe'])
    run('protocol',[out/'protocol'])
    wsm_path=build/'demos/programs/workspace/.libs/wsm'
    if wsm_path.stat().st_mtime_ns < max(path.stat().st_mtime_ns for path in (source/'demos/programs/workspace').glob('*.[ch]')):
        raise RuntimeError('Wsm executable predates its source inputs')
    run('wsm-ui',[wsm_path,'--integration-test','-file',out/'workspace.db','-workspaceList','One,Two'],timeout=40)
    if manager.poll() is not None:raise RuntimeError('MWM failed during workspace tests')
    restart_log=out/'wsm-restart.log'
    with restart_log.open('w') as log:
        client=subprocess.Popen([str(wsm_path),'--integration-restart-test','-file',str(out/'restart.db'),
                                 '-workspaceList','One,Two'],env=env,cwd=out,stdout=log,stderr=subprocess.STDOUT)
        processes.append(client)
        deadline=time.monotonic()+25
        while 'READY Wsm integration restart' not in restart_log.read_text():
            if client.poll() is not None or time.monotonic()>deadline:
                raise RuntimeError('Wsm restart readiness failed (exit='+str(client.poll())+'): '+restart_log.read_text()[-3000:])
            time.sleep(.1)
        manager.terminate();manager.wait(timeout=10)
        manager=subprocess.Popen([str(mwm_path),'-multiscreen','-xrm','Mwm*keyboardFocusPolicy: explicit','-xrm','Mwm*showFeedback: none'],
            env=env,stdout=(out/'mwm-restart.log').open('w'),stderr=subprocess.STDOUT)
        processes.append(manager)
        client.wait(timeout=30)
        if client.returncode or 'PASS Wsm reconnected after MWM restart' not in restart_log.read_text():
            raise RuntimeError('Wsm restart failed: '+restart_log.read_text()[-4000:])
    receipt['results']['restart']={'exit':0,'log':str(restart_log)}
    with (out/'wsm-signal.log').open('w') as log:
        signal_client=subprocess.Popen([str(wsm_path),'-file',str(out/'signal.db'),
            '-workspaceList','One,Two'],env=env,cwd=out,stdout=log,stderr=subprocess.STDOUT)
        processes.append(signal_client)
        time.sleep(1)
        if signal_client.poll() is not None: raise RuntimeError('Wsm exited before signal test')
        signal_client.terminate(); signal_client.wait(timeout=10)
        if signal_client.returncode: raise RuntimeError('Wsm termination did not complete through Xt cleanup')
    receipt['results']['wsm-signal']={'exit':0,'signal':'SIGTERM'}
    gallery=build/'demos/programs/ckmotif-gallery/.libs/ckmotif-gallery'
    run('gallery-demos',[gallery,'--integration-demos'])
    run('gallery-all-demos',[gallery,'--integration-all-demos'],timeout=90)
    if manager.poll() is not None: raise RuntimeError('MWM exited during demo startup: '+str(manager.returncode))
    run('targets-after-demos',[out/'probe'],timeout=10)
    # Temporarily remove only this task's executable to exercise the real missing-app notice.
    hello=build/'demos/programs/hellomotif/hellomotif'
    hidden=hello.with_name('hellomotif.test-hidden')
    hello.rename(hidden)
    try:run('gallery-missing',[gallery,'--integration-demos'])
    finally:hidden.rename(hello)
    env['CKMOTIF_GALLERY']=str(gallery)
    run('gallery-regression',['python3',source/'tests/automated/run-gallery-integration.py'],timeout=660)
    if manager.poll() is not None:raise RuntimeError('MWM failed during gallery tests')
    spec=importlib.util.spec_from_file_location('gallery_capture',source/'tests/automated/run-gallery-screenshots.py')
    capture=importlib.util.module_from_spec(spec);spec.loader.exec_module(capture)
    for name,geometry in (('desktop','1100x740'),('compact','450x500')):
        process=subprocess.Popen([str(gallery),'--component','gallery.demos','--screenshot-mode',
                                  '-geometry',geometry],env=env,cwd=out,
                                  stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
        processes.append(process)
        window=capture.find_window_id('xwininfo',env,process=process,entry_id='gallery.demos')
        capture.wait_ready(window,process,env,'gallery.demos')
        actual=Path('/proc/'+str(process.pid)+'/exe').resolve()
        if actual!=gallery: raise RuntimeError('capture PID is not the verified gallery executable')
        receipt['processes'].append({'pid':process.pid,'window':window,'executable':str(actual),
            'sha256':hashlib.sha256(actual.read_bytes()).hexdigest(),'library':capture.loaded_library(process)})
        # The client must remain inside the manager's decorated frame. Moving
        # its direct parent corrupts MWM's separate base/frame arrangement.
        def window_geometry(identifier):
            result=subprocess.run(['xwininfo','-id',identifier,'-stats','-tree'],env=env,
                capture_output=True,text=True,check=True,timeout=5).stdout
            def field(pattern):
                match=re.search(pattern,result,re.M)
                if not match: raise RuntimeError('missing X window geometry field')
                return match.group(1)
            return {'window':identifier,'root':field(r'Root window id: (0x[0-9a-fA-F]+)'),
                'parent':field(r'Parent window id: (0x[0-9a-fA-F]+)'),
                'x':int(field(r'Absolute upper-left X:\s*(-?\d+)')),
                'y':int(field(r'Absolute upper-left Y:\s*(-?\d+)')),
                'width':int(field(r'^\s*Width:\s*(\d+)')),
                'height':int(field(r'^\s*Height:\s*(\d+)'))}
        client_geometry=frame_geometry=window_geometry(window)
        for level in range(64):
            if frame_geometry['parent']==frame_geometry['root']: break
            frame_geometry=window_geometry(frame_geometry['parent'])
        else: raise RuntimeError('unexpectedly deep gallery frame hierarchy')
        if not (frame_geometry['x']<=client_geometry['x'] and frame_geometry['y']<=client_geometry['y'] and
            client_geometry['x']+client_geometry['width']<=frame_geometry['x']+frame_geometry['width'] and
            client_geometry['y']+client_geometry['height']<=frame_geometry['y']+frame_geometry['height']):
            raise RuntimeError('gallery client lies outside its manager frame')
        receipt['processes'][-1]['client_geometry']=client_geometry
        receipt['processes'][-1]['frame_geometry']=frame_geometry
        xwd=out/('gallery-demos-'+name+'.xwd');png=xwd.with_suffix('.png')
        with xwd.open('wb') as stream:
            subprocess.run(['xwd','-id',window,'-silent','-screen'],env=env,stdout=stream,check=True,timeout=10)
        pnm=subprocess.run(['xwdtopnm',str(xwd)],env=env,capture_output=True,check=True,timeout=10)
        with png.open('wb') as stream:
            subprocess.run(['pnmtopng'],env=env,input=pnm.stdout,stdout=stream,check=True,timeout=10)
        from PIL import Image
        with Image.open(png) as captured:
            if all(low==high for low,high in captured.convert('RGB').getextrema()):
                raise RuntimeError('gallery screenshot is blank: '+str(png))
        xwd.unlink();capture.stop_process(process)
    # Also manage only a nonzero default screen: managed-array indices differ from X screen numbers.
    manager.terminate();manager.wait(timeout=10)
    env['DISPLAY']=env['DISPLAY']+'.1'
    manager=subprocess.Popen([str(mwm_path),'-xrm','Mwm*multiScreen: False','-xrm','Mwm*showFeedback: none'],env=env,
        stdout=(out/'mwm-screen1.log').open('w'),stderr=subprocess.STDOUT)
    processes.append(manager);time.sleep(1)
    run('targets-screen1',[out/'probe'])
    run('protocol-screen1',[out/'protocol'])
    run('wsm-ui-screen1',[wsm_path,'--integration-test','-file',out/'screen1.db','-workspaceList','One,Two'],timeout=40)
    receipt['success']=True
    print('PASS real MWM protocol and Wsm workspace UI on '+env['DISPLAY'])
finally:
    for process in reversed(processes):
        if process.poll() is None:
            process.terminate()
            try:process.wait(timeout=5)
            except subprocess.TimeoutExpired:process.kill();process.wait()
    receipt['ended_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
