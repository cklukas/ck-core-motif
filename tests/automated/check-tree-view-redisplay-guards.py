#!/usr/bin/env python3
"""Check XmTreeView Redisplay handles missing data and small geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static int row_at(", start)
function = source[start:end]

assert "!window || !view->tree_view.gc" in function
assert 'label = view->tree_view.items[index].label ?' in function
assert "view->core.width > 3 ? view->core.width - 3 : 1" in function
assert "view->tree_view.row_height > 3" in function
assert "view->core.width > 2 * view->primitive.highlight_thickness" in function
print("PASS TreeView redisplay guards")
