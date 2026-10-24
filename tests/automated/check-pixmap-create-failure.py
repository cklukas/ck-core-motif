#!/usr/bin/env python3
"""Check that pixmap creation failure does not enter the cache."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("pixmap = XCreatePixmap")
cache_insert = source.index("pix_entry->pixmap = pixmap", start)
region = source[start:cache_insert]
assert "if (pixmap == XmUNSPECIFIED_PIXMAP)" in region
assert "XDestroyImage(image);" in region
assert "ReleaseImagePixels(screen, pixels, npixels);" in region
assert "XtFree((char *)pix_entry);" in region
print("pixmap creation failure releases resources before cache insertion")
