#!/usr/bin/env python3
"""Check rollback when scaled-pixmap GC creation fails."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("gc = GetGCForPutImage(")
end = source.index("/* transfer and scale the image */", start)
body = source[start:end]
assert "if (gc == NULL)" in body
assert "_XmRemoveHashEntry(pixmap_data_set, pix_entry);" in body
assert "_XmRemoveHashEntry(pixmap_set, pix_entry);" in body
assert "XFreePixmap(display, pixmap);" in body
assert "DiscardScaledPixmapBuild(ret, image" in body
print("Xm scaled-pixmap GC failure rolls back provisional cache state")
