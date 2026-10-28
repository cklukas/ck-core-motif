#!/usr/bin/env python3
"""Check XPM file-string accumulation guards unsigned length arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmGetString(")
body = source[start:source.index("int\nxpmGetCmt(", start)]
assert "n > UINT_MAX - i" in body
assert "n > UINT_MAX - i - 1" in body
assert "XpmFree(p);" in body
print("PASS XPM string growth guards unsigned length arithmetic")
