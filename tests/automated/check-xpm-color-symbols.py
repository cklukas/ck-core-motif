#!/usr/bin/env python3
"""Check XPM color-symbol attributes are complete before scanning."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
start = source.index("XpmCreateImageFromXpmImage(")
body = source[start:source.index("/* retrieve information from the XpmAttributes */", start)]
assert "XpmColorSymbols" in body
assert "attributes->numsymbols != 0" in body
assert "attributes->colorsymbols == NULL" in body
assert "return XpmFileInvalid;" in body
print("PASS XPM color-symbol attributes are validated")
