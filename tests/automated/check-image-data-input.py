#!/usr/bin/env python3
"""Check scaled-pixmap construction rejects an XImage without pixel data."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGetScaledPixmap(")
body = source[start : source.index("/****************", start)]

needle = "if (image->data == NULL || image->width <= 0 || image->height <= 0 ||"
if needle not in body:
    raise SystemExit("scaled-pixmap construction does not reject missing image data")
if body.index(needle) > body.index("old_image_data = image->data"):
    raise SystemExit("missing image data is checked after the depth-1 dereference")

print("scaled-pixmap construction rejects missing XImage data")
