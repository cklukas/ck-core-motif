#!/usr/bin/env python3
"""Build and run the installed-prefix XmCalendar fixture."""
import os
import pathlib
import subprocess
import sys
import tempfile

root = pathlib.Path(__file__).resolve().parents[2]
prefix = pathlib.Path(os.environ.get("MOTIF_TEST_PREFIX", ""))
if len(sys.argv) > 1:
    prefix = pathlib.Path(sys.argv[1])
if not prefix:
    raise SystemExit("usage: run-calendar.py PREFIX")
cc = os.environ.get("CC", "gcc")
env = os.environ.copy()
env["LD_LIBRARY_PATH"] = f"{prefix / 'lib'}:{env.get('LD_LIBRARY_PATH', '')}"
with tempfile.TemporaryDirectory(prefix="calendar-", dir=env.get("TMPDIR")) as tmp:
    binary = pathlib.Path(tmp) / "check-calendar"
    compile_cmd = [cc, "-std=c99", "-Werror", "-I" + str(prefix / "include"),
                   str(root / "tests/automated/check-calendar.c"),
                   "-L" + str(prefix / "lib"), "-lXm", "-lXt", "-lX11", "-o",
                   str(binary)]
    subprocess.run(compile_cmd, check=True, env=env)
    subprocess.run(["xvfb-run", "-a", str(binary)], check=True, env=env)
