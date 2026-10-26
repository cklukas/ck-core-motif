#!/usr/bin/env python3
"""Check bounded XmTreeView RTL label geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text(encoding="utf-8")
start = source.index("long long depth_offset")
end = source.index("if (label_string)", start)
region = source[start:end]

assert "Dimension label_width = 0, label_height = 0" in region
assert "XmStringExtent" in region
assert "long long rtl_x" in region
assert "rtl_x < INT_MIN" in region
assert "rtl_x > INT_MAX" in region
assert "item_x < INT_MIN" in source
print("PASS tree-view RTL label geometry is bounded")
