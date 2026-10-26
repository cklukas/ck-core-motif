#!/usr/bin/env python3
"""Check DataGrid text baselines use active font metrics."""
from pathlib import Path

source = Path("lib/Xm/DataGrid.c").read_text(encoding="utf-8")
start = source.index("static void draw_text")
end = source.index("static int column_width", start)
draw_text = source[start:end]
assert "XmStringExtent(grid->data_grid.render_table" in draw_text
assert "text_height" in draw_text
assert "XmStringDraw" in draw_text
assert "top + ((int)height - (int)text_height) / 2" in draw_text
print("PASS DataGrid text uses render-table metrics")
