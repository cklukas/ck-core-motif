#!/usr/bin/env python3
"""Check Prompt text extents saturate before adding layout padding."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Prompt.c").read_text()
assert "static Dimension padded_extent(Dimension value, Dimension padding)" in source
assert "value > (Dimension)USHRT_MAX - padding" in source
assert "width = padded_extent(text_width, 8);" in source
assert "height = padded_extent(text_height, 8);" in source
assert "reply->height = padded_extent(height, 8);" in source
print("PASS Prompt padded extents are saturating")
