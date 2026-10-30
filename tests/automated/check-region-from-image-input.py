#!/usr/bin/env python3
"""Ensure region construction rejects a NULL XImage safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("_XmRegionFromImage(")
end = source.index("\n}", start) + 2
region = source[start:end]

assert "if (image == NULL || image->data == NULL ||" in region
assert "image->width <= 0 || image->height <= 0" in region
guard = region.index("if (image == NULL || image->data == NULL ||")
assert "return NULL;" in region[guard:guard + 120]
assert guard < region.index("XCreateRegion()")
print("PASS region-from-image rejects invalid input")
