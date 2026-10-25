#!/usr/bin/env python3
"""Check that default-glyph lookup validates screen/output state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (screen == NULL) {",
    "if (xmScreen == NULL) {",
    "if (width) *width = 0;",
):
    if needle not in text:
        raise SystemExit(f"default-glyph input guard is missing: {needle}")

print("default-glyph input guards present")
