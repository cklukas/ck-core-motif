#!/usr/bin/env python3
"""Compile lib/Xm/EditresCom.c into check-editres-wire-protocol.c under
AddressSanitizer and UndefinedBehaviorSanitizer, and run it on a virtual X
display.

    run-editres-wire-protocol.py RESULTS [--source-dir DIR] [--control]

RESULTS is a fresh absolute directory.  --source-dir names the directory that
holds EditresCom.c and EditresComI.h (default: lib/Xm of this checkout); it is
how an earlier revision of the files is tested.  --control builds the test for
that earlier implementation, which relied on the protocol helpers of libXmu.
"""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("results", type=Path)
parser.add_argument("--source-dir", type=Path)
parser.add_argument("--control", action="store_true")
args = parser.parse_args()
if not args.results.is_absolute():
    parser.error("the results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
here = Path(__file__).resolve().parent
source_dir = (args.source_dir or here.parents[1] / "lib" / "Xm").resolve()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           ASAN_OPTIONS="detect_leaks=1:abort_on_error=0:halt_on_error=1",
           UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
binary = args.results / "check-editres-wire-protocol"
flags = ["-std=gnu17", "-g", "-O1", "-fno-omit-frame-pointer",
         "-fsanitize=address,undefined", "-fno-sanitize-recover=undefined",
         "-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-function",
         "-I" + str(source_dir)]
if args.control:
    flags.append("-DEDITRES_PREFIX_CONTROL")
result = subprocess.run(
    ["gcc", *flags, str(here / "check-editres-wire-protocol.c"),
     "-lXmu", "-lXt", "-lX11", "-o", str(binary)],
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
if result.returncode:
    raise SystemExit(result.returncode)
