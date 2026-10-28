#!/usr/bin/env python3
"""Check XPM header/color line totals reject unsigned-size wraparound."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
header = source[source.index("*header = (char *) XpmMalloc") : source.index("strcpy(*header", source.index("*header = (char *) XpmMalloc"))]
colors_start = source.index("l = (unsigned int)(s - buf + 1);")
colors = source[colors_start : source.index("return (XpmSuccess);", colors_start)]

assert "l > UINT_MAX - header_size" in header
assert "l > UINT_MAX - *data_size" in colors
assert "XpmFree(s);" in colors
print("XPM header and color totals guard unsigned-size wraparound")
