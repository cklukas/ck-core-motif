#!/usr/bin/env python3
"""Check Text source block lengths are bounded before legacy narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
prototype = source.index("_XmStringSourceReadString(")
start = source.index("_XmStringSourceReadString(", prototype + 1)
end = source.index("static XmTextPosition", start)
body = source[start:end]

assert "size_t prefix_bytes" in body
assert "prefix_bytes > (size_t)INT_MAX" in body
assert "block->length = (int)prefix_bytes;" in body
assert "block->length = data->gap_start" not in body

print("Text source block length narrowing is bounded")
