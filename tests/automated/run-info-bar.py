#!/usr/bin/env python3
"""Build and run the installed XmInfoBar contract fixture."""
import hashlib, json, os, pathlib, platform, subprocess, sys, time
if len(sys.argv) != 3: raise SystemExit("usage: run-info-bar.py PREFIX OUTPUT_DIR")
prefix=pathlib.Path(sys.argv[1]).resolve(); out=pathlib.Path(sys.argv[2]).resolve(); out.mkdir(parents=True,exist_ok=True); source=pathlib.Path(__file__).with_name("check-info-bar.c").resolve(); binary=out/"check-info-bar"; env=os.environ.copy(); env["TMPDIR"]=str(out/"tmp"); (out/"tmp").mkdir(exist_ok=True); env["LD_LIBRARY_PATH"]=f"{prefix/'lib'}:{env.get('LD_LIBRARY_PATH','')}"; r={"started_utc":time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime()),"prefix":str(prefix),"architecture":platform.machine(),"source_sha256":hashlib.sha256(source.read_bytes()).hexdigest(),"steps":[]}
cmd=["gcc","-std=c11","-Wall","-Wextra","-Werror","-g","-O2",f"-I{prefix/'include'}",f"-L{prefix/'lib'}",f"-Wl,-rpath,{prefix/'lib'}",str(source),"-lXm","-lXt","-lX11","-o",str(binary)]
with (out/"compile.log").open("w") as log: result=subprocess.run(cmd,env=env,stdout=log,stderr=subprocess.STDOUT)
r["steps"].append({"name":"compile","command":cmd,"exit":result.returncode}); print(f"compile {result.returncode}")
if result.returncode: r["success"]=False; r["ended_utc"]=time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime()); (out/"receipt.json").write_text(json.dumps(r,indent=2)+"\n"); raise SystemExit(result.returncode)
run=["xvfb-run","-a",str(binary)]
with (out/"fixture.log").open("w") as log: result=subprocess.run(run,env=env,stdout=log,stderr=subprocess.STDOUT)
r["steps"].append({"name":"fixture","command":run,"exit":result.returncode}); print(f"fixture {result.returncode}"); r["success"]=result.returncode==0; r["ended_utc"]=time.strftime("%Y-%m-%dT%H:%M:%SZ",time.gmtime()); (out/"receipt.json").write_text(json.dumps(r,indent=2)+"\n"); raise SystemExit(result.returncode)
