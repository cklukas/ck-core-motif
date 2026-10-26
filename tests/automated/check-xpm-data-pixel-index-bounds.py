#!/usr/bin/env python3
"""Check XPM image serialization validates pixel indexes and row arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nCreatePixels")
body = source[start:source.index("static int\nCountExtensions", start)]
assert "unsigned int\t  ncolors" in body
assert "*pixels >= ncolors" in body
assert "width > (UINT_MAX - 1) / cpp" in body
assert "(size_t)(s - *dataptr) > data_size" in body
assert "cpp >= data_size - (size_t)(s - *dataptr)" in body
call = source[source.index("if (!CreatePixels"):source.index("RETURN(XpmFileInvalid);", source.index("if (!CreatePixels"))]
assert "image->ncolors" in call
print("PASS XPM data serialization bounds pixel indexes and row arithmetic")
