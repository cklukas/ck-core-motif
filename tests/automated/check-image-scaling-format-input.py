#!/usr/bin/env python3
"""Check scaled-image copying rejects unknown XImage formats."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "src_image->format != XYBitmap" in body
assert "src_image->format != XYPixmap" in body
assert "src_image->format != ZPixmap" in body
assert body.index("src_image->format != XYBitmap") < body.index("(size_t)src_image->width >")
print("PASS scaled-image copying rejects unknown XImage formats")
