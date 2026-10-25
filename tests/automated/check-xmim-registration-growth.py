#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmIm.c").read_text()
first = source.index("remember_values(")
start = source.index("remember_values(", first + 1)
region = source[start:source.index("\n}\n", start) + 2]

assert "SIZE_MAX / sizeof(Arg) - 1" in region
assert "UINT_MAX / sizeof(Arg) - 1" in region
assert region.index("continue;") < region.index("XtRealloc")
assert "((size_t)r->count + 1) * sizeof(Arg)" in region

print("XmIm registration growth checks Arg-array byte bounds")
