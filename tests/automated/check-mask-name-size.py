#!/usr/bin/env python3
"""Check image mask-name allocations use checked size arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
assert "static Boolean XmMaskNameSize(String image_name, size_t *size);" in source
helper_start = source.rindex("XmMaskNameSize(String image_name")
helper = source[helper_start:source.index("\n}\n", helper_start) + 2]
assert "length > SIZE_MAX - 3" in helper
assert "*size = length + 3" in helper
assert "*size > UINT_MAX" in helper

mask_start = source.index("XmeGetMask(")
mask = source[mask_start:source.index("\n}\n", mask_start) + 2]
assert "XmMaskNameSize(image_name, &mask_name_size)" in mask
assert "XtMalloc((Cardinal)mask_name_size)" in mask
assert mask.index("XmMaskNameSize") < mask.index("XtDisplayToApplicationContext")

xpm_start = source.index("if (mask_image && useMask)")
xpm = source[xpm_start:source.index("\n\t} else if (mask_image)", xpm_start)]
assert "XmMaskNameSize(image_name, &mask_name_size)" in xpm
assert "XtMalloc((Cardinal)mask_name_size)" in xpm
print("PASS checked image mask-name sizing")
