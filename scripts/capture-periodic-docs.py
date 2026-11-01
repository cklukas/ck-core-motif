#!/usr/bin/env python3
"""Capture actual Periodic windows from a fingerprint-verified Linux build."""
from pathlib import Path
import argparse, os, subprocess, json, hashlib, time, select, re, platform, struct
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build", type=Path, required=True)
parser.add_argument("--record", type=Path, required=True)
parser.add_argument("--out", type=Path, required=True)
parser.add_argument("--logical-viewport", type=int, nargs=2, metavar=("WIDTH", "HEIGHT"),
                    default=(1440, 1200), help="reference viewport at 100%%; scales with DPI")
args = parser.parse_args()
if any(value < 320 or value > 4096 for value in args.logical_viewport):
    parser.error("logical viewport dimensions must be between 320 and 4096")
if platform.system() != "Linux": parser.error("capture requires Linux, Xvfb and the native Periodic executable")
b = args.build.resolve(strict=True)
out = args.out
if not out.is_absolute() or out.exists(): parser.error("--out must be a fresh absolute directory")
record = json.loads(args.record.read_text())
if not record["success"]: parser.error("build record did not succeed")
for path, digest in {**record["outputs"], **record["sources"]}.items():
    if hashlib.sha256(Path(path).read_bytes()).hexdigest() != digest:
        parser.error("build fingerprint changed: " + path)
exe = b / "demos/programs/periodic/.libs/periodic"
lib = (b / "lib/Xm/.libs/libXm.so").resolve(strict=True)
if str(exe) not in record["outputs"] or str(lib) not in record["outputs"]:
    parser.error("build record does not identify the requested executable/library")
out.mkdir(parents=True)
(out / "tmp").mkdir()
for p in Path('/proc').iterdir():
 if p.name.isdigit():
  try:name=(p/'comm').read_text().strip()
  except OSError:continue
  if name in ('periodic','ckmotif-gallery'):raise RuntimeError('Existing project app '+p.name)
env=dict(os.environ,TMPDIR=str(out/'tmp'),XDG_CACHE_HOME=str(out/'tmp/cache'),LD_LIBRARY_PATH=str(b/'lib/Mrm/.libs')+':'+str(b/'lib/Xm/.libs'),XFILESEARCHPATH=str(out/'%N'),XUSERFILESEARCHPATH=str(out/'%N'))
for k in ('LD_PRELOAD','LD_AUDIT','XENVIRONMENT','XAPPLRESDIR'):env.pop(k,None)
a,c=os.pipe();server=subprocess.Popen(['Xvfb','-displayfd',str(c),'-screen','0',f'{int(args.logical_viewport[0]*1.5)+100}x{int(args.logical_viewport[1]*1.5)+100}x24','-nolisten','tcp','-ac'],env=env,pass_fds=(c,),stdout=(out/'xserver.log').open('w'),stderr=subprocess.STDOUT);os.close(c)
ready_bytes=b'';deadline=time.monotonic()+15
while not ready_bytes.endswith(b'\n'):
 remaining=deadline-time.monotonic();assert remaining>0 and select.select([a],[],[],remaining)[0]
 chunk=os.read(a,64);assert chunk;ready_bytes+=chunk
env['DISPLAY']=':'+ready_bytes.decode().strip();os.close(a)
wm=None;app=None;captures=[]
try:
 wm=subprocess.Popen(['openbox','--sm-disable'],env=env,stdout=(out/'wm.log').open('w'),stderr=subprocess.STDOUT)
 for variant, profile, dpi, density in [('classic','classic',96,'comfortable'), ('modern','modern-light',96,'comfortable'), ('modern-scaled','modern-light',144,'comfortable'), ('modern-dark','modern-dark',96,'comfortable'), ('modern-compact','modern-light',96,'compact')]:
  viewport=[round(value*dpi/96) for value in args.logical_viewport]
  app=subprocess.Popen([str(exe),'--appearance',profile,'--density',density,'-xrm',f'Xft.dpi: {dpi}','-geometry',f'{viewport[0]}x{viewport[1]}+20+20'],cwd=b/'demos/programs/periodic',env=env,stdout=(out/(variant+'.log')).open('w'),stderr=subprocess.STDOUT)
  window=None;ready=False
  for i in range(100):
   if app.poll() is not None:raise RuntimeError('Application exited')
   query=subprocess.run(['xwininfo','-root','-tree'],env=env,text=True,capture_output=True)
   if query.returncode:time.sleep(.1);continue
   tree=query.stdout
   for line in tree.splitlines():
    if '"XmdPeriodicModern"' not in line:continue
    candidate=re.search(r'0x[0-9a-f]+',line).group()
    prop=subprocess.check_output(['xprop','-id',candidate,'_NET_WM_PID'],env=env,text=True)
    if prop.strip().endswith('= '+str(app.pid)):window=candidate;ready=True;break
   if ready:break
   time.sleep(.1)
  (out/(variant+'-tree.txt')).write_text(tree)
  assert ready, 'No realized PID window'
  assert Path('/proc/'+str(app.pid)+'/exe').resolve()==exe, 'Wrong executable'
  maps=Path('/proc/'+str(app.pid)+'/maps').read_text();(out/(variant+'-maps.txt')).write_text(maps);assert str(lib) in maps, 'Wrong library'
  time.sleep(.5)
  xwd=out/(variant+'.xwd');subprocess.run(['xwd','-silent','-id',window,'-out',str(xwd)],env=env,check=True)
  pnm=subprocess.check_output(['xwdtopnm',str(xwd)],stderr=subprocess.DEVNULL);png=subprocess.check_output(['pnmtopng'],input=pnm,stderr=subprocess.DEVNULL);(out/(variant+'.png')).write_bytes(png);xwd.unlink()
  captures.append({'variant':variant,'profile':profile,'dpi':dpi,'scale':dpi/96,'density':density,'viewport':viewport,'logical_viewport':list(args.logical_viewport),'pid_verified':True,'executable':'demos/programs/periodic/.libs/periodic','binary_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'library_sha256':hashlib.sha256(lib.read_bytes()).hexdigest(),'uid_sha256':hashlib.sha256((b/'demos/programs/periodic/periodic.uid').read_bytes()).hexdigest(),'png':variant+'.png','dimensions':list(struct.unpack('>II',png[16:24])),'sha256':hashlib.sha256(png).hexdigest()})
  app.terminate();app.wait(timeout=5);app=None
finally:
 if app and app.poll() is None:app.terminate();app.wait(timeout=5)
 if wm:wm.terminate();wm.wait(timeout=5)
 server.terminate();server.wait(timeout=5)
 (out/'manifest.json').write_text(json.dumps({'schema':1,'kind':'development snapshot','entries':captures},indent=2)+'\n');print(json.dumps(captures,indent=2))
