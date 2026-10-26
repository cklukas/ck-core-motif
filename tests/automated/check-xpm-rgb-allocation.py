#!/usr/bin/env python3
"""Check XPM RGB lookup handles string duplication failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmrgb.c").read_text()
start = source.index("xpmGetRGBfromName(")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "name = xpmstrdup(inname);" in body
assert "inname == NULL || r == NULL || g == NULL || b == NULL" in body
assert "if (name == NULL)" in body
assert "return (0);" in body
assert "middle = left + (right - left) / 2;" in body
assert "middle = (left + right) / 2;" not in body
assert "rgbn == NULL || rgbn_max <= 0" in source
assert "rgb_fname == NULL || rgbn == NULL" in source
free_start = source.index("xpmFreeRgbNames(\n")
free_end = source.index("\n}\n", free_start) + 2
assert "rgbn == NULL || rgbn_max <= 0" in source[free_start:free_end]
print("PASS XPM RGB lookup checks duplicated-name allocation")
