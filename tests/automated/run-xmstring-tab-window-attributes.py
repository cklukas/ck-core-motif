#!/usr/bin/env python3
"""Compile check-xmstring-tab-window-attributes.c against an installed prefix
and run it on a virtual X display.

    run-xmstring-tab-window-attributes.py PREFIX RESULTS

PREFIX is the installed toolkit (build and run it and this program with the
same sanitizer list); RESULTS is a fresh absolute directory.  The program
replaces XGetWindowAttributes at link time, so it runs on Linux only.
"""
import argparse
import os
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
if sys.platform != "linux":
    parser.error("the test interposes Xlib through ELF symbol lookup: run it on Linux")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
lib = args.prefix / "lib"
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(lib),
           ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=1",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
here = Path(__file__).resolve().parent
binary = args.results / "check-xmstring-tab-window-attributes"
sanitize = os.environ.get("MOTIF_TEST_SANITIZE", "address,undefined")
flags = ["-std=gnu17", "-Wall", "-Wextra", "-Wno-unused-parameter", "-g", "-O1",
         "-fno-omit-frame-pointer", "-I" + str(args.prefix / "include"),
         "-L" + str(lib), "-Wl,-rpath," + str(lib)]
if sanitize:
    flags += ["-fsanitize=" + sanitize, "-fno-sanitize-recover=undefined"]
result = subprocess.run(
    ["gcc", *flags, str(here / "check-xmstring-tab-window-attributes.c"),
     "-lXm", "-lXt", "-lX11", "-ldl", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    print(result.stdout, end="")
    raise SystemExit(result.returncode)
result = subprocess.run(["xvfb-run", "-a", str(binary)], env=env,
                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                        text=True, check=False)
(args.results / "run.log").write_text(result.stdout)
print(result.stdout, end="")
raise SystemExit(result.returncode)
