#!/usr/bin/env python3
"""Check XPM conversion validates the requested bitmap format."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(
    encoding="utf-8"
)
assert source.count("attributes->bitmap_format != XYBitmap") >= 2
assert source.count("attributes->bitmap_format != ZPixmap") >= 2
print("PASS XPM bitmap format input is validated")
