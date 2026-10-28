#!/usr/bin/env python3
"""Ensure DataGrid navigation cannot wrap its top-row arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(
    encoding="utf-8"
)
assert "static Cardinal add_top_row(XmDataGridWidget grid, Cardinal delta)" in source
assert "delta > maximum - grid->data_grid.top_row" in source
assert "(Cardinal)row - grid->data_grid.top_row >= grid->data_grid.visible_rows" in source
assert "add_top_row(grid, grid->data_grid.visible_rows)" in source
assert "add_top_row(grid, 1)" in source
print("PASS DataGrid top-row navigation saturates")
