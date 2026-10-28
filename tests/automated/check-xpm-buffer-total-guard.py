#!/usr/bin/env python3
"""Check XPM buffer totals are bounded before unsigned addition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
start = source.index("/* write the header line */")
region = source[start : source.index("ptr = (char *) XpmMalloc", start)]

assert "used_size > UINT_MAX - ext_size" in region
assert "cmt_size > UINT_MAX - used_size - ext_size" in region
assert "ptr_size = used_size + ext_size + cmt_size + 1;" in region
assert region.index("used_size > UINT_MAX - ext_size") < region.index("ptr_size =")
print("XPM buffer totals are bounded before unsigned addition")
