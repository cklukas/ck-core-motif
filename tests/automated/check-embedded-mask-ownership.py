#!/usr/bin/env python3
"""Check that embedded XPM masks use bounded names and clear ownership."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("if (mask_image && useMask)")
end = source.index("\n\n\t/* now we have to adjust", start)
region = source[start:end]
assert "String mask_name = XtMalloc(strlen(image_name) + 3);" in region
assert "if (!installed)" in region
assert "XDestroyImage(mask_image);" in region
assert "char mask_name[255]" not in region
print("embedded XPM mask names and image ownership are bounded")
