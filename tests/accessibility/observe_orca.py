#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Observe real Orca/Speech Dispatcher/eSpeak-NG with native controls.

Uses a fresh owned Xvfb, session/accessibility bus and ALSA file PCM sink.
Does not replace a desktop client, change HOME or write user preferences.
Requires Linux, Orca, speech-dispatcher-espeak-ng, Python GI (Gio), Xvfb,
Openbox, xdotool and gdbus. Compile native_controls.c against the exact
installed toolkit first; this observer deliberately never builds a library.
All output/temp paths must be explicitly supplied and fresh. Keep --runtime
short enough for the GTK accessibility UNIX socket. No audio device is used.
A passed native chrome baseline does not qualify a full application or human
screen-reader usability; receipt.json lists the intentionally unaccepted scope.
"""
import argparse,json,os,pathlib,signal,subprocess,sys
if '--inside' not in sys.argv:
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--fixture',type=pathlib.Path,required=True)
 parser.add_argument('--prefix',type=pathlib.Path,required=True)
 parser.add_argument('--out',type=pathlib.Path,required=True)
 parser.add_argument('--runtime',type=pathlib.Path,required=True)
 args=parser.parse_args()
 fixture=args.fixture.resolve(strict=True)
 prefix=args.prefix.resolve(strict=True)
 if not args.prefix.is_absolute():parser.error('prefix must be absolute')
 if not args.out.is_absolute() or not args.runtime.is_absolute():parser.error('output/runtime paths must be absolute')
 if len(os.fsencode(args.runtime))>60 or len(os.fsencode(args.out/'spd.sock'))>100:parser.error('choose shorter owned output/runtime paths for UNIX sockets')
 args.out.mkdir(mode=0o700);args.runtime.mkdir(mode=0o700)
 env=dict(os.environ,GSETTINGS_BACKEND='memory',LC_ALL='C.UTF-8',ORCA_TEST_ROOT=str(args.out),ORCA_TEST_FIXTURE=str(fixture),ORCA_TEST_PREFIX=str(prefix),LD_LIBRARY_PATH=str(prefix/'lib'),PYTHONDONTWRITEBYTECODE='1')
 for key in ('LD_PRELOAD','DBUS_SESSION_BUS_ADDRESS','DBUS_STARTER_ADDRESS','DBUS_STARTER_BUS_TYPE','AT_SPI_BUS_ADDRESS','DISPLAY'):
  env.pop(key,None)
 for key,folder in [('TMPDIR','tmp'),('XDG_CONFIG_HOME','config_home'),('XDG_DATA_HOME','data_home'),('XDG_CACHE_HOME','cache_home'),('XDG_STATE_HOME','state_home')]:
  path=args.out/folder;path.mkdir(mode=0o700);env[key]=str(path)
 env['XDG_RUNTIME_DIR']=str(args.runtime)
 command=['xvfb-run','-a','-s','-screen 0 1280x1000x24','--','dbus-run-session','--',sys.executable,str(pathlib.Path(__file__).resolve()),'--inside']
 process=subprocess.Popen(command,env=env,start_new_session=True)
 try:
  result=process.wait(timeout=90)
 except subprocess.TimeoutExpired:
  os.killpg(process.pid,signal.SIGTERM)
  try:process.wait(timeout=12)
  except subprocess.TimeoutExpired:
   os.killpg(process.pid,signal.SIGKILL);process.wait(timeout=5)
  # Each service is its own process group. Retain exact /proc start identity so
  # a timeout can clean owned groups without signalling a reused or user PID.
  registry=args.out/'owned-processes.jsonl'
  if registry.exists():
   for line in registry.read_text().splitlines():
    item=json.loads(line)
    try:
     fields=(pathlib.Path('/proc')/str(item['pid'])/'stat').read_text().rsplit(') ',1)[1].split()
     if fields[19]==item['start'] and int(fields[2])==item['pid']:
      os.killpg(item['pid'],signal.SIGKILL)
    except (OSError,IndexError):pass
  raise SystemExit('isolated screen-reader observation timed out; owned services stopped')
 raise SystemExit(result)

import datetime,hashlib,json,os,pathlib,select,shutil,signal,subprocess,time
import sys
base=pathlib.Path(__file__).resolve().parent
root=pathlib.Path(os.environ['ORCA_TEST_ROOT'])
if not root.is_dir():raise RuntimeError('Outer isolation root missing')
for name in ('tmp','config','config/modules','logs','orca-prefs'):(root/name).mkdir(mode=0o700,exist_ok=True)
users=[]
for path in pathlib.Path('/proc').glob('[0-9]*'):
 try:
  command=(path/'cmdline').read_bytes().split(b'\0')
  name=pathlib.Path(os.fsdecode(command[0])).name
  if name in ('orca','speech-dispatcher','sd_espeak-ng'):
   users.append({'pid':int(path.name),'command':[os.fsdecode(x) for x in command if x]})
 except (OSError,IndexError):pass
if users:raise RuntimeError('Assistive process already present: '+str(users))
env=dict(os.environ,TMPDIR=str(root/'tmp'),XDG_RUNTIME_DIR=os.environ['XDG_RUNTIME_DIR'],GSETTINGS_BACKEND='memory',LC_ALL='C.UTF-8',GIO_USE_VFS='local',PYTHONDONTWRITEBYTECODE='1')
for k in ('AT_SPI_BUS_ADDRESS','DBUS_STARTER_ADDRESS','DBUS_STARTER_BUS_TYPE','NO_AT_BRIDGE','SPEECHD_HOST','SPEECHD_PORT','SPEECHD_SOCKET'):
 env.pop(k,None)
for key in ('CONFIG_HOME','DATA_HOME','CACHE_HOME','STATE_HOME'):
 path=root/key.lower();path.mkdir(mode=0o700,exist_ok=True);env['XDG_'+key]=str(path)
env.update(SPEECHD_ADDRESS='unix_socket:'+str(root/'spd.sock'),ALSA_CONFIG_PATH=str(root/'alsa.conf'),PULSE_SERVER='unix:'+str(root/'unavailable-pulse'),PIPEWIRE_REMOTE=str(root/'unavailable-pipewire'))
(root/'config/modules/espeak-ng.conf').write_text(pathlib.Path('/etc/speech-dispatcher/modules/espeak-ng.conf').read_text().replace('Debug 0','Debug 1',1))
(root/'alsa.conf').write_text('pcm.capture { type file slave.pcm { type null } file "'+str(root/'speech.raw')+'" format "raw" }\n')
(root/'config/speechd.conf').write_text('LogLevel 5\nLogDir "'+str(root/'logs')+'"\nAddModule "espeak-ng" "/usr/lib/speech-dispatcher-modules/sd_espeak-ng" "'+str(root/'config/modules/espeak-ng.conf')+'"\nDefaultModule "espeak-ng"\nDefaultLanguage "en-US"\nAudioOutputMethod "alsa"\nAudioALSADevice "capture"\nDisableAutoSpawn\n')
record={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'display':env['DISPLAY'],'session_bus':env['DBUS_SESSION_BUS_ADDRESS'],'processes_before':users,'steps':[],'orchestration_completed':False,'native_semantic_acceptance':False,'scope':'Actual Orca client and native eSpeak-NG synthesis against isolated native controls; no application or human usability qualification.'}
processes=[]
def launch(args,name,**kw):
 p=subprocess.Popen(args,env=env,stdout=kw.pop('stdout',(root/(name+'.stdout')).open('w')),stderr=(root/(name+'.stderr')).open('w'),start_new_session=True,**kw)
 processes.append(p);record.setdefault('processes',[]).append({'name':name,'pid':p.pid,'command':args})
 start=(pathlib.Path('/proc')/str(p.pid)/'stat').read_text().rsplit(') ',1)[1].split()[19]
 with (root/'owned-processes.jsonl').open('a') as registry:registry.write(json.dumps({'pid':p.pid,'start':start})+'\n')
 return p
def run(args,name=None):
 r=subprocess.run(args,env=env,text=True,capture_output=True,timeout=10)
 if name:(root/(name+'.log')).write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError(str(args)+': '+r.stderr)
 return r.stdout

def read(p):
 if not select.select([p.stdout],[],[],8)[0]:raise RuntimeError('Native fixture output timeout')
 line=p.stdout.readline()
 if not line:raise RuntimeError('Native fixture exited '+str(p.poll()))
 return json.loads(line)

def snapshot(p):
 p.stdin.write('snapshot\n');p.stdin.flush();return read(p)

def step(name,args):
 start=time.monotonic();run(args,name);time.sleep(1.2)
 debug=(root/'orca.debug').read_text(errors='replace') if (root/'orca.debug').exists() else ''
 record['steps'].append({'name':name,'command':args,'seconds':time.monotonic()-start,'snapshot':snapshot(fixture),'speech_lines':[s for s in debug.splitlines()[offsets[0]:] if 'SPEECH OUTPUT:' in s or 'SPEECH DISPATCHER: Speaking' in s]})
 offsets[0]=len(debug.splitlines())

def click(name):
 wid=ids[name]['window'];step('focus-'+name,['xdotool','mousemove','--window',str(wid),'15','12','click','1'])

def key(name,keys):step(name,['xdotool','key','--clearmodifiers',keys])

def stop(p):
 if p.poll() is not None:return
 try:os.killpg(p.pid,signal.SIGTERM)
 except ProcessLookupError:pass
 try:p.wait(timeout=5)
 except subprocess.TimeoutExpired:
  os.killpg(p.pid,signal.SIGKILL);p.wait(timeout=5)

signal.signal(signal.SIGTERM,lambda *_:(_ for _ in ()).throw(SystemExit(143)))
began=time.monotonic()
try:
 raw=run(['gdbus','call','--session','--dest','org.a11y.Bus','--object-path','/org/a11y/bus','--method','org.a11y.Bus.GetAddress'],'private-bus-address')
 import ast
 address=ast.literal_eval(raw.strip())[0]
 if env['XDG_RUNTIME_DIR'] not in address or ('/run/user/'+str(os.getuid())+'/at-spi') in address:raise RuntimeError('Accessibility bus not in private runtime: '+address)
 env['AT_SPI_BUS_ADDRESS']=address;record['accessibility_bus']=address
 run(['gdbus','introspect','--address',address,'--dest','org.a11y.atspi.Registry','--object-path','/org/a11y/atspi/accessible/root'],'private-registry')
 launch(['openbox','--sm-disable','--config-file','/usr/share/openbox/rc.xml'],'openbox');time.sleep(.4)
 speech=launch(['speech-dispatcher','--run-single','--socket-path',str(root/'spd.sock'),'--config-dir',str(root/'config'),'--log-dir',str(root/'logs'),'--pid-file',str(root/'speechd.pid'),'--timeout','0','--log-level','5'],'speechd')
 for _ in range(100):
  if (root/'spd.sock').exists():break
  if speech.poll() is not None:raise RuntimeError('Speech server exited')
  time.sleep(.05)
 run(['orca','--help'],'orca-help')
 orca=launch(['orca','--user-prefs',str(root/'orca-prefs'),'--debug-file',str(root/'orca.debug'),'--speech-system','speechdispatcherfactory','--enable','speech','--disable','braille,sound'],'orca')
 time.sleep(3)
 if orca.poll() is not None:raise RuntimeError('Orca exited '+str(orca.returncode))
 fixture=launch([env.get('ORCA_TEST_FIXTURE',str(base/'native-controls'))],'fixture',stdin=subprocess.PIPE,stdout=subprocess.PIPE,text=True,bufsize=1)
 ids=read(fixture);record['fixture_identity']=ids
 executable=pathlib.Path('/proc')/str(fixture.pid)/'exe'
 actual_executable=executable.resolve(strict=True)
 if actual_executable!=pathlib.Path(env['ORCA_TEST_FIXTURE']).resolve(strict=True):raise RuntimeError('Fixture PID executable mismatch')
 mapped=[]
 for line in (pathlib.Path('/proc')/str(fixture.pid)/'maps').read_text().splitlines():
  path=line.split()[-1]
  if '/libXm.so.' in path and path not in mapped:mapped.append(path)
 # CK Core Motif's installed ABI is libXm.so.8.  Keep this identity check
 # aligned with the public ABI contract; accepting an older SONAME here would
 # allow the observer to report a pass against a different toolkit.
 expected_library=(pathlib.Path(env['ORCA_TEST_PREFIX'])/'lib/libXm.so.8').resolve(strict=True)
 if len(mapped)!=1 or pathlib.Path(mapped[0]).resolve(strict=True)!=expected_library:raise RuntimeError('Unexpected toolkit mapping: '+repr(mapped))
 record['runtime_identity']={'executable':str(actual_executable),'executable_sha256':hashlib.sha256(actual_executable.read_bytes()).hexdigest(),'library':str(expected_library),'library_sha256':hashlib.sha256(expected_library.read_bytes()).hexdigest()}
 if shutil.which('dpkg-query'):
  packages=subprocess.run(['dpkg-query','-W','-f=${Package}\t${Version}\n','orca','speech-dispatcher','speech-dispatcher-espeak-ng','espeak-ng','libatspi2.0-0t64'],capture_output=True,text=True,env=env)
  record['debian_packages']=packages.stdout

 for _ in range(60):
  if snapshot(fixture)['connected']:break
  time.sleep(.1)
 else:raise RuntimeError('Fixture not connected')
 # Actual external D-Bus calls, independently of Orca's speech observations.
 import gi
 gi.require_version('Gio','2.0')
 from gi.repository import Gio,GLib
 connection=Gio.DBusConnection.new_for_address_sync(address,Gio.DBusConnectionFlags.AUTHENTICATION_CLIENT|Gio.DBusConnectionFlags.MESSAGE_BUS_CONNECTION,None,None)
 def protocol(method,signature,args,node='editor'):
  result=connection.call_sync(ids['bus'],'/org/a11y/atspi/accessible/'+str(ids[node]['accessible']),'org.a11y.atspi.Text',method,GLib.Variant(signature,args),None,Gio.DBusCallFlags.NONE,8000,None).unpack()
  record.setdefault('protocol',[]).append({'method':method,'args':args,'result':result})
  return result
 run(['xdotool','windowactivate','--sync',str(ids['shell'])]);time.sleep(2)
 offsets=[0]
 click('button');key('activate-button','space')
 key('tab-toggle','Tab');key('toggle-grid','space')
 click('title');key('title-end','End');step('type-title',['xdotool','type','--clearmodifiers',' report'])
 click('readonly')
 click('editor');key('editor-home','Home');key('editor-right','Right');key('editor-select','shift+Right')
 assert protocol('GetNSelections','()',())==(1,)
 assert protocol('GetSelection','(i)',(0,))==(1,2)
 click('list');key('list-next','Down')
 click('disclosure');key('disclosure-collapse','space');key('disclosure-expand','space')
 click('scrollbar');key('scroll-right','Right')
 key('menu-bar','F10');key('menu-open','Down');key('menu-dismiss','Escape')
 time.sleep(1)
 record['final_snapshot']=snapshot(fixture)
 fixture.stdin.write('set-boundary\n');fixture.stdin.flush();read(fixture)
 expected=[(2,0,('😀',2,3)),(5,0,('́',5,6)),(4,1,('é',4,6)),(9,4,('Aé😀 é. 中\r\n',0,11)),(11,3,('Next.\n',11,17)),(17,3,('',17,17))]
 for offset,granularity,result in expected:
  assert protocol('GetStringAtOffset','(iu)',(offset,granularity))==result
 assert protocol('GetTextAtOffset','(iu)',(9,6))==('\r\nNext.',9,16)
 cache=connection.call_sync(ids['bus'],'/org/a11y/atspi/cache','org.a11y.atspi.Cache','GetItems',None,None,Gio.DBusCallFlags.NONE,8000,None).unpack()[0]
 record['cache_count']=len(cache)
 assert len(cache)>10 and any(x[0][1].endswith('/'+str(ids['editor']['accessible'])) for x in cache)
 indexed={tuple(item[0]):item for item in cache}
 assert len(indexed)==len(cache), 'duplicate Cache identities'
 for item in cache:
  ref=tuple(item[0])
  children=connection.call_sync(ref[0],ref[1],'org.a11y.atspi.Accessible','GetChildren',None,None,Gio.DBusCallFlags.NONE,8000,None).unpack()[0]
  role=connection.call_sync(ref[0],ref[1],'org.a11y.atspi.Accessible','GetRole',None,None,Gio.DBusCallFlags.NONE,8000,None).unpack()[0]
  assert role==item[7] and len(children)==item[4], 'Cache role/children disagree with live interfaces'
  for position,child in enumerate(children):
   cached=indexed[tuple(child)]
   assert tuple(cached[2])==ref and cached[3]==position, 'Cache parent/index disagree with live tree'

 record['boundary_cache_protocol_passed']=True

 record['orchestration_completed']=True
finally:
 if 'orca' in globals():stop(orca)
 if 'fixture' in globals() and fixture.poll() is None:
  fixture.stdin.write('quit\n');fixture.stdin.flush()
  try:fixture.wait(timeout=3)
  except subprocess.TimeoutExpired:stop(fixture)
 for p in reversed(processes):
  if p.poll() is None:stop(p)
 debug=(root/'orca.debug').read_text(errors='replace') if (root/'orca.debug').exists() else ''
 record['utterances']=[line for line in debug.splitlines() if 'SPEECH OUTPUT:' in line or 'SPEECH DISPATCHER: Speaking' in line]
 record['errors']=[line for line in debug.splitlines() if any(x in line for x in ('Traceback','ERROR:','CRITICAL','Exception'))]
 outputs=[line.split('SPEECH OUTPUT:',1)[1] for line in record['utterances'] if 'SPEECH OUTPUT:' in line]
 expected=['Apply changes button','Show grid check box','checked','Document title text','Quarterly notes','Read only status read only text','Ready','Document editor text','Hello é 中 🙂','selected','expanded','collapsed','Zoom level horizontal scroll bar']
 record['required_utterances']={phrase:any(phrase in line for line in outputs) for phrase in expected}
 record['synthesis_connected']=any('SPEECH DISPATCHER: Speaking' in line for line in record['utterances'])
 record['unaccepted_scope']=['native list item semantics','native menu item semantics','precise TextChanged insertion/deletion echo','character geometry','human screen-reader usability','application integration']

 pcm=root/'speech.raw'
 record.update(ended_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),seconds=time.monotonic()-began,pcm_bytes=pcm.stat().st_size if pcm.exists() else 0,pcm_sha256=hashlib.sha256(pcm.read_bytes()).hexdigest() if pcm.exists() else None,process_exits=[{'pid':p.pid,'exit':p.poll()} for p in processes])
 record['native_chrome_baseline_passed']=bool(record['orchestration_completed'] and record.get('boundary_cache_protocol_passed') and record['synthesis_connected'] and record['pcm_bytes']>0 and not record['errors'] and all(record['required_utterances'].values()))
 (root/'receipt.json').write_text(json.dumps(record,ensure_ascii=False,indent=2)+'\n')
 print(json.dumps({'native_chrome_baseline_passed':record['native_chrome_baseline_passed'],'orchestration_completed':record['orchestration_completed'],'native_semantic_acceptance':False,'utterances':len(record['utterances']),'pcm_bytes':record['pcm_bytes'],'errors':record['errors'][-8:]},indent=2))

if not record['native_chrome_baseline_passed']:raise SystemExit(1)
