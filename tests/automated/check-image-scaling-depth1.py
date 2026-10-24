#!/usr/bin/env python3
"""Check that depth-1 image copies use checked size arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("/* Flip the bits so fg == 1 in the image. */")
end = source.index("/* Image data may be constant", start)
region = source[start:end]
assert "size_t nbytes;" in region
assert "SIZE_MAX / (size_t)image->bytes_per_line" in region
assert "new_image_data = XtMalloc(nbytes);" in source
assert "image->data = new_image_data;" in source
print("depth-1 image copies check size arithmetic before allocation")
