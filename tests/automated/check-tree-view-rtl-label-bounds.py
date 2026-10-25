#!/usr/bin/env python3
"""Check bounded XmTreeView RTL label geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text(encoding="utf-8")
start = source.index("size_t label_length = strlen(label)")
end = source.index("XDrawString", start)
region = source[start:end]

assert "label_length > (size_t)(INT_MAX - 4) / 8" in region
assert "long long rtl_x" in region
assert "draw_length" in region
assert "long long depth_offset" in source
assert "item_x < INT_MIN" in source
print("PASS tree-view RTL label geometry is bounded")
