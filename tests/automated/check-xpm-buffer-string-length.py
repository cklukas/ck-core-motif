#!/usr/bin/env python3
"""Check XPM in-memory string lengths are bounded before narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmGetString(")
body = source[start:source.index("int\nxpmGetCmt(", start)]
assert "size_t span;" in body
assert "span >= (size_t)UINT_MAX" in body
assert "n = (unsigned int)(span + 1);" in body
print("PASS XPM buffer string lengths are bounded")
