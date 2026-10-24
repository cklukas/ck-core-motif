#!/usr/bin/env python3
"""Check that JPEG conversion honors XImage layout and row stride."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Jpeg.c").read_text()
assert "int pixel_bytes = (ximage->bits_per_pixel + 7) / 8;" in source
assert "(size_t)y * (size_t)ximage->bytes_per_line" in source
assert "x * (size_t)pixel_bytes" in source
assert "if (ximage->byte_order == MSBFirst)" in source
assert "cdata[x * 4]" not in source
assert "store_pixel(screen, p, *ximage, xx, yy)" in source
print("JPEG conversion honors XImage pixel width, byte order, and stride")
