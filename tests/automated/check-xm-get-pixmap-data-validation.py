#!/usr/bin/env python3
"""Check Xm pixmap-data argument and output validation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmGetPixmapData(")
end = source.index("/*******************************************************************/\n/* Release temporary state", start)
body = source[start:end]
assert "screen == NULL" in body
assert "pixmap == XmUNSPECIFIED_PIXMAP" in body
for name in ("image_name", "depth", "foreground", "background", "hot_x", "hot_y", "width", "height"):
    assert f"{name} == NULL" in body
    assert f"*{name} =" in body
assert "pixmap_data_set == NULL || pixmap_set == NULL" in body
print("Xm pixmap-data lookup validates arguments and clears outputs")
