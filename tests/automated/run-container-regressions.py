#!/usr/bin/env python3
"""Build and run the dynamic Container tab-list regression."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
assert args.prefix.is_absolute() and args.results.is_absolute()
args.results.mkdir(parents=True, exist_ok=False)
(args.results / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(args.results / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(args.prefix / "lib"))
source = Path(__file__).resolve().parent / "check-container-tabs.c"
binary = args.results / "check-container-tabs"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
         "-I" + str(args.prefix / "include"), "-L" + str(args.prefix / "lib"),
         "-Wl,-rpath," + str(args.prefix / "lib")]
compile_result = subprocess.run(
    ["gcc", *flags, source, "-lXm", "-lXt", "-lX11", "-o", binary],
    env=env, cwd=args.results, capture_output=True, timeout=60)
(args.results / "compile.log").write_bytes(compile_result.stdout + compile_result.stderr)
assert compile_result.returncode == 0
run_result = subprocess.run(["xvfb-run", "-a", binary], env=env,
                            cwd=args.results, capture_output=True, timeout=60)
(args.results / "run.log").write_bytes(run_result.stdout + run_result.stderr)
assert run_result.returncode == 0
print("PASS container tab-list ownership")
