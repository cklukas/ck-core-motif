#!/usr/bin/env python3
"""Run the installed-library WP-03 modern-dark profile test."""
import argparse
import os
from pathlib import Path
import platform
import subprocess
import sys


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
if platform.system() != "Linux":
    parser.error("WP-03 currently requires Linux/Xvfb")
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
if not prefix.is_absolute() or not output.is_absolute() or output.exists():
    parser.error("--out must be a fresh absolute directory")
source = Path(__file__).resolve().parent / "check-modern-dark-profile.c"
output.mkdir(parents=True)
(output / "tmp").mkdir()
env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
           LC_ALL="C.UTF-8", TMPDIR=str(output / "tmp"))
for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
    env.pop(name, None)
binary = output / "check-modern-dark-profile"
compile_log = output / "compile.log"
compile_command = ["cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
                   "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
                   "-Wl,-rpath," + str(prefix / "lib"), str(source), "-lXm",
                   "-lXt", "-lX11", "-o", str(binary)]
with compile_log.open("w") as stream:
    if subprocess.run(compile_command, env=env, stdout=stream,
                      stderr=subprocess.STDOUT).returncode:
        print(compile_log.read_text(errors="replace"), file=sys.stderr)
        raise SystemExit(1)
run_log = output / "run.log"
with run_log.open("w") as stream:
    result = subprocess.run(["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                             str(binary)], env=env, stdout=stream,
                            stderr=subprocess.STDOUT)
if result.returncode:
    print(run_log.read_text(errors="replace"), file=sys.stderr)
    raise SystemExit(result.returncode)
result = run_log.read_text(errors="replace")
if "PASS WP-03 modern-dark profile" not in result:
    print(result, file=sys.stderr)
    raise SystemExit(1)
print(result, end="")
