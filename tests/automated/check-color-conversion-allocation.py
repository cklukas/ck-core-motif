#!/usr/bin/env python3
"""Check color conversion validates strdup before reading its result."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Color.c").read_text()
start = source.index("_XmConvertToBW(")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "col != NULL && HasExpandedHexColor(col)" in body
assert "col = strdup(im.colorTable[0].c_color);" in body
print("PASS color conversion checks duplicated color allocation")
