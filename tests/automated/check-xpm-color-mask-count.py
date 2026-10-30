#!/usr/bin/env python3
"""Ensure XPM color scanning rejects an empty masked color table."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text()
start = source.index("static int\nScanOtherColors")
end = source.index("\nstatic int", start + 1)
region = source[start:end]

assert "ncolors != 0 && (colors == NULL || pixels == NULL)" in region
assert "if (ncolors == 0 || colors == NULL || pixels == NULL)" in region
print("PASS XPM color scanning rejects empty or null masked tables")
