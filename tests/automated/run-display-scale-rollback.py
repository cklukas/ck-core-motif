#!/usr/bin/env python3
"""Run the installed display-scale transaction rollback fixture."""

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
source = Path(__file__).resolve().parent / "check-display-scale-rollback.c"
fault_source = Path(__file__).resolve().parent / "check-display-scale-fault.c"
binary = output / "check-display-scale-rollback"
fault_library = output / "libcheck-display-scale-fault.so"
fault_command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2",
                 "-fPIC", "-shared", str(fault_source), "-o",
                 str(fault_library)]
command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
           "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
           "-Wl,-rpath," + str(prefix / "lib"),
           str(source), "-lXm", "-lXt", "-lX11", "-o", str(binary)]
with (output / "compile.log").open("w") as stream:
    subprocess.run(fault_command, env=env, check=True, stdout=stream,
                   stderr=subprocess.STDOUT)
    subprocess.run(command, env=env, check=True, stdout=stream,
                   stderr=subprocess.STDOUT)
with (output / "run.log").open("w") as stream:
    run_env = dict(env)
    subprocess.run(["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                    "env", "LD_PRELOAD=" + str(fault_library), str(binary)],
                   env=run_env, check=True, stdout=stream,
                   stderr=subprocess.STDOUT)
result = (output / "run.log").read_text(errors="replace")
if "PASS WP-05.2 display-scale rollback" not in result:
    raise RuntimeError("rollback fixture did not report its pass marker")
print(result, end="")
