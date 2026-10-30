#!/usr/bin/env python3
"""Check ImageCache string hashing uses the final character, not the NUL."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("HashString (XmHashKey key)\n{")
end = source.index("#ifdef SVG_SUPPORTED", start)
region = source[start:end]
assert "size_t tail_index;" in region
assert "tail_index = len == 0 ? 0 : len - 1;" in region
assert "data[tail_index]" in region
assert "data[len])" not in region
print("PASS ImageCache string hash uses the final character")
