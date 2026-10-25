#!/usr/bin/env python3
"""Check that Screen font-unit APIs validate display/screen state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (display == NULL)", "if (xmScreen == NULL)", "if (screen == NULL)"):
    if needle not in text:
        raise SystemExit(f"font-unit input guard is missing: {needle}")

print("font-unit input guards present")
