#!/usr/bin/env python3
"""Qualify local Compose/dead keys through real XTest and native XIM lookup.

Usage: local_compose.py ABSOLUTE_BINARY FRESH_ABSOLUTE_OUTPUT --prefix PREFIX
       [--runtime-library-dir DIRECTORY ...]
The caller compiles the matching C fixture; this observer never builds anything.
It owns an authenticated Xvfb/Openbox and changes only that server's unused keys.
No IBus, external XIM, clipboard, physical desktop or Escape behavior is tested.
"""
import argparse
import ctypes as C
import ctypes.util
import hashlib
import json
import os
from pathlib import Path
import re
import secrets
import select
import shutil
import signal
import struct
import subprocess
import time


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def identity(pid):
    path = Path('/proc') / str(pid)
    fields = (path / 'stat').read_text().rsplit(')',1)[1].split()
    return dict(pid=pid,exe=os.readlink(path/'exe'),start_ticks=fields[19])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('binary',type=Path)
    parser.add_argument('output',type=Path)
    parser.add_argument('--prefix',required=True,type=Path)
    parser.add_argument('--runtime-library-dir',action='append',default=[],type=Path,
                        help='additional absolute runtime library directory; may be repeated')
    args = parser.parse_args()
    assert os.uname().sysname == 'Linux', 'Run native qualification on Linux'
    assert all(p.is_absolute() for p in (args.binary,args.output,args.prefix))
    binary=args.binary.resolve(strict=True); prefix=args.prefix.resolve(strict=True)
    runtime_library_dirs=[]
    for path in args.runtime_library_dir:
        assert path.is_absolute(), '--runtime-library-dir must be absolute'
        path=path.resolve(strict=True)
        assert ':' not in str(path), 'runtime library directories must have no colon'
        runtime_library_dirs.append(path)
    library_roots=((prefix/'lib').resolve(strict=True), *runtime_library_dirs)
    out=args.output
    assert not out.exists() and not out.resolve().is_relative_to(prefix)
    assert binary.is_file() and os.access(binary,os.X_OK)
    source=Path(__file__).with_suffix('.c')
    compose=Path(__file__).with_suffix('.Compose')
    assert source.is_file() and compose.is_file()
    out.mkdir(parents=True,mode=0o700,exist_ok=False)
    for name in ('tmp','runtime','cache','config','state','home'):
        (out/name).mkdir(mode=0o700)
    os.environ['TMPDIR']=str(out/'tmp')
    compose_copy=out/'Compose'
    shutil.copyfile(compose,compose_copy)
    env=dict(os.environ,TMPDIR=str(out/'tmp'),LC_ALL='C.UTF-8',LANG='C.UTF-8',
             XMODIFIERS='@im=none',XCOMPOSEFILE=str(compose_copy),
             XCOMPOSECACHE=str(out/'cache'),HOME=str(out/'home'),
             XDG_RUNTIME_DIR=str(out/'runtime'),XDG_CACHE_HOME=str(out/'cache'),
             XDG_CONFIG_HOME=str(out/'config'),XDG_STATE_HOME=str(out/'state'),
             DBUS_SESSION_BUS_ADDRESS='unix:path='+str(out/'absent.bus'),
             LD_LIBRARY_PATH=os.pathsep.join(str(path) for path in library_roots))
    for name in ('DISPLAY','XAUTHORITY','WAYLAND_DISPLAY','AT_SPI_BUS_ADDRESS',
                 'IBUS_ADDRESS','SESSION_MANAGER','LD_PRELOAD','LD_AUDIT'):
        env.pop(name,None)
    record=dict(success=False,started_ns=time.time_ns(),scope='local XIM Compose/dead-key native commits',
                binary=dict(path=str(binary),sha256=sha(binary),mtime_ns=binary.stat().st_mtime_ns),
                sources={str(p):sha(p) for p in (Path(__file__),source,compose)},
                compose_input=dict(path=str(compose_copy),sha256=sha(compose_copy)),
                processes={},observations=[],key_events=[],policies={},
                runtime_library_dirs=[str(path) for path in runtime_library_dirs])
    children={}; identities={}; buffers={}; streams={}
    reader=writer=-1; display=None; original_map=None; mappings={}; x11=xtst=None

    def save():
        (out/'receipt.json').write_text(json.dumps(record,indent=2)+'\n')

    def command(argv):
        result=subprocess.run([str(x) for x in argv],env=env,capture_output=True,text=True,timeout=15)
        assert result.returncode==0, (argv,result.stderr)
        return result.stdout

    def start(name,argv,interactive=False,pass_fds=()):
        stderr=(out/(name+'.stderr.log')).open('xb'); streams[name]=stderr
        stdout=subprocess.PIPE if interactive else stderr
        p=subprocess.Popen([str(x) for x in argv],env=env,cwd=out,stdin=subprocess.PIPE if interactive else subprocess.DEVNULL,
                           stdout=stdout,stderr=stderr,bufsize=0,pass_fds=pass_fds,start_new_session=True)
        children[name]=p
        deadline=time.monotonic()+3
        expected=str(Path(shutil.which(str(argv[0])) or str(argv[0])).resolve())
        while True:
            assert p.poll() is None, (name,p.returncode)
            info=identity(p.pid)
            if info['exe']==expected: break
            assert time.monotonic()<deadline, 'Executable identity deadline'
            time.sleep(.01)
        identities[name]=info
        record['processes'][name]=dict(info,argv=[str(x) for x in argv])
        buffers[name]=b''
        save()
        return p

    def read(name):
        p=children[name]; deadline=time.monotonic()+8
        while b'\n' not in buffers[name]:
            assert time.monotonic()<deadline, 'Native reply deadline: '+name
            ready=select.select([p.stdout],[],[],max(0,deadline-time.monotonic()))[0]
            assert ready, 'Native reply timed out: '+name
            block=os.read(p.stdout.fileno(),65536)
            if not block:
                try:
                    exit_code=p.wait(timeout=1)
                except subprocess.TimeoutExpired:
                    exit_code=p.poll()
                stderr_path=out/(name+'.stderr.log')
                stderr_tail=stderr_path.read_text(errors='replace').splitlines()[-20:]
                raise AssertionError(dict(name=name,exit_code=exit_code,
                                          stderr_tail=stderr_tail))
            buffers[name]+=block
        line,buffers[name]=buffers[name].split(b'\n',1)
        with (out/(name+'.stdout.log')).open('ab') as stream: stream.write(line+b'\n')
        return json.loads(line)

    def request(name,action):
        children[name].stdin.write((action+'\n').encode()); children[name].stdin.flush()
        return read(name)

    def snapshot(name,label):
        value=request(name,'snapshot')
        for key in ('field','text'): value[key]=bytes.fromhex(value[key]).decode('utf-8')
        record['observations'].append(dict(policy=name,label=label,time_ns=time.time_ns(),value=value))
        return value

    def key(keysym):
        code=mappings[keysym]
        assert xtst.XTestFakeKeyEvent(display,code,1,0)
        assert xtst.XTestFakeKeyEvent(display,code,0,0)
        x11.XSync(display,0)
        record['key_events'].append(dict(keysym=keysym,keycode=code,time_ns=time.time_ns()))

    def map_state():
        low=C.c_int(); high=C.c_int(); per=C.c_int()
        x11.XDisplayKeycodes(display,C.byref(low),C.byref(high))
        values=x11.XGetKeyboardMapping(display,low.value,high.value-low.value+1,C.byref(per))
        assert values and per.value>0
        result={code:[int(values[(code-low.value)*per.value+i]) for i in range(per.value)]
                for code in range(low.value,high.value+1)}
        x11.XFree(C.cast(values,C.c_void_p))
        return dict(minimum=low.value,maximum=high.value,columns=per.value,keys=result)

    def auth(path,number,cookie):
        field=lambda data:struct.pack('>H',len(data))+data
        path.write_bytes(struct.pack('>H',65535)+field(b'')+field(str(number).encode())+
                         field(b'MIT-MAGIC-COOKIE-1')+field(cookie))
        path.chmod(0o600)

    try:
        cookie=secrets.token_bytes(16); auth(out/'server.auth',0,cookie)
        reader,writer=os.pipe()
        server=start('server',['Xvfb','-displayfd',str(writer),'-screen','0','1024x768x24',
                              '-nolisten','tcp','-noreset','-auth',str(out/'server.auth')],pass_fds=(writer,))
        os.close(writer); writer=-1
        assert select.select([reader],[],[],15)[0], 'Private server readiness deadline'
        number=os.read(reader,32).decode().strip()
        assert number.isdecimal() and server.poll() is None
        env['DISPLAY']=':'+number
        auth(out/'client.auth',number,cookie); env['XAUTHORITY']=str(out/'client.auth')
        (out/'xdpyinfo.log').write_text(command(['xdpyinfo']))
        wm=start('wm',['openbox','--sm-disable'])
        deadline=time.monotonic()+10
        while '_NET_SUPPORTING_WM_CHECK(WINDOW)' not in command(['xprop','-root','_NET_SUPPORTING_WM_CHECK']):
            assert wm.poll() is None and time.monotonic()<deadline
            time.sleep(.05)
        # ctypes loads the native libraries; this connection and every remap are
        # restricted to the fresh authenticated server above.
        os.environ['DISPLAY']=env['DISPLAY']; os.environ['XAUTHORITY']=env['XAUTHORITY']
        x11=C.CDLL(ctypes.util.find_library('X11'))
        xtst=C.CDLL(ctypes.util.find_library('Xtst'))
        own_maps=Path('/proc/self/maps').read_text()
        (out/'injection-process-maps.log').write_text(own_maps)
        injected=sorted({line.split()[-1] for line in own_maps.splitlines()
                         if '/' in line and any(stem in line for stem in ('libX11.so','libXtst.so'))})
        record['injection_libraries']={path:sha(path) for path in injected}
        x11.XOpenDisplay.argtypes=[C.c_char_p]; x11.XOpenDisplay.restype=C.c_void_p
        x11.XCloseDisplay.argtypes=[C.c_void_p]
        x11.XSync.argtypes=[C.c_void_p,C.c_int]
        x11.XDisplayKeycodes.argtypes=[C.c_void_p,C.POINTER(C.c_int),C.POINTER(C.c_int)]
        x11.XGetKeyboardMapping.argtypes=[C.c_void_p,C.c_ubyte,C.c_int,C.POINTER(C.c_int)]
        x11.XGetKeyboardMapping.restype=C.POINTER(C.c_ulong)
        x11.XChangeKeyboardMapping.argtypes=[C.c_void_p,C.c_int,C.c_int,C.POINTER(C.c_ulong),C.c_int]
        x11.XFree.argtypes=[C.c_void_p]
        xtst.XTestQueryExtension.argtypes=[C.c_void_p,C.POINTER(C.c_int),C.POINTER(C.c_int),C.POINTER(C.c_int),C.POINTER(C.c_int)]
        xtst.XTestFakeKeyEvent.argtypes=[C.c_void_p,C.c_uint,C.c_int,C.c_ulong]
        display=x11.XOpenDisplay(env['DISPLAY'].encode()); assert display
        event=C.c_int(); error=C.c_int(); major=C.c_int(); minor=C.c_int()
        assert xtst.XTestQueryExtension(display,C.byref(event),C.byref(error),C.byref(major),C.byref(minor))
        record['xtest_version']=[major.value,minor.value]
        original_map=map_state()
        (out/'keymap-before.json').write_text(json.dumps(original_map,indent=2)+'\n')
        symbols={'Multi_key':0xff20,'dead_acute':0xfe51,'apostrophe':0x27,'e':0x65,
                 'equal':0x3d,'Left':0xff51,'Right':0xff53,'BackSpace':0xff08}
        unused=[code for code,values in original_map['keys'].items() if not any(values)]
        assert len(unused)>=len(symbols), 'Need eight unused keys on the private server'
        for (name,symbol),code in zip(symbols.items(),unused):
            value=(C.c_ulong*1)(symbol)
            x11.XChangeKeyboardMapping(display,code,1,value,1); mappings[name]=code
        x11.XSync(display,0)
        modified=map_state(); (out/'keymap-used.json').write_text(json.dumps(modified,indent=2)+'\n')
        for name,code in mappings.items(): assert modified['keys'][code][0]==symbols[name]
        record['keymap_inputs']={name:dict(path=str(out/name),sha256=sha(out/name))
                                 for name in ('keymap-before.json','keymap-used.json')}
        record['mapped_keys']=mappings
        for policy in ('shell','widget'):
            env['XM_LOCAL_COMPOSE_POLICY']=policy
            p=start(policy,[binary],True); initial=read(policy)
            assert initial['pid']==p.pid and initial['policy']==policy and initial['local_im'] and initial['external_xim_servers']==0
            command(['xdotool','windowactivate','--sync',str(initial['shell'])])
            maps=Path('/proc/%d/maps'%p.pid).read_text(); (out/(policy+'-maps.log')).write_text(maps)
            libraries=sorted({line.split()[-1] for line in maps.splitlines() if '/' in line and '.so' in line})
            for stem in ('libXm.so','libXt.so'):
                selected=[Path(path) for path in libraries if Path(path).name.startswith(stem)]
                selected_path=selected[0].resolve() if len(selected)==1 else None
                allowed = selected_path is not None and any(
                    selected_path.is_relative_to(root) for root in library_roots)
                assert allowed, (stem,selected,library_roots)
            record['policies'][policy]=dict(initial=initial,libraries={path:sha(path) for path in libraries})
            expected={'field':'','text':''}; counts={'field':0,'text':0}; carets={'field':0,'text':0}

            def check(label,focus):
                value=snapshot(policy,label)
                assert value['focus']==focus and value['xim'] and value['field_xic'] and value['text_xic'] and value['x_errors']==0
                for widget in ('field','text'):
                    assert value[widget]==expected[widget], (policy,label,widget,value,expected)
                    assert value[widget+'_changes']==counts[widget], (policy,label,'callbacks',value,counts)
                    assert value[widget+'_length']==len(expected[widget]) and value[widget+'_caret']==carets[widget], (policy,label,'scalar caret',value,carets)
                return value

            for widget in ('field','text'):
                request(policy,'focus-'+widget)
                check(widget+'-focused-empty',widget)
                for case,keys,result in (
                    ('compose-acute',('Multi_key','apostrophe','e'),'é'),
                    ('dead-acute',('dead_acute','e'),'é'),
                    ('compose-combining',('Multi_key','e','equal'),'e\u0301')):
                    for step,keysym in enumerate(keys):
                        key(keysym)
                        if step==len(keys)-1:
                            expected[widget]=result; counts[widget]+=1; carets[widget]=len(result)
                        check(widget+'-'+case+'-'+str(step),widget)
                    key('Left'); carets[widget]=0; check(widget+'-'+case+'-left-grapheme',widget)
                    key('Right'); carets[widget]=len(result); check(widget+'-'+case+'-right-grapheme',widget)
                    key('BackSpace'); expected[widget]=''; counts[widget]+=1; carets[widget]=0
                    check(widget+'-'+case+'-delete-grapheme',widget)
                # Leave one committed value while the other widget is exercised.
                for keysym in ('Multi_key','apostrophe','e'): key(keysym)
                expected[widget]='é'; counts[widget]+=1; carets[widget]=1
                check(widget+'-retained-value',widget)
            p.stdin.write(b'quit\n'); p.stdin.flush()
            assert read(policy)==dict(finished=True,x_errors=0)
            assert p.wait(timeout=10)==0
            record['processes'][policy]['exit']=0
            record['policies'][policy]['passed']=True
        record['success']=True
    finally:
        if display and x11:
            if original_map:
                for code in mappings.values():
                    values=original_map['keys'][code]; array=(C.c_ulong*len(values))(*values)
                    x11.XChangeKeyboardMapping(display,code,len(values),array,1)
                x11.XSync(display,0)
                restored=map_state(); (out/'keymap-restored.json').write_text(json.dumps(restored,indent=2)+'\n')
                record['keymap_restored']=restored==original_map
            x11.XCloseDisplay(display)
        for name,p in reversed(list(children.items())):
            forced=p.poll() is None
            if forced:
                info=identities.get(name)
                if info and identity(p.pid)==info:
                    fd=os.pidfd_open(p.pid)
                    try: signal.pidfd_send_signal(fd,signal.SIGTERM)
                    finally: os.close(fd)
                try: p.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    info=identities[name]; assert identity(p.pid)==info
                    fd=os.pidfd_open(p.pid)
                    try: signal.pidfd_send_signal(fd,signal.SIGKILL)
                    finally: os.close(fd)
                    p.wait(timeout=5)
            record['processes'][name]['exit']=p.returncode
            record['processes'][name]['cleanup_signal_required']=forced
        for stream in streams.values(): stream.close()
        for fd in (reader,writer):
            if fd>=0: os.close(fd)
        for name in ('client.auth','server.auth'): (out/name).unlink(missing_ok=True)
        record['authentication_removed']=all(not (out/name).exists() for name in ('client.auth','server.auth'))
        record['ended_ns']=time.time_ns()
        record['success']=bool(record['success'] and record.get('keymap_restored') and
            record['authentication_removed'] and all(record['processes'].get(name,{}).get('exit')==0
                                                   for name in ('shell','widget','wm','server')))
        save()
        print(json.dumps(dict(success=record['success'],receipt=str(out/'receipt.json'))),flush=True)


if __name__=='__main__':
    main()
