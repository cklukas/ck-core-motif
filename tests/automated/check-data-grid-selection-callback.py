#!/usr/bin/env python3
"""Check DataGrid selection callbacks require an actual cell change."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text()
start = source.index("Boolean XmDataGridSetSelectedCell")
end = source.index("Boolean XmDataGridGetSelectedCell", start)
function = source[start:end]
assert "Boolean changed;" in function
assert "grid->data_grid.selected_row != row" in function
assert "if (notify && changed) notify_selection(grid, NULL);" in function
print("DataGrid selection callbacks require an actual cell change")
