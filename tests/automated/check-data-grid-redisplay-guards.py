#!/usr/bin/env python3
"""Check XmDataGrid Redisplay handles missing data and small geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text()
draw = source[source.index("static void draw_text"):source.index("static int column_width")]
start = source.rindex("static void Redisplay(")
end = source.index("static void move_selection(", start)
function = source[start:end]

assert "if (!grid->data_grid.gc || !grid->data_grid.render_table) return;" in draw
assert 'if (!text) text = "";' in draw
assert "!XtIsRealized(widget) || !grid->data_grid.gc" in function
assert "cell_width > 5 ? cell_width - 5 : 1" in function
assert "grid->data_grid.row_height > 3" in function
print("PASS DataGrid redisplay guards")
