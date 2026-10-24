#!/usr/bin/env python3
"""Build and run the image-scaling and malformed-image native check."""
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
source_dir = Path(__file__).resolve().parent
flags = ["-Wall", "-Wextra", "-Werror", "-g", "-O2",
         "-I" + str(args.prefix / "include"), "-I" + str(source_dir.parent.parent / "lib/Xm"),
         "-L" + str(args.prefix / "lib"), "-Wl,-rpath," + str(args.prefix / "lib")]

def run(name, command):
    result = subprocess.run([str(item) for item in command], env=env,
                            cwd=args.results, capture_output=True, timeout=60)
    (args.results / (name + ".log")).write_bytes(result.stdout + result.stderr)
    assert result.returncode == 0, name

name = "image-scaling"
source = source_dir / ("check-" + name + ".c")
executable = args.results / ("check-" + name)
run("compile-" + name, ["gcc", *flags, source, "-lXm", "-lXt", "-lX11", "-o", executable])
run(name, ["xvfb-run", "-a", executable])
print("PASS image scaling and malformed-image regressions")
