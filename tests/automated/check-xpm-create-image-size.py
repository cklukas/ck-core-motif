#!/usr/bin/env python3
"""Check XPM image-data allocation uses size_t overflow checks."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text()
start = source.index(" * Create an XImage with its data")
start = source.index("\nCreateXImage(\n", start)
start = source.rfind("static int", 0, start)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "SIZE_MAX / (size_t)height" in function
assert "XpmMalloc((size_t)(*image_return)->bytes_per_line *" in function
assert "INT_MAX / height" not in function
print("PASS XPM image allocation size check")
