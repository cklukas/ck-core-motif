#!/usr/bin/env python3
"""Check scaled-image row offsets use size_t arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "(size_t)y * (size_t)src_image->bytes_per_line" in region
assert "(size_t)(table.y[y] - min_y + h)" in region
assert "(size_t)dest_image->bytes_per_line" in region
print("PASS scaled-image row offsets use size_t arithmetic")
