#!/usr/bin/env python3
"""Run the installed ScrolledWindow display-scale fixture."""

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
if not prefix.is_absolute() or not output.is_absolute() or output.exists():
    parser.error("--prefix must be absolute and --out must be fresh")
output.mkdir(parents=True)
(output / "tmp").mkdir()
env = dict(os.environ, TMPDIR=str(output / "tmp"), LC_ALL="C.UTF-8",
           LD_LIBRARY_PATH=str(prefix / "lib"),
           PYTHONDONTWRITEBYTECODE="1")
for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
    env.pop(name, None)
source = Path(__file__).resolve().parent / "check-display-scale-scrolled.c"
binary = output / "check-display-scale-scrolled"
flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
         "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
         "-Wl,-rpath," + str(prefix / "lib")]
subprocess.run(["cc", *flags, str(source), "-lXm", "-lXt", "-lX11",
                "-o", str(binary)], env=env, check=True)
result = subprocess.run(
    ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)],
    env=env, check=True, capture_output=True, text=True)
print(result.stdout, end="")
if "PASS WP-05.2 display-scale scrolled window" not in result.stdout:
    raise RuntimeError("ScrolledWindow fixture did not report its pass marker")
print("PASS WP-05.2 display-scale scrolled window")
