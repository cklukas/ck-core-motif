#!/usr/bin/env python3
"""Check that desktop color-cell lookup rejects non-positive capacities."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(
    encoding="utf-8"
)
start = text.index("XmeGetDesktopColorCells")
body = text[start : text.index("}\n", start) + 2]
guard = "if (!colors || n_colors <= 0) return False ;"
if guard not in body:
    raise SystemExit("desktop color-cell capacity guard is missing")

print("desktop color-cell capacity guard present")
