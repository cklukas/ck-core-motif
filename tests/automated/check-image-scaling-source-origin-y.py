#!/usr/bin/env python3
"""Check cropped image scaling normalizes Y coordinates."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
body = source[start:end]

assert "source_origin_y = table.y[src_y]" in body
assert "table.y[src_y + src_height] - source_origin_y > dest_height" in body
assert "min_y - source_origin_y" in body

print("PASS cropped image scaling normalizes source Y")
