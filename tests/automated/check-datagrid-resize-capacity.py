#!/usr/bin/env python3
"""Check that DataGrid refreshes capacity and painting after geometry changes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c"
text = source.read_text(encoding="utf-8")

assert "static void Resize(Widget);" in text
assert "Destroy, Resize, BufferedRedisplay" in text
resize_start = text.index("static void Resize(Widget widget)")
resize_end = text.index("\nstatic Boolean SetValues", resize_start)
assert "recompute_visible((XmDataGridWidget)widget);" in text[resize_start:resize_end]

set_start = text.index("static Boolean SetValues(Widget old_widget")
set_end = text.index("\nstatic void draw_text", set_start)
set_values = text[set_start:set_end]
assert "Boolean geometry_changed = old->core.width != grid->core.width ||" in set_values
assert "old->core.height != grid->core.height;" in set_values
assert "render_changed || colors_changed || geometry_changed;" in set_values

print("DataGrid resize refreshes capacity and geometry repaint")
