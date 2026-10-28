#!/usr/bin/env python3
"""Check XPM conversion rejects an explicitly requested null visual."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("XpmCreateImageFromXpmImage(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
assert "XpmVisual" in body
assert "attributes->visual == NULL" in body
assert "return XpmFileInvalid;" in body
print("PASS XPM explicit visual input is validated")
