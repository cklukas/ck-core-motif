#!/usr/bin/env python3
"""Check that image-array size validation rejects zero element sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("XmImageArrayFits(size_t count")
end = source.index("\n\nstatic Boolean\nXmMaskNameSize", start)
region = source[start:end]
assert "element_size != 0" in region
assert region.index("element_size != 0") < region.index("SIZE_MAX / element_size")
print("image-array size validation rejects zero element sizes")
