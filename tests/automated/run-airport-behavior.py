#!/usr/bin/env python3
"""Airport rendering, actual pointer/keyboard behavior, native DnD, and child appearance inheritance."""
from pathlib import Path
import os,subprocess,select,time,json,ctypes as C,sys,hashlib
import argparse,platform,datetime,ctypes.util
parser=argparse.ArgumentParser(description="Exercise Airport with real XTest input on an isolated Xvfb display, never the user desktop.")
parser.add_argument('--prefix',type=Path,required=True)
parser.add_argument('--binary',type=Path)
parser.add_argument('--gallery',type=Path)
parser.add_argument('--out',type=Path,required=True)
a=parser.parse_args();prefix=a.prefix.resolve(strict=True);exe=(a.binary or prefix/'share/Xm/airport/airport').resolve(strict=True);o=a.out.resolve()
if o.exists():parser.error('--out must be fresh')
if platform.system()=='Darwin' and not any(base.parent.is_mount() and o.is_relative_to(base) for base in [Path('/Volumes/S8T/tmp'),Path('/Volumes/PRO-BLADE/tmp')]):parser.error('use a mounted Mac temporary volume')
o.mkdir(parents=True);(o/'tmp').mkdir()
e=dict(os.environ,TMPDIR=str(o/'tmp'),DYLD_LIBRARY_PATH=str(prefix/'lib'),LD_LIBRARY_PATH=str(prefix/'lib'),PATH='/opt/X11/bin:/opt/homebrew/bin:'+os.environ['PATH'],LC_ALL='en_US.UTF-8')
library=prefix/('lib/libXm.9.dylib' if platform.system()=='Darwin' else 'lib/libXm.so')
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
r={'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'binary':str(exe),'binary_sha256':sha(exe),'library_sha256':sha(library),'scope':'Real server input on isolated Xvfb; no desktop input','success':False,'cases':[],'runner_sha256':sha(Path(__file__))};procs=[];children=[]
x=C.CDLL('/opt/X11/lib/libX11.dylib' if platform.system()=='Darwin' else ctypes.util.find_library('X11'));t=C.CDLL('/opt/X11/lib/libXtst.dylib' if platform.system()=='Darwin' else ctypes.util.find_library('Xtst'))
x.XOpenDisplay.argtypes=[C.c_char_p];x.XOpenDisplay.restype=C.c_void_p
x.XFlush.argtypes=[C.c_void_p];x.XSetInputFocus.argtypes=[C.c_void_p,C.c_ulong,C.c_int,C.c_ulong];x.XResizeWindow.argtypes=[C.c_void_p,C.c_ulong,C.c_uint,C.c_uint]
x.XStringToKeysym.argtypes=[C.c_char_p];x.XStringToKeysym.restype=C.c_ulong;x.XKeysymToKeycode.argtypes=[C.c_void_p,C.c_ulong];x.XKeysymToKeycode.restype=C.c_uint
for n,args in [('XTestGrabControl',[C.c_void_p,C.c_int]),('XTestFakeMotionEvent',[C.c_void_p,C.c_int,C.c_int,C.c_int,C.c_ulong]),('XTestFakeButtonEvent',[C.c_void_p,C.c_uint,C.c_int,C.c_ulong]),('XTestFakeKeyEvent',[C.c_void_p,C.c_uint,C.c_int,C.c_ulong])]:getattr(t,n).argtypes=args
rd,wr=os.pipe();v=subprocess.Popen(['Xvfb','-displayfd',str(wr),'-noreset','-screen','0','3840x2160x24','-listen','tcp','-nolisten','unix','-nolisten','local'],env=e,pass_fds=(wr,),stdout=(o/'xvfb.log').open('w'),stderr=subprocess.STDOUT);os.close(wr)
try:
 with os.fdopen(rd) as f:assert select.select([f],[],[],10)[0];e['DISPLAY']='127.0.0.1:'+f.readline().strip()
 d=x.XOpenDisplay(e['DISPLAY'].encode());assert d
 # Preregister drags may grab the server; keep only this isolated test controller impervious.
 t.XTestGrabControl(d,1);x.XFlush(d)
 def state(path):
  try:return json.loads(path.read_text())
  except (OSError,ValueError):return None
 def wait(path,condition=lambda s:True,timeout=8):
  end=time.monotonic()+timeout
  while time.monotonic()<end:
   s=state(path)
   if s and condition(s):return s
   time.sleep(.03)
  raise AssertionError('state timeout: '+str(state(path)))
 def move(xp,yp):t.XTestFakeMotionEvent(d,0,int(xp),int(yp),0);x.XFlush(d);time.sleep(.06)
 def button(down,num=1):t.XTestFakeButtonEvent(d,num,down,0);x.XFlush(d);time.sleep(.08)
 def click(point):move(*point);button(1);button(0);time.sleep(.1)
 def key(name):
  c=x.XKeysymToKeycode(d,x.XStringToKeysym(name.encode()));assert c;t.XTestFakeKeyEvent(d,c,1,0);t.XTestFakeKeyEvent(d,c,0,0);x.XFlush(d);time.sleep(.12)
 def count(s):return [sum(not g['empty'] for g in a['gates']) for a in s['airports']]
 def gate(s,a,i):
  p=s['airports'][a];g=p['gates'][i];return p['x']+g['x']+g['size']//2,p['y']+g['y']+g['size']//2
 def capture(s,name):
  q=o/(name+'.xwd');subprocess.run(['xwd','-silent','-id',str(s['shell']),'-out',str(q)],env=e,check=True)
  p=subprocess.Popen(['xwdtopnm',str(q)],env=e,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
  with (o/(name+'.png')).open('wb') as f:subprocess.run(['pnmtopng'],env=e,stdin=p.stdout,stdout=f,check=True)
  p.stdout.close();assert p.wait()==0;q.unlink()
 cases=[(p,s,'comfortable',False) for p in ['modern-light','modern-dark'] for s in [1,1.5,2]]+[('classic',1,'comfortable',False),('modern-dark',1.5,'compact',False)]
 if a.gallery:cases += [('modern-light',None,'comfortable',True),('modern-light',1.5,'comfortable',True),('modern-dark',2,'comfortable',True),('classic',1,'comfortable',True),('modern-dark',1.5,'compact',True)]
 for profile,scale,density,via_gallery in cases:
  tag=('gallery-' if via_gallery else '')+profile+'-'+str(scale)+'-'+density;report=o/(tag+'.json');case_env=dict(e)
  if via_gallery:
   command=[str(a.gallery.resolve()),'--open-demo','airport','--appearance',profile,'-xrm','ckCoreMotif.appearanceDensity: '+density]
   if scale is not None:command+=['--scale',str(scale)]
   case_env['XM_AIRPORT_STATE_REPORT']=str(report)
  else:command=[str(exe),'--state-report',str(report),'-xrm','ckCoreMotif.appearanceProfile: '+profile,'-xrm','ckCoreMotif.appearanceDensity: '+density,'-xrm','Xft.dpi: '+str(scale*96),'-geometry','+40+40']
  p=subprocess.Popen(command,env=case_env,stdout=(o/(tag+'.log')).open('w'),stderr=subprocess.STDOUT);procs.append(p)
  s=wait(report,lambda s:count(s)==[4,4]);assert s['profile']=={'classic':0,'modern-light':1,'modern-dark':3}[profile] and abs(s['scale']-(scale or 1))<.001 and s['density']==(1 if density=='compact' else 0)
  print(tag,'initial',count(s),flush=True);capture(s,tag)
  if platform.system()=='Darwin':
   mapped=subprocess.check_output(['lsof','-a','-p',str(s['pid']),'-d','txt','-Fn'],text=True);(o/(tag+'-mapped.log')).write_text(mapped);assert any(Path(line[1:]).resolve()==library.resolve() for line in mapped.splitlines() if line.startswith('n/'))
  first_number=s['airports'][0]['gates'][0]['number']
  if via_gallery:children.append(s['pid'])
  click(gate(s,0,0));s=wait(report,lambda s:s['airports'][0]['selected']==0);assert count(s)==[4,4]
  key('Return');s=wait(report,lambda s:count(s)==[3,5]);assert any(g['number']==first_number and not g['empty'] for g in s['airports'][1]['gates'])
  key('Right');key('Return');s=wait(report,lambda s:count(s)==[2,6]);click(s['buttons']['reset']);s=wait(report,lambda s:count(s)==[4,4])
  click(gate(s,0,1));click(s['buttons']['flyA']);s=wait(report,lambda s:count(s)==[3,5]);click(s['buttons']['reset']);s=wait(report,lambda s:count(s)==[4,4])
  # Real server input starts and completes Motif's selection-based move.
  origin=gate(s,0,0);dest=s['airports'][1];target=(dest['x']+dest['width']//2,dest['y']+dest['height']//2)
  move(*origin);button(1);move(origin[0]+12,origin[1]+12);s=wait(report,lambda s:s['airports'][0]['dragging']);print(tag,'drag started',flush=True)
  for j in range(1,11):move(origin[0]+(target[0]-origin[0])*j/10,origin[1]+(target[1]-origin[1])*j/10)
  button(0);s=wait(report,lambda s:count(s)==[3,5] and not s['airports'][0]['dragging']);print(tag,'drop',count(s),flush=True)
  click(s['buttons']['reset']);s=wait(report,lambda s:count(s)==[4,4]);origin=gate(s,0,0);move(*origin);button(1);move(origin[0]+12,origin[1]+12);wait(report,lambda s:s['airports'][0]['dragging']);key('Escape');button(0);s=wait(report,lambda s:not s['airports'][0]['dragging']);assert count(s)==[4,4]
  before=[[g['number'] for g in a['gates']] for a in s['airports']];x.XResizeWindow(d,s['shell'],int(900*(scale or 1)),int(650*(scale or 1)));x.XFlush(d);time.sleep(.3);s=wait(report);assert before==[[g['number'] for g in a['gates']] for a in s['airports']]
  click(s['buttons']['help']);s=wait(report,lambda s:s['help_visible']);click(s['buttons']['helpOk']);s=wait(report,lambda s:not s['help_visible'])
  # Invalid drops return the original plane without changing gate ownership.
  origin=gate(s,0,0);move(*origin);button(1);move(origin[0]+12,origin[1]+12);wait(report,lambda s:s['airports'][0]['dragging']);move(3800,2100);button(0);s=wait(report,lambda s:not s['airports'][0]['dragging']);assert count(s)==[4,4]
  r['cases'].append({'profile':profile,'scale':scale or 'automatic','density':density,'via_gallery':via_gallery,'selection_keyboard_buttons_drag_drop_cancel_invalid_resize_help_close':'pass'});(o/'receipt.json').write_text(json.dumps(r,indent=2));click(s['buttons']['close'])
  if via_gallery:
   end=time.monotonic()+5
   while time.monotonic()<end and subprocess.run(['kill','-0',str(s['pid'])],capture_output=True).returncode==0:time.sleep(.05)
   assert subprocess.run(['kill','-0',str(s['pid'])],capture_output=True).returncode!=0
   p.terminate();p.wait(timeout=5)
  else:assert p.wait(timeout=5)==0
 # A separate process proves the 32-bit FLIGHT selection transfer and full-site rejection.
 report=o/'capacity.json';other_report=o/'external.json'
 def launch_extra(path,xpos):
  q=subprocess.Popen([str(exe),'--state-report',str(path),'-xrm','ckCoreMotif.appearanceProfile: modern-light','-xrm','Xft.dpi: 96','-geometry','+'+str(xpos)+'+40'],env=e,stdout=(path.with_suffix('.log')).open('w'),stderr=subprocess.STDOUT);procs.append(q);return q
 first=launch_extra(report,40);s=wait(report,lambda s:count(s)==[4,4])
 for i in range(4):click(gate(s,0,i));click(s['buttons']['flyA']);s=wait(report,lambda s:count(s)==[3-i,5+i])
 second=launch_extra(other_report,1200);other=wait(other_report,lambda s:count(s)==[4,4]);external_number=other['airports'][0]['gates'][0]['number'];origin=gate(other,0,0);target=s['airports'][1];destination=(target['x']+target['width']//2,target['y']+target['height']//2)
 def external_drag():
  move(*origin);button(1);move(origin[0]+12,origin[1]+12);wait(other_report,lambda z:z['airports'][0]['dragging'])
  for i in range(1,11):move(origin[0]+(destination[0]-origin[0])*i/10,origin[1]+(destination[1]-origin[1])*i/10)
  button(0);return wait(other_report,lambda z:not z['airports'][0]['dragging'])
 other=external_drag();assert count(other)==[4,4] and count(wait(report))==[0,8]
 click(s['buttons']['flyB']);s=wait(report,lambda z:count(z)==[1,7]);other=external_drag();assert count(other)==[3,4];s=wait(report,lambda z:count(z)==[1,8]);assert any(g['number']==external_number and not g['empty'] for g in s['airports'][1]['gates'])
 r['cross_process_full_rejection_and_transfer']='pass'
 assert sha(exe)==r['binary_sha256'] and sha(library)==r['library_sha256']
 r['success']=True
finally:
 for pid in children:
  q=subprocess.run(['ps','-p',str(pid),'-o','comm='],capture_output=True,text=True)
  if q.returncode==0 and q.stdout.strip()==str(prefix/'share/Xm/airport/airport'):
   import signal
   os.kill(pid,signal.SIGTERM)
 for p in procs:
  if p.poll() is None:p.terminate();p.wait()
 v.terminate();v.wait();r['ended_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();(o/'receipt.json').write_text(json.dumps(r,indent=2))
