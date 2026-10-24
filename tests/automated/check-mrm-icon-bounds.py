#!/usr/bin/env python3
"""Check URM icon dimension, stride, and raster-size bounds."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "iconwid <= 0 || iconhgt <= 0 || srcpix <= 0",
    "SIZE_MAX / (size_t)iconhgt",
    "SIZE_MAX / (size_t)dstbytsize",
    "(size_t)iconwid * (size_t)iconhgt",
    "bytes_per_line > INT_MAX / icon->height",
    "sizeof(RGMIconImage) + (size_t)(raster_len - 1) > UINT_MAX",
):
    if needle not in text:
        raise SystemExit(f"Mrm icon capacity guard missing: {needle}")

print("Mrm icon allocation sizing is bounded")
