#!/usr/bin/env python3
"""Check that pixmap creation failure does not enter the cache."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("pixmap = XCreatePixmap")
cache_insert = source.index("pix_entry->pixmap = pixmap", start)
region = source[start:cache_insert]
assert "if (pixmap == XmUNSPECIFIED_PIXMAP)" in region
assert "DiscardScaledPixmapBuild" in region
assert "XFreePixmap(display, pixmap);" in source[start:]
print("pixmap creation failure releases resources before cache insertion")
