#!/usr/bin/env python3
"""Check XPM automatic chars-per-pixel growth cannot wrap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text(
    encoding="utf-8"
)
start = source.index("for (cppm = 1, c = MAXPRINTABLE")
region = source[start:source.index("if (cpp < cppm)", start)]
assert "if (c > UINT_MAX / MAXPRINTABLE)" in region
assert "RETURN(XpmNoMemory);" in region
assert region.index("if (c > UINT_MAX / MAXPRINTABLE)") < region.index(
    "c *= MAXPRINTABLE"
)
print("PASS XPM chars-per-pixel growth is overflow-safe")
