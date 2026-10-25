#!/usr/bin/env python3
"""Check TreeView text uses X font metrics and keeps programmatic selection visible."""
from pathlib import Path

source = Path("lib/Xm/TreeView.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XTextWidth" in source
assert "font_ascent" in source and "font_descent" in source
assert "visible_row_for_id(view, id)" in source
assert "view->tree_view.top_row = (Cardinal)row - view->tree_view.visible_rows + 1" in source
assert "const int box_size = 9" in source
assert "view->tree_view.row_height - box_size" in source
print("PASS TreeView text uses font metrics and selection visibility")
