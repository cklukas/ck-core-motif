#!/usr/bin/env python3
"""Check null-safe image-cache name lookup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static Boolean \nCompareStrings")
end = source.index("#ifdef SVG_SUPPORTED", start)
helpers = source[start:end]
assert "data_1 != NULL && data_2 != NULL" in helpers
assert "if (data == NULL)" in helpers
start = source.index("_XmInImageCache(")
body = source[start:source.index("/************************************************************************", start)]
assert "image_name == NULL || !image_set" in body
print("Xm image-cache string lookup rejects null names")
