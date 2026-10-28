#!/usr/bin/env python3
"""Check XPM scanning accepts images containing only a transparent color."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text(
    encoding="utf-8"
)
start = source.index("/* start from the right element */")
region = source[start:source.index("/* first get character strings", start)]
assert "ncolors--;" in region
assert "if (ncolors == 0)" in region
assert "return (XpmSuccess);" in region
assert region.index("if (ncolors == 0)") < region.index("return (XpmSuccess);")
print("PASS XPM transparent-only images avoid zero-size allocation")
