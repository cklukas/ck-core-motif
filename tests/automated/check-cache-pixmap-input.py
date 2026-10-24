#!/usr/bin/env python3
"""Check that _XmCachePixmap rejects invalid handles before Xlib calls."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmCachePixmap(")
end = source.index("\n\n\n/********************************************************************/", start)
region = source[start:end]
assert "if (screen == NULL || pixmap == XmUNSPECIFIED_PIXMAP" in region
assert "image_name == NULL)" in region
assert region.index("screen == NULL") < region.index("InitializePixmapSets")
assert region.index("pixmap == XmUNSPECIFIED_PIXMAP") < region.index("XGetGeometry")
print("pixmap cache rejects invalid screen and pixmap inputs")
