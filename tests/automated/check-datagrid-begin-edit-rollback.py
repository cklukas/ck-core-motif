#!/usr/bin/env python3
"""Check DataGrid begin-edit allocation is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text()
start = source.index("Boolean XmDataGridBeginEdit")
end = source.index("Boolean XmDataGridSetEditText", start)
region = source[start:end]

assert "char *copy;" in region
assert "copy = XtNewString(value ? value : \"\");" in region
assert "if (!copy) return False;" in region
assert region.index("XtNewString(value ? value : \"\")") < region.index("XtFree(grid->data_grid.edit_text)")
assert "grid->data_grid.edit_text = copy;" in region
print("DataGrid preserves edit state on begin-edit allocation failure")
