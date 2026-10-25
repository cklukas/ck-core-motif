#!/usr/bin/env python3
"""Check ImageCache color releases cannot underflow references."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("static Boolean\nFreeCacheColor")
end = source.index("static Boolean\nCacheColorPixel", start)
region = source[start:end]
assert "if (color->num_cached == 0)" in region
assert "return False;" in region
assert "color->num_cached--;" in region
print("PASS ImageCache color release preserves non-negative references")
