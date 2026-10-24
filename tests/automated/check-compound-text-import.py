#!/usr/bin/env python3
"""Check bounded compound-text property parsing."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "memchr(text_prop->value + offset, '\\0'" in text
assert "segment_length = nul ?" in text
assert "text_prop->nitems > (unsigned long)INT_MAX" in text
print("compound text import bounds contract passed")
