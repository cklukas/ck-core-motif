#!/usr/bin/env python3
"""Check that pixmap geometry failure is rejected before caching."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmCachePixmap(")
end = source.index("\n\n\n/********************************************************************/", start)
region = source[start:end]
assert "if (!(width && height && depth)) {" in region
assert "if (!XGetGeometry(DisplayOfScreen(screen), pixmap" in region
assert "loc_width > (unsigned int)((Dimension)~0)" in region
assert "loc_height > (unsigned int)((Dimension)~0)" in region
assert "loc_depth > (unsigned int)INT_MAX" in region
assert "pix_entry->width = (Dimension)loc_width" in region
assert "pix_entry->height = (Dimension)loc_height" in region
assert "return (False);" in region
assert region.index("XGetGeometry") < region.index("pix_entry = XtNew")
print("pixmap cache rejects failed geometry queries")
