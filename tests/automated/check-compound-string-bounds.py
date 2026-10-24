#!/usr/bin/env python3
"""Check compound-string text-property sizing and termination."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "item_size < 0 || item_size > INT_MAX - total_size" in text
assert "sizeof(unsigned char) * ((size_t)total_size + 1)" in text
assert "*ubufptr = '\\0';" in text
print("compound string conversion bounds contract passed")
