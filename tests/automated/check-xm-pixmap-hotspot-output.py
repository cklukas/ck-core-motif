#!/usr/bin/env python3
"""Check deterministic hotspot outputs for cached pixmaps."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmGetPixmapData(")
end = source.index("\n\n\n/*******************************************************************/", start)
body = source[start:end]
if "*hot_x = 0;" not in body or "*hot_y = 0;" not in body:
    raise SystemExit("cached hotspot outputs are not initialized")
if body.index("*hot_x = 0;") > body.index("if (image_set)"):
    raise SystemExit("hotspot defaults are assigned after optional metadata lookup")
if body.index("*hot_y = 0;") > body.index("if (image_set)"):
    raise SystemExit("hotspot defaults are assigned after optional metadata lookup")
print("PASS Xm pixmap hotspot outputs are initialized on cache hits")
