#!/usr/bin/env python3
"""Check bounded XmDataGrid text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(encoding="utf-8")
start = source.index("static void draw_text")
end = source.index("static int column_width", start)
region = source[start:end]

assert "if (!grid->data_grid.gc || !grid->data_grid.render_table) return;" in region
assert "XmStringExtent(grid->data_grid.render_table, value" in region
assert "XmStringDraw(XtDisplay((Widget)grid)" in region
print("PASS data-grid text uses guarded render-table metrics")
