#!/usr/bin/env python3
"""Compile and run the XmString-table converter regression."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error("prefix and fresh results directory must be absolute")
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"),
           LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(args.prefix / "lib"))
source = Path(__file__).resolve().parent / "check-xmstring-table-conversion.c"
binary = args.results / "check-xmstring-table-conversion"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
         "-I" + str(args.prefix / "include"), "-L" + str(args.prefix / "lib"),
         "-Wl,-rpath," + str(args.prefix / "lib")]
result = subprocess.run(
    ["gcc", *flags, str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)],
    env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
    check=False)
(args.results / "compile.log").write_text(result.stdout)
if result.returncode:
    raise SystemExit(result.returncode)
result = subprocess.run(["xvfb-run", "-a", str(binary)], env=env,
                        stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                        text=True, check=False)
(args.results / "run.log").write_text(result.stdout)
if result.returncode:
    raise SystemExit(result.returncode)
print(result.stdout, end="")
