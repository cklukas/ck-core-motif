#!/usr/bin/env python3
"""Check the raw XPM-image conversion entry validates its inputs first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("XpmCreateImageFromXpmImage(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
assert "display == NULL || image == NULL" in body
assert "image_return == NULL && shapeimage_return == NULL" in body
assert "return XpmFileInvalid;" in body
assert source.index("if (display == NULL", start) < source.index(
    "image->ncolors", start
)
print("PASS XPM raw-image entry inputs are validated")
