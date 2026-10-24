#!/usr/bin/env python3
"""Build and run the deterministic WP-27 bounded-model benchmark."""
from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix="data-grid-benchmark-", dir=os.environ.get("TMPDIR")) as tmp:
    binary = Path(tmp) / "benchmark-data-grid-prototype"
    subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-O2", "-Wall",
                    "-Wextra", "-Werror",
                    str(root / "tests/automated/benchmark-data-grid-prototype.c"),
                    "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
