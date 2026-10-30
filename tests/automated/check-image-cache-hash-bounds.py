#!/usr/bin/env python3
"""Check ImageCache hash mixes are bounded before signed conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("HashString (XmHashKey key)")
end = source.index("HashPixmap (XmHashKey key)\n{")
region = source[start:end]
assert "(uint32_t)value % (uint32_t)INT_MAX" in region
assert "(unsigned int)INT_MAX" in region
assert "return (XmHashValue)(uint32_t)value;" not in region
print("PASS ImageCache hash mixes are bounded before signed conversion")
