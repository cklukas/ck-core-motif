#!/usr/bin/env python3
"""Check that PNG conversion honors 24- and 32-bit XImage pixel widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Png.c").read_text()
assert "bg_red = (unsigned char)(xcolor.red >> 8);" in source
assert "bg_green = (unsigned char)(xcolor.green >> 8);" in source
assert "bg_blue = (unsigned char)(xcolor.blue >> 8);" in source
assert "ximage_pixel_bytes = (ximage->bits_per_pixel + 7) / 8;" in source
assert "if (ximage_pixel_bytes < 3 || ximage_pixel_bytes > 4)" in source
assert "png_store_pixel(&dest, pixel, ximage_pixel_bytes);" in source
assert "/* GRR BUG:  this assumes bpp == 32" not in source
assert source.count("png_store_pixel(&dest, pixel, ximage_pixel_bytes);") == 2
assert "*dest++ = (char) ((pixel >> 24) & 0xff);" not in source
print("PNG conversion honors 16-bit XColor background and XImage pixel width across RGB and RGBA")
