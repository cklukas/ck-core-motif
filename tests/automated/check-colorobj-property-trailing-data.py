#!/usr/bin/env python3
"""Ensure ColorObj property parsing rejects trailing bytes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()
start = source.index("static Boolean\nFetchPixelData")
end = source.index("static Boolean\nColorCachePropertyExists", start)
region = source[start:end]

parse_end = region.index("UpdateXrm (colors, screen, tmpColorObj);")
validation = region[:parse_end]
assert "if (*cursor != '\\0')" in validation
guard = validation.index("if (*cursor != '\\0')")
assert "XtFree(bounded_value);" in validation[guard:guard + 120]
assert "return False;" in validation[guard:guard + 160]
print("PASS ColorObj property parser rejects trailing data")
