#!/usr/bin/env python3
"""Check locale/text property aggregate sizing."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert text.count("SIZE_MAX / sizeof(char *)") >= 2
assert text.count("item_size > (size_t)INT_MAX - (size_t)total_size") >= 2
print("locale text property sizing contract passed")
