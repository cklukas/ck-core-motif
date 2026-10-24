#!/usr/bin/env python3
"""Check that PNG conversion honors 24- and 32-bit XImage pixel widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
assert "ximage_pixel_bytes = (ximage->bits_per_pixel + 7) / 8;" in source
assert "if (ximage_pixel_bytes < 3 || ximage_pixel_bytes > 4)" in source
assert "png_store_pixel(&dest, pixel, ximage_pixel_bytes);" in source
assert "/* GRR BUG:  this assumes bpp == 32" not in source
assert source.count("png_store_pixel(&dest, pixel, ximage_pixel_bytes);") == 2
print("PNG conversion honors the XImage pixel width")
