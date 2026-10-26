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
assert "(unsigned int)image->width" in source
assert "(unsigned int)image->height" in source

override_start = source.index("static int\nGetOverrideColors")
override_end = source.index("\n\n\n#ifdef _ORIG_GET_ICON_FILE", override_start)
override_region = source[override_start:override_end]
assert "Cardinal i;" in override_region
assert "int n;" in override_region
assert "return n ;" in override_region

assert "attrib.numsymbols = (unsigned int)num_override_colors;" in source
assert "loc_depth = (unsigned int)depth ;" in source

cache_start = source.index("CacheColorPixel(")
cache_end = source.index("\n\nstatic int", cache_start)
cache_region = source[cache_start:cache_end]
assert "size_t cache_bytes" in cache_region
assert "(Cardinal)cache_bytes" in cache_region

print("ImageCache scaling and allocation conversions are explicit")
