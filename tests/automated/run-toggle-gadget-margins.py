#!/usr/bin/env python3
"""Validate ToggleButtonGadget horizontal margins against an out-of-tree build."""
import argparse
import os
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[2]
build = args.build.resolve(strict=True)
output = args.out
if not output.is_absolute() or output.exists():
    parser.error("--out must be a fresh absolute task directory")
output.mkdir(parents=True)
env = dict(os.environ, TMPDIR=str(output))
binary = output / "check-toggle-gadget-margins"
command = ["cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
           "-I" + str(build / "lib"), "-I" + str(root / "lib"),
           str(root / "tests/automated/check-toggle-gadget-margins.c"),
           "-L" + str(build / "lib/Xm/.libs"),
           "-Wl,-rpath," + str(build / "lib/Xm/.libs"),
           "-lXm", "-lXt", "-lX11", "-o", str(binary)]
with (output / "compile.log").open("w") as log:
    subprocess.run(command, env=env, stdout=log, stderr=subprocess.STDOUT, check=True)
with (output / "run.log").open("w") as log:
    subprocess.run(["xvfb-run", "-a", str(binary)], env=env,
                   stdout=log, stderr=subprocess.STDOUT, check=True)
print((output / "run.log").read_text(), end="")
