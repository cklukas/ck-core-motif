#!/usr/bin/env python3
"""Check that rejected image conversions release transient resources."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
assert "static void ReleaseImagePixels(Screen *screen, Pixel *pixels, int npixels);" in source
assert "ReleaseImagePixels(screen, pixels, npixels);" in source
assert "ReleaseImagePixels(pix_entry->screen, pix_entry->pixels," in source
condition = source.index("if (image->data == NULL || image->width <= 0")
region = source[condition:source.index("/* force the foreground", condition)]
assert "XDestroyImage(image);" in region
assert "ReleaseImagePixels(screen, pixels, npixels);" in region
print("rejected scaled images release transient XImage and color storage")
