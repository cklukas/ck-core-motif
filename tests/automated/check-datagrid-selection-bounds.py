#!/usr/bin/env python3
"""Guard DataGrid selection consumers against stale out-of-range indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(
    encoding="utf-8"
)
for function in ("Boolean XmDataGridGetSelectedCell", "Boolean XmDataGridBeginEdit"):
    start = text.index(function)
    end = text.index("\n}\n", start) + 2
    body = text[start:end]
    for needle in (
        "(Cardinal)grid->data_grid.selected_row >= grid->data_grid.row_count",
        "(Cardinal)grid->data_grid.selected_column >= grid->data_grid.column_count",
    ):
        if needle not in body:
            raise SystemExit(f"{function} missing bounds check: {needle}")

print("DataGrid selection bounds checks passed")
