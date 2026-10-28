#!/usr/bin/env python3
"""Check XPM extension names are length-validated and NUL-terminated."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nxpmParseExtensions(")
body = source[start:source.index("int\nxpmParseData(", start)]
assert "a > l || l - a < 6" in body
assert "name_length = (size_t)(l - a - 6);" in body
assert "XpmMalloc(name_length + 1)" in body
assert "memcpy(ext->name, s + a, name_length);" in body
assert "ext->name[name_length] = '\\0';" in body
assert "strncpy(ext->name" not in body
print("PASS XPM extension names are terminated safely")
