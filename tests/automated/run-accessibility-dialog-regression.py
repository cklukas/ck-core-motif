#!/usr/bin/env python3
"""Compile and run the native accessibility dialog journey under Xvfb."""
import os
import subprocess
import sys
from pathlib import Path

if len(sys.argv) != 3:
    raise SystemExit("usage: run-accessibility-dialog-regression.py PREFIX RESULTS")
prefix = Path(sys.argv[1]).resolve()
results = Path(sys.argv[2]).resolve()
results.mkdir(parents=True, exist_ok=False)
(results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(results / "tmp"),
           LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(prefix / "lib"))
source = Path(__file__).with_name("check-accessibility-dialog.c")
binary = results / "check-accessibility-dialog"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
         "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
         "-Wl,-rpath," + str(prefix / "lib")]
compile_result = subprocess.run(
    ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
(results / "compile.log").write_text(compile_result.stdout)
if compile_result.returncode:
    raise SystemExit(compile_result.returncode)
run_result = subprocess.run(["xvfb-run", "-a", str(binary)], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True)
(results / "run.log").write_text(run_result.stdout)
if run_result.returncode:
    raise SystemExit(run_result.returncode)
print(run_result.stdout, end="")
