#!/usr/bin/env python3
"""Check that Screen menu-cursor APIs validate display/screen state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (display == NULL)",
    "if (xmScreen != NULL)",
    "if (screen == NULL)",
):
    if needle not in text:
        raise SystemExit(f"menu-cursor input guard is missing: {needle}")

print("menu-cursor input guards present")
