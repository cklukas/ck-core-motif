#!/usr/bin/env python3
"""Build and run the installed-prefix XmTeachingTip fixture."""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
prefix = Path(os.environ.get("MOTIF_TEST_PREFIX", ""))
if not prefix:
    raise SystemExit("usage: MOTIF_TEST_PREFIX=PREFIX run-teaching-tip.py")
env = os.environ.copy()
env["LD_LIBRARY_PATH"] = str(prefix / "lib")
with tempfile.TemporaryDirectory(prefix="teaching-tip-", dir=env.get("TMPDIR")) as tmp:
    binary = Path(tmp) / "check-teaching-tip"
    subprocess.run([os.environ.get("CC", "cc"), "-std=c11", "-Werror",
                    "-I" + str(prefix / "include"),
                    str(root / "tests/automated/check-teaching-tip.c"),
                    "-L" + str(prefix / "lib"), "-lXm", "-lXt", "-lX11",
                    "-o", str(binary)], check=True, env=env)
    subprocess.run(["xvfb-run", "-a", str(binary)], check=True, env=env)
