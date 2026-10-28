#!/usr/bin/env python3
"""Check XPM raw-image conversion validates the image structure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("XpmCreateImageFromXpmImage(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
for condition in (
    "image->width == 0",
    "image->height == 0",
    "image->cpp == 0",
    "image->ncolors == 0",
    "image->colorTable == NULL",
    "image->data == NULL",
):
    assert condition in body
assert "return XpmFileInvalid;" in body
print("PASS XPM raw-image structure is validated")
