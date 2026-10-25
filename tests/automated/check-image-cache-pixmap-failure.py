#!/usr/bin/env python3
"""Check that XCreatePixmap failure releases image-cache construction state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("if (pixmap == XmUNSPECIFIED_PIXMAP)")
end = source.index("pix_entry->pixmap", start)
region = source[start:end]
assert "DiscardScaledPixmapBuild(ret, image, old_image_data" in region
assert "_XmProcessUnlock();" not in region
assert "XtFree(pix_entry->image_name)" not in region
print("PASS image-cache pixmap failure uses complete cleanup")
