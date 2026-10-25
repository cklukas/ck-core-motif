#!/usr/bin/env python3
"""Check DataGrid text baselines use active font metrics."""
from pathlib import Path

source = Path("lib/Xm/DataGrid.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "text_baseline(grid, top, height)" in source
assert "grid->data_grid.header_height,\n                  grid->data_grid.columns" in source
print("PASS DataGrid text uses measured font baselines")
