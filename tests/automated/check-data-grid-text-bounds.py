#!/usr/bin/env python3
"""Check bounded XmDataGrid text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(encoding="utf-8")
start = source.index("static void draw_text")
end = source.index("static int column_width", start)
region = source[start:end]

assert "length > (size_t)INT_MAX" in region
assert "XDrawString" in region
print("PASS data-grid text lengths are bounded")
