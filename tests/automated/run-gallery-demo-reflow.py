from pathlib import Path
import os,subprocess,select,importlib.util,json,hashlib,shutil
import argparse,platform
from PIL import Image
parser=argparse.ArgumentParser(description="Check visible demo descriptions after external resize on isolated Xvfb.")
parser.add_argument('--prefix',type=Path,required=True)
parser.add_argument('--binary',type=Path)
parser.add_argument('--out',type=Path,required=True)
a=parser.parse_args();prefix=a.prefix.resolve(strict=True);binary=(a.binary or prefix/'bin/ckmotif-gallery').resolve(strict=True);o=a.out.resolve()
if o.exists():parser.error('--out must be fresh')
if platform.system()=='Darwin' and not any(base.parent.is_mount() and o.is_relative_to(base) for base in [Path('/Volumes/S8T/tmp'),Path('/Volumes/PRO-BLADE/tmp')]):parser.error('use a mounted Mac temporary volume')
o.mkdir(parents=True);(o/'tmp').mkdir();e=dict(os.environ,TMPDIR=str(o/'tmp'),DYLD_LIBRARY_PATH=str(prefix/'lib'),LD_LIBRARY_PATH=str(prefix/'lib'),PATH='/opt/X11/bin:/opt/homebrew/bin:'+os.environ['PATH'],PYTHONDONTWRITEBYTECODE='1');spec=importlib.util.spec_from_file_location('gallery_capture',Path(__file__).with_name('run-gallery-screenshots.py'));m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m);rd,wr=os.pipe();v=subprocess.Popen(['Xvfb','-displayfd',str(wr),'-noreset','-screen','0','1920x1440x24','-listen','tcp','-nolisten','unix','-nolisten','local'],env=e,pass_fds=(wr,),stdout=(o/'xvfb.log').open('w'),stderr=subprocess.STDOUT);os.close(wr);rows=[]
try:
 with os.fdopen(rd) as f:assert select.select([f],[],[],10)[0];e['DISPLAY']='127.0.0.1:'+f.readline().strip()
 for name,geometry in [('desktop','1100x740'),('compact','450x500')]:
  p=subprocess.Popen([str(binary),'--appearance','modern-light','--scale','1','--component','gallery.demos'],env=e,stdout=(o/(name+'.log')).open('w'),stderr=subprocess.STDOUT)
  try:
   import time,re
   for attempt in range(100):
    assert p.poll() is None,(o/(name+'.log')).read_text()
    tree=subprocess.check_output(['xwininfo','-root','-tree'],env=e,text=True)
    found=[match.group(1) for line in tree.splitlines() if (match:=m.WINDOW_RE.match(line)) and int(match.group(2))>100 and int(match.group(3))>100]
    if len(found)==1:
     window=found[0];info=subprocess.check_output(['xwininfo','-id',window],env=e,text=True)
     (o/(name+'-tree.log')).write_text(tree);(o/(name+'-window.log')).write_text(info)
     if 'Map State: IsViewable' in info:break
    time.sleep(.05)
   else:raise RuntimeError('owned gallery window not found')
   import ctypes as C
   x=C.CDLL('/opt/X11/lib/libX11.dylib' if platform.system()=='Darwin' else __import__('ctypes.util').util.find_library('X11'));x.XOpenDisplay.argtypes=[C.c_char_p];x.XOpenDisplay.restype=C.c_void_p;x.XResizeWindow.argtypes=[C.c_void_p,C.c_ulong,C.c_uint,C.c_uint];x.XFlush.argtypes=[C.c_void_p];x.XCloseDisplay.argtypes=[C.c_void_p]
   connection=x.XOpenDisplay(e['DISPLAY'].encode());width,height=map(int,geometry.split('x'));x.XResizeWindow(connection,int(window,16),width,height);x.XFlush(connection);time.sleep(.5);x.XCloseDisplay(connection)
   q=o/(name+'.xwd');subprocess.run(['xwd','-silent','-id',window,'-out',str(q)],env=e,check=True)
   ppm=subprocess.Popen(['xwdtopnm',str(q)],env=e,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL)
   png=o/(name+'.png')
   with png.open('wb') as f:subprocess.run(['pnmtopng'],stdin=ppm.stdout,stdout=f,env=e,check=True)
   ppm.stdout.close();assert ppm.wait()==0;q.unlink();rows.append({'name':name,'geometry':geometry,'sha256':hashlib.sha256(png.read_bytes()).hexdigest()})
  finally:p.terminate();p.communicate(timeout=5)
 # Explicit Mac/Xft reference viewport: these regions contain only wrapped intro/description text.
 # Reading pixels from an external client avoids changing the app's event scheduling.
 im=Image.open(o/'compact.png').convert('RGB')
 ink={name:sum(max(pixel)-min(pixel)<50 and max(pixel)<200 for pixel in im.crop(box).getdata())
      for name,box in [('intro',(85,110,405,185)),('description',(85,222,405,257))]}
 assert ink['intro']>500 and ink['description']>250,ink
 (o/'receipt.json').write_text(json.dumps({'success':True,'captures':rows,'gallery_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'ink':ink,'scope':'External resize and read-only pixel observation on isolated Xvfb, modern-light at scale 1'},indent=2))
finally:v.terminate();v.wait()
