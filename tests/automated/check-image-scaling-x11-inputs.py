#!/usr/bin/env python3
"""Check that scaled-image output validates its X11 handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("void _XmPutScaledImage")
end = source.index("/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
assert "display == NULL" in region
assert "d == None" in region
assert "gc == NULL" in region
print("PASS scaled-image output validates X11 handles")
