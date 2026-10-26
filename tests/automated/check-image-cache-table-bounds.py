#!/usr/bin/env python3
"""Check image-cache scaling tables fit both size_t and Xt Cardinal."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("if ((size_t)src_image->width >\n\t    SIZE_MAX / sizeof(*table.x)")
body = source[start:source.index("table.x =", start)]
for expression in (
    "UINT_MAX / sizeof(*table.x)",
    "UINT_MAX / sizeof(*table.y)",
    "UINT_MAX / sizeof(*table.width)",
    "UINT_MAX / sizeof(*table.height)",
):
    assert expression in body
print("PASS image-cache scaling table bounds for Xt Cardinal")
