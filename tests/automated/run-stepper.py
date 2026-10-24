#!/usr/bin/env python3
"""Build and run the installed-prefix Stepper contract probe on Xvfb."""
import argparse, datetime, hashlib, json, os, platform, subprocess, sys
from pathlib import Path
def main():
    p = argparse.ArgumentParser(description=__doc__); p.add_argument("prefix", type=Path); p.add_argument("results", type=Path); a = p.parse_args()
    if platform.system() != "Linux" or not a.prefix.is_absolute() or not a.results.is_absolute(): p.error("use absolute Linux prefix and results paths")
    if a.results.exists(): p.error("results directory must be fresh")
    a.results.mkdir(parents=True); (a.results / "tmp").mkdir(); env = dict(os.environ, TMPDIR=str(a.results / "tmp"), LD_LIBRARY_PATH=str(a.prefix / "lib"), LC_ALL="C.UTF-8"); source = Path(__file__).resolve().parent / "check-stepper.c"; binary = a.results / "check-stepper"; flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2", "-I" + str(a.prefix / "include"), "-L" + str(a.prefix / "lib"), "-Wl,-rpath," + str(a.prefix / "lib")]; receipt = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(), "prefix": str(a.prefix), "architecture": platform.machine(), "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(), "steps": []}
    def run(name, command):
        r = subprocess.run(command, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT); (a.results / (name + ".log")).write_text(r.stdout); receipt["steps"].append({"name": name, "command": command, "exit": r.returncode}); print(name, r.returncode, flush=True); return r.returncode == 0
    ok = run("compile", ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)])
    if ok: ok = run("fixture", ["xvfb-run", "-a", str(binary)])
    receipt["success"] = ok; receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat(); (a.results / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n"); return 0 if ok else 1
if __name__ == "__main__": sys.exit(main())
