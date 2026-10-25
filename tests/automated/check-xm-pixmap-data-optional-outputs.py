#!/usr/bin/env python3
"""Check optional output compatibility for Xm pixmap-data lookup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmGetPixmapData(")
end = source.index("/*******************************************************************/\n/* Release temporary state", start)
body = source[start:end]
assert "screen == NULL || pixmap == XmUNSPECIFIED_PIXMAP" in body
for name in ("image_name", "depth", "foreground", "background", "hot_x", "hot_y", "width", "height"):
    assert f"if ({name})" in body
assert "if (image_name == NULL" not in body
assert "_XmGetHashEntry(image_set, pix_entry->image_name)" in body
print("Xm pixmap-data lookup preserves optional output pointers")
