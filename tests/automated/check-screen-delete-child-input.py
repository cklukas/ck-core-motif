#!/usr/bin/env python3
"""Check that Screen child deletion validates parent/list state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (wid == NULL)", "cw == NULL ||", "cw->desktop.children == NULL"):
    if needle not in text:
        raise SystemExit(f"Screen child deletion guard is missing: {needle}")

print("Screen child deletion guards present")
