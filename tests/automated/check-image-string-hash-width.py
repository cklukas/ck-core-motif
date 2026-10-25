#!/usr/bin/env python3
"""Check ImageCache string hashing does not truncate strlen to unsigned int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("HashString (XmHashKey key)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "size_t len;" in function
assert "uintmax_t" in function
assert "folded_len" in function
assert "unsigned int len;" not in function
assert "data[len]" in function
print("PASS wide image-cache string hash length")
