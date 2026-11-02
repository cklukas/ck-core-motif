#!/usr/bin/env python3
"""Build and test the Paint demo; preserve source hashes and timing evidence."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import select
import subprocess
import time
import shutil

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--prefix',type=Path,required=True)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--baseline',type=Path,help='Previous successful receipt for a comparable clean compile')
a=p.parse_args()
root=Path(__file__).resolve().parents[2]
if not a.out.is_absolute():
    p.error('--out must be a fresh absolute path on the selected scratch volume')
out=a.out.resolve()
a.prefix=a.prefix.resolve()
out.mkdir(parents=True,exist_ok=False)
(out/'tmp').mkdir()
env=dict(os.environ,TMPDIR=str(out/'tmp'),DYLD_LIBRARY_PATH=str(a.prefix/'lib'),LD_LIBRARY_PATH=str(a.prefix/'lib'))
receipt={'target':'draw; optimized standalone and sanitized raster/native regressions',
         'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
         'baseline':'No previous receipt supplied; all sources compiled explicitly without a build cache.',
         'sources':{},'commands':[],'success':False,'output':str(out/'draw')}
if a.baseline:
    previous=json.loads(a.baseline.read_text())
    assert previous['success'] and previous['target']==receipt['target']
    receipt['baseline']={'receipt':str(a.baseline),'compile_seconds':previous['commands'][0]['elapsed_seconds']}
for path in [root/'demos/programs/draw'/name for name in ['draw.c','paint.c','paint.h','paint-icons.c','paint-icons.h','README','draw.man','draw.help','XmdDraw.ad','Makefile.am']]+[Path(__file__),root/'tests/automated/check-paint.c',root/'tests/automated/check-paint-ui.c']:
    receipt['sources'][str(path.relative_to(root))]=hashlib.sha256(path.read_bytes()).hexdigest()
for path in list((a.prefix/'lib').glob('libXm.*dylib'))+list((a.prefix/'lib').glob('libXm.so*')):
    receipt.setdefault('libraries',{})[str(path)]=hashlib.sha256(path.read_bytes()).hexdigest()
def run(cmd):
    start=time.monotonic()
    r=subprocess.run(list(map(str,cmd)),cwd=out,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,timeout=90)
    receipt['commands'].append({'command':list(map(str,cmd)),'elapsed_seconds':time.monotonic()-start,'exit':r.returncode,'output':r.stdout})
    print(r.stdout,end='',flush=True)
    assert r.returncode==0,r.stdout
    assert 'Warning:' not in r.stdout,r.stdout
server=None
try:
    cc=['cc','-DPNG_SUPPORTED','-I/opt/X11/include','-L/opt/X11/lib','-std=c11','-D_POSIX_C_SOURCE=200809L','-Wall','-Wextra','-Werror']
    model=root/'demos/programs/draw/paint.c'
    includes=['-I'+str(a.prefix/'include'),'-I/opt/X11/include']
    libs=['-L'+str(a.prefix/'lib'),'-L/opt/X11/lib','-lXm','-lXt','-lX11','-lpng','-lm']
    run(cc+['-O2']+includes+[root/'demos/programs/draw/draw.c',model,root/'demos/programs/draw/paint-icons.c']+libs+['-o',out/'draw'])
    run(cc+['-fsanitize=address,undefined','-g',root/'tests/automated/check-paint.c',model,'-lpng','-lm','-o',out/'check-paint'])
    run([out/'check-paint',out/'tmp'])
    run([arg for arg in cc if arg not in ('-DPNG_SUPPORTED','-lpng')]+['-fsanitize=address,undefined','-g',root/'tests/automated/check-paint.c',model,'-lm','-o',out/'check-paint-bmp'])
    run([out/'check-paint-bmp',out/'tmp'])
    run(cc+['-g','-fsanitize=address,undefined']+includes+[root/'tests/automated/check-paint-ui.c',model,root/'demos/programs/draw/paint-icons.c']+libs+['-lXtst','-lXext']+['-o',out/'check-paint-ui'])
    rd,wr=os.pipe()
    with (out/'xvfb.log').open('w') as log:
        xvfb=shutil.which('Xvfb',path=env.get('PATH','')+os.pathsep+'/opt/X11/bin')
        if not xvfb: raise RuntimeError('Xvfb is required for native tooltip and drawing checks')
        server=subprocess.Popen([xvfb,'-displayfd',str(wr),'-screen','0','1400x1000x24','-listen','tcp','-nolisten','unix','-nolisten','local'],env=env,pass_fds=(wr,),stdout=log,stderr=log)
    os.close(wr)
    with os.fdopen(rd) as pipe:
        assert select.select([pipe],[],[],10)[0],'Xvfb startup timed out'
        env['DISPLAY']='127.0.0.1:'+pipe.readline().strip()
    run([out/'check-paint-ui'])
    run([out/'check-paint-ui','-xrm','ckCoreMotif.appearanceProfile: modern-light','-xrm','Xft.dpi: 144'])
    receipt['binary_sha256']=hashlib.sha256((out/'draw').read_bytes()).hexdigest()
    receipt['success']=True
finally:
    if server:
        server.terminate();server.wait(timeout=10)
    receipt['ended_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
