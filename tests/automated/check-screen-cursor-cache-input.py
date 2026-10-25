#!/usr/bin/env python3
"""Check that cursor-cache removal validates its drag-icon input."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (icon == NULL)", "if (xmScreen == NULL)"):
    if needle not in text:
        raise SystemExit(f"cursor-cache input guard is missing: {needle}")

print("cursor-cache input guards present")
