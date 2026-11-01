#!/usr/bin/env python3
"""Check TreeView text uses X font metrics and keeps programmatic selection visible."""
from pathlib import Path

source = Path("lib/Xm/TreeView.c").read_text(encoding="utf-8")
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmFontListCopy" in source
assert "XmRenderTableFree" in source
assert "if (new_table)" in source
assert "view->tree_view.render_table = old->tree_view.render_table" in source
assert "visible_row_for_id(view,view->tree_view.selected_id)" in source
assert "view->tree_view.top_row = (Cardinal)row - viewport_rows(view) + 1" in source
assert "indicator=9" in source
print("PASS TreeView text uses font metrics and selection visibility")
