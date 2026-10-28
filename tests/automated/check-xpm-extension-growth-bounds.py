#!/usr/bin/env python3
"""Check XPM extension and line arrays guard count multiplication."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmParseExtensions(")
body = source[start:source.index("int\nxpmParseData(", start)]
assert "exts == NULL" in body
assert "(size_t)(num + 1) > (size_t)UINT_MAX / sizeof(XpmExtension)" in body
assert "(size_t)(nlines + 1) > (size_t)UINT_MAX / sizeof(char *)" in body
assert "can the loop be forced" not in body
print("PASS XPM extension growth guards allocation arithmetic")
