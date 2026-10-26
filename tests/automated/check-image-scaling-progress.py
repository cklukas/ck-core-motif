#!/usr/bin/env python3
"""Check that scaled-image strips cannot repeat a zero-progress row."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "if (y >= image_height && table.y[y] <= min_y)" in region
assert "goto cleanup;" in region
print("scaled image strips reject zero-progress row traversal")
