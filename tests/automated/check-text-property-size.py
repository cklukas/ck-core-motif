#!/usr/bin/env python3
"""Check aggregate text-property size and pointer-array bounds."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "SIZE_MAX / sizeof(char *)" in text
assert "item_size > (size_t)INT_MAX - (size_t)total_size" in text
assert "return BadAlloc;" in text
print("text property aggregate sizing contract passed")
