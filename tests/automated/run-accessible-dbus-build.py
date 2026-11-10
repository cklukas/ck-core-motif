#!/usr/bin/env python3
"""Compile lib/Xm/AccessibleDBus.c into check-accessible-dbus-build.c under
AddressSanitizer and UndefinedBehaviorSanitizer and run it on a virtual X
display with a private session bus.

    run-accessible-dbus-build.py PREFIX RESULTS [--source-dir DIR]

PREFIX is an installed toolkit built with --enable-sanitizers and --enable-atspi
(the program is linked against it); RESULTS is a fresh absolute directory.
--source-dir names the directory that holds AccessibleDBus.c (default: lib/Xm
of this checkout), which is how an earlier revision of the file is tested.
"""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
parser.add_argument("--source-dir", type=Path)
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
lib = args.prefix / "lib"
here = Path(__file__).resolve().parent
repo_xm = here.parents[1] / "lib" / "Xm"
source_dir = (args.source_dir or repo_xm).resolve()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(lib),
           ASAN_OPTIONS="detect_leaks=0:abort_on_error=0:halt_on_error=1",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
binary = args.results / "check-accessible-dbus-build"
dbus = subprocess.run(["pkg-config", "--cflags", "--libs", "dbus-1"],
                      stdout=subprocess.PIPE, text=True, check=True).stdout.split()
flags = ["-std=gnu17", "-g", "-O1", "-fno-omit-frame-pointer",
         "-fsanitize=address,undefined", "-fno-sanitize-recover=undefined",
         "-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-function",
         "-I" + str(source_dir), "-I" + str(repo_xm),
         "-I" + str(args.prefix / "include"), "-L" + str(lib),
         "-Wl,-rpath," + str(lib)]
result = subprocess.run(
    ["gcc", *flags, str(here / "check-accessible-dbus-build.c"), *dbus,
     "-lXm", "-lXt", "-lX11", "-ldl", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    print(result.stdout, end="")
    raise SystemExit(result.returncode)
result = subprocess.run(["dbus-run-session", "--", "xvfb-run", "-a", str(binary)],
                        env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                        text=True, check=False)
(args.results / "run.log").write_text(result.stdout)
print(result.stdout, end="")
raise SystemExit(result.returncode)
