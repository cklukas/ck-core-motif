#!/usr/bin/env python3
"""Build and run the installed-prefix XmDatePicker fixture."""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
prefix = Path(os.environ.get("MOTIF_TEST_PREFIX", ""))
if not prefix:
    raise SystemExit("usage: MOTIF_TEST_PREFIX=PREFIX run-date-picker.py")
env = os.environ.copy()
env["LD_LIBRARY_PATH"] = str(prefix / "lib") + (":" + env["LD_LIBRARY_PATH"] if env.get("LD_LIBRARY_PATH") else "")
with tempfile.TemporaryDirectory(prefix="date-picker-", dir=env.get("TMPDIR")) as tmp:
    binary = Path(tmp) / "check-date-picker"
    subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-Werror",
                    "-I" + str(prefix / "include"),
                    str(root / "tests/automated/check-date-picker.c"),
                    "-L" + str(prefix / "lib"), "-lXm", "-lXt", "-lX11",
                    "-o", str(binary)], check=True, env=env)
    subprocess.run(["xvfb-run", "-a", str(binary)], check=True, env=env)
