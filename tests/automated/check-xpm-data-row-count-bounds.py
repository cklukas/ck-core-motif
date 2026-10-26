#!/usr/bin/env python3
"""Check XPM data serialization bounds the combined pointer-row count."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("data_size += header_size + ext_size;")
body = source[start:source.index("data = (char **) XpmMalloc", start)]
assert "header_nlines > UINT_MAX - image->height" in body
assert "(header_nlines + image->height) > UINT_MAX - ext_nlines" in body
assert "data_nlines = header_nlines + image->height + ext_nlines" in source
print("PASS XPM data serialization bounds combined pointer rows")
