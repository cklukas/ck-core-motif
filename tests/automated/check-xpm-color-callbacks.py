#!/usr/bin/env python3
"""Check XPM image conversion rejects incomplete color callback attributes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("XpmCreateImageFromXpmImage(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
assert "XpmAllocColor" in body
assert "attributes->alloc_color == NULL" in body
assert "XpmFreeColors" in body
assert "attributes->free_colors == NULL" in body
assert "return XpmFileInvalid;" in body
print("PASS XPM color callback attributes are validated")
