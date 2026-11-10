#!/usr/bin/env python3
"""Compile every translation unit of MWM with the exact flags of a configured
build, without any CDE header on the include path.

    run-mwm-standalone-compile.py --build-root BUILD --out OUT

BUILD is a configured Motif build tree (its generated headers and its
clients/mwm/Makefile are used; nothing in it is modified). OUT is a fresh
directory for the logs. The standalone window manager supports its workspace
protocol through its own WmWsmLib, so no `<Dt/...>` header may be needed unless
the CDE-only `WSM` configuration is selected.
"""
import argparse
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build-root", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()
build = args.build_root.resolve(strict=True)
out = args.out.resolve()
if out.exists():
    parser.error("--out must be a fresh directory")
out.mkdir(parents=True)
directory = build / "clients" / "mwm"
if not (directory / "Makefile").is_file():
    parser.error(f"{directory}/Makefile not found: configure the build tree first")

plan = subprocess.run(["make", "-n", "-B", "-C", str(directory), "mwm"],
                      stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                      check=False)
(out / "make-n.log").write_text(plan.stdout)
if plan.returncode:
    print(plan.stdout, end="")
    raise SystemExit(plan.returncode)

# The compile lines of the build: `gcc ... -c -o X.o X.c`, possibly folded by
# backslash-newline. The dependency-file options are dropped and the object
# output is replaced by a syntax-only pass.
text = plan.stdout.replace("\\\n", " ")
commands = []
for line in text.splitlines():
    if " -c " not in line or not re.search(r"\.c\s*$", line.strip()):
        continue
    words = shlex.split(line)
    cleaned = []
    skip = 0
    for index, word in enumerate(words):
        if skip:
            skip -= 1
        elif word in ("-MT", "-MF", "-o"):
            skip = 1
        elif word in ("-MD", "-MP", "-c"):
            pass
        else:
            cleaned.append(word)
    commands.append(cleaned + ["-fsyntax-only"])
if len(commands) < 25:
    raise SystemExit(f"only {len(commands)} compile commands found in {out / 'make-n.log'}")

failed = 0
for number, command in enumerate(commands, 1):
    unit = Path(command[-2]).name
    result = subprocess.run(command, cwd=directory, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, text=True, check=False,
                            env=dict(os.environ, LC_ALL="C"))
    (out / f"{unit}.log").write_text(result.stdout)
    if result.returncode:
        failed += 1
        errors = [l for l in result.stdout.splitlines() if "error" in l][:3]
        print(f"FAIL {unit}: " + " | ".join(errors))
    else:
        print(f"ok   {unit}")
if failed:
    print(f"{failed} of {len(commands)} translation units do not compile standalone")
    sys.exit(1)
print(f"all {len(commands)} MWM translation units compile without CDE headers")
