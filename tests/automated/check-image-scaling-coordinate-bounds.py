#!/usr/bin/env python3
"""Check signed destination-coordinate handling in image scaling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("\n\n\n/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "dest_y > INT_MAX - (int)dest_height" in region
assert "(long long)dest_y + (long long)min_y" in region
assert "output_y = (int)((long long)dest_y + (long long)min_y);" in region
assert "dest_y + min_y," not in region
print("PASS image scaling checks signed strip destination coordinates")
