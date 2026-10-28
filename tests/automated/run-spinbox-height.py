#!/usr/bin/env python3
"""Compile and run the constrained-height SpinBox layout regression."""

from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path
import subprocess


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
    parser.error("--out must be fresh and separate from --prefix")
output.mkdir(parents=True)
source = Path(__file__).with_name("check-spinbox-height.c").resolve()
env = dict(os.environ, TMPDIR=str(output), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"))
binary = output / "check-spinbox-height"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
         "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
         "-Wl,-rpath," + str(prefix / "lib")]
subprocess.run(["cc", *flags, str(source), "-lXm", "-lXt", "-lX11",
                "-o", str(binary)], cwd=output, env=env, check=True)
link = subprocess.run(["ldd", str(binary)], cwd=output, env=env,
                      check=True, capture_output=True, text=True)
if str(prefix / "lib") not in link.stdout:
    raise RuntimeError("regression did not resolve the selected Motif prefix")
run = subprocess.run(["xvfb-run", "-a", "-s", "-screen 0 1600x1200x24",
                      str(binary)], cwd=output, env=env,
                     check=True, capture_output=True, text=True)
if "PASS spinbox height" not in run.stdout:
    raise RuntimeError("regression did not report its pass marker")
print("PASS spinbox height")
print("source_sha256=" + hashlib.sha256(source.read_bytes()).hexdigest())
