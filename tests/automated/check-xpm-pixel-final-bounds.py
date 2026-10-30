#!/usr/bin/env python3
"""Ensure XPM pixel serialization bounds its final quote and size update."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
start = source.index("static int\nWritePixels")
end = source.index("\nstatic unsigned int\nExtensionsSize", start)
region = source[start:end]

assert "(size_t)(s - dataptr) >= (size_t)data_size" in region
assert "(size_t)(s - dataptr) > (size_t)UINT_MAX - *used_size" in region
print("PASS XPM pixel serialization bounds its final quote and size update")
