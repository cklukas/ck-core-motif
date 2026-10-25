#!/usr/bin/env python3
"""Check deterministic hotspot outputs for cached pixmaps."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmGetPixmapData(")
end = source.index("\n\n\n/*******************************************************************/", start)
body = source[start:end]
assert "*hot_x = 0;" in body
assert "*hot_y = 0;" in body
assert body.index("*hot_x = 0;") < body.index("if (image_set)")
assert body.index("*hot_y = 0;") < body.index("if (image_set)")
print("Xm pixmap hotspot outputs are initialized on cache hits")
