#!/usr/bin/env python3
"""Check successful DataGrid model replacement releases edit buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean copy_model(")
end = source.index("static Cardinal max_top_row", start)
body = source[start:end]

assert "XtFree(grid->data_grid.edit_text);" in body
assert "XtFree(grid->data_grid.edit_error);" in body
assert "grid->data_grid.edit_text = NULL;" in body
assert "grid->data_grid.edit_error = NULL;" in body
assert body.index("XtFree(grid->data_grid.edit_text);") < body.index(
    "grid->data_grid.editing = False;"
)
assert body.index("XtFree(grid->data_grid.edit_error);") < body.index(
    "grid->data_grid.editing = False;"
)

print("PASS DataGrid model replacement releases edit buffers")
