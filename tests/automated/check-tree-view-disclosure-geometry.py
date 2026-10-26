#!/usr/bin/env python3
"""Ensure TreeView disclosure glyph strokes share one pixel centre."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TreeView.c").read_text(encoding="utf-8")
assert "const int top = y + (view->tree_view.row_height >= box_size ?" in source
assert "const int center_x = x + 4;" in source
assert "const int center_y = top + 4;" in source
assert "x + 2, center_y, x + 6" in source
assert "center_x, top + 2" in source
print("PASS TreeView disclosure cross uses a shared centre")
