#!/usr/bin/env python3
"""Check scaled-image input validates bits-per-pixel and row stride."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("void _XmPutScaledImage")
body = source[start : source.index("/*** COLOR CACHE NOW ***/", start)]
assert "src_image->bits_per_pixel <= 0" in body
assert "src_image->width >" in body
assert "minimum_row_bytes" in body
assert "minimum_row_bytes > (size_t)src_image->bytes_per_line" in body
assert body.index("minimum_row_bytes >") < body.index("image_width =")
print("PASS scaled-image input validates row stride")
