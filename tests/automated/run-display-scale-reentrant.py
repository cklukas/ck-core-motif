#!/usr/bin/env python3
"""Run the installed display-scale transaction reentrancy fixture."""

import argparse
import os
from pathlib import Path
import platform
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("WP-05.2 currently requires Linux/Xvfb")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
source_root = Path(__file__).resolve().parents[2]
source = source_root / "tests/automated/check-display-scale-reentrant.c"
if not prefix.is_absolute() or not output.is_absolute() or output.exists():
    parser.error("--prefix must be absolute and --out must be fresh")
output.mkdir(parents=True)
(output / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"))
for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
    env.pop(name, None)
binary = output / "check-display-scale-reentrant"
command = [
    "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
    "-I" + str(prefix / "include"),
    "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
    str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary),
]
with (output / "compile.log").open("w") as stream:
    subprocess.run(command, env=env, check=True, stdout=stream,
                   stderr=subprocess.STDOUT)
with (output / "run.log").open("w") as stream:
    subprocess.run(["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                    str(binary)], env=env, check=True, stdout=stream,
                   stderr=subprocess.STDOUT)
result = (output / "run.log").read_text(errors="replace")
if "PASS WP-05.2 display-scale reentrancy" not in result:
    raise RuntimeError("fixture did not report its pass marker")
print(result, end="")
