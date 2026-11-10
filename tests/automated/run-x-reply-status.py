#!/usr/bin/env python3
"""Compile check-x-reply-status.c against an installed prefix and run every
case on a virtual X display.

    run-x-reply-status.py PREFIX RESULTS

PREFIX is the installed toolkit (build and run it and this program with the
same sanitizer list); RESULTS is a fresh absolute directory.
"""
import argparse
import os
from pathlib import Path
import subprocess

CASES = ["cascade", "cascade-gadget", "datafield-geometry", "datafield-image",
         "icon-button", "visibility", "viewable", "receiver-info", "drag-start"]

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
lib = args.prefix / "lib"
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(lib),
           ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=1",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
here = Path(__file__).resolve().parent
binary = args.results / "check-x-reply-status"
sanitize = os.environ.get("MOTIF_TEST_SANITIZE", "address,undefined")
flags = ["-std=gnu17", "-Wall", "-Wextra", "-Wno-unused-parameter", "-g", "-O1",
         "-fno-omit-frame-pointer", "-I" + str(args.prefix / "include"),
         "-L" + str(lib), "-Wl,-rpath," + str(lib)]
if sanitize:
    flags += ["-fsanitize=" + sanitize, "-fno-sanitize-recover=undefined"]
result = subprocess.run(
    ["gcc", *flags, str(here / "check-x-reply-status.c"),
     "-lXm", "-lXt", "-lX11", "-ldl", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    print(result.stdout, end="")
    raise SystemExit(result.returncode)
status = 0
for case in CASES:
    result = subprocess.run(["xvfb-run", "-a", str(binary), case], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, check=False)
    (args.results / (case + ".log")).write_text(result.stdout)
    print(result.stdout, end="")
    if result.returncode:
        print(f"{case}: exit status {result.returncode}")
        status = 1
raise SystemExit(status)
