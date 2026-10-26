#!/usr/bin/env python3
"""Check that scaled temporary images use the target drawable depth."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "unsigned int destination_depth;" in region
assert "destination_depth = (unsigned int)src_image->depth;" in region
assert "destination_depth = geometry_depth;" in region
assert "destination_depth,\n" in region
print("scaled temporary images depth follows the target drawable")
