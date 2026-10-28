#!/usr/bin/env python3
"""Check XPM file-string parsing handles its initial allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmGetString(")
body = source[start:source.index("int\nxpmGetCmt(", start)]
needle = "p = (char *) XpmMalloc(1);"
position = body.index(needle)
assert "if (!p)" in body[position : position + 90]
assert "return (XpmNoMemory);" in body[position : position + 120]
print("PASS XPM string parsing handles initial allocation failure")
