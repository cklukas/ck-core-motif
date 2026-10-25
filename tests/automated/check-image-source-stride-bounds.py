#!/usr/bin/env python3
"""Check scaled-image conversion validates source data and row stride."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "src_image->data == NULL" in region
assert "SIZE_MAX / (size_t)src_image->bytes_per_line" in region
assert "src_image->height >" in region
print("PASS scaled-image conversion validates source stride and data")
