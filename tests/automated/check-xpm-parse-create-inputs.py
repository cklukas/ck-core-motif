#!/usr/bin/env python3
"""Check the direct XPM parse-and-create path validates its inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("xpmParseDataAndCreate(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
for condition in (
    "display == NULL",
    "data == NULL",
    "image == NULL",
    "image_return == NULL && shapeimage_return == NULL",
    "attributes->visual == NULL",
    "attributes->alloc_color == NULL",
    "attributes->free_colors == NULL",
    "attributes->colorsymbols == NULL",
):
    assert condition in body
assert "return XpmFileInvalid;" in body
print("PASS XPM parse-and-create inputs are validated")
