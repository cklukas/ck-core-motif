#!/usr/bin/env python3
"""Check that scaled table walks stay inside their allocated rows."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "y < image_height && table.y[y] < max_y" in region
assert "table.y[image_height]" not in region
print("scaled image row traversal is explicitly bounded")
