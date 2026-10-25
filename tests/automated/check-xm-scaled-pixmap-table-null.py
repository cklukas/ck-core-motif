#!/usr/bin/env python3
"""Check scaled-pixmap table initialization failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmGetScaledPixmap(")
end = source.index("/* if screen not provided", start)
body = source[start:end]
assert "if (!pixmap_data_set) InitializePixmapSets()" in body
assert "if (pixmap_data_set == NULL || pixmap_set == NULL)" in body
assert "return (XmUNSPECIFIED_PIXMAP);" in body
print("Xm scaled-pixmap lookup handles missing cache tables")
