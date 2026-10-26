#!/usr/bin/env python3
"""Check scaled image buffers fit Xt's Cardinal allocation width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("image_bytes = (size_t)dest_image->bytes_per_line * h;")
body = source[start:source.index("fast8 =", start)]
assert "if (image_bytes > UINT_MAX)" in body
assert "dest_image->data = XtMalloc(image_bytes);" in body
print("PASS image-cache scaled buffer bounds for Xt Cardinal")
