#!/usr/bin/env python3
"""Check cropped image scaling normalizes X coordinates."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
body = source[start:end]

assert "source_origin_x = table.x[src_x]" in body
assert "table.x[src_max_x] - source_origin_x > dest_width" in body
assert "table.x[x] - source_origin_x" in body

print("PASS cropped image scaling normalizes source X")
