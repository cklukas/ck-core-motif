#!/usr/bin/env python3
"""Check XPM pixel-map growth rejects size and allocation multiplication overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nstorePixel")
body = source[start:source.index("static int\nstoreMaskPixel", start)]
assert "pmap->size == 0 || pmap->size > UINT_MAX / 2" in body
assert "new_size = pmap->size * 2" in body
assert "new_size > UINT_MAX / sizeof(Pixel)" in body
assert "sizeof(Pixel) * new_size" in body
print("PASS XPM pixel-map growth bounds size multiplication")
