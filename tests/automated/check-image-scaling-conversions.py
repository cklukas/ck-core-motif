#!/usr/bin/env python3
"""Ensure ImageCache scaling loops use checked, explicit API conversions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("void _XmPutScaledImage (")
end = source.index("\n\n\n/*** COLOR CACHE NOW", start)
region = source[start:end]

assert "unsigned int image_width, image_height;" in region
assert "for (x = (unsigned int)src_x" in region
assert "for (y = (unsigned int)src_y" in region
assert "XGetPixel(src_image, (int)x, (int)y)" in region
assert "(int)(table.x[x] + w)" in region
assert "(int)(unsigned char)pixel" in region
assert "XtMalloc((Cardinal)image_bytes)" in region
assert "table_x_bytes" in region
assert "XtMalloc((Cardinal)table_x_bytes)" in region

cache_start = source.index("CacheColorPixel(")
cache_end = source.index("\n\nstatic int", cache_start)
cache_region = source[cache_start:cache_end]
assert "size_t cache_bytes" in cache_region
assert "(Cardinal)cache_bytes" in cache_region

print("ImageCache scaling and allocation conversions are explicit")
