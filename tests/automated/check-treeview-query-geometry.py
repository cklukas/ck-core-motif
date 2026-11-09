#!/usr/bin/env python3
"""Check TreeView retains explicit per-axis geometry."""

from pathlib import Path


root = Path(__file__).parents[2]
private = (root / "lib" / "Xm" / "TreeViewP.h").read_text(encoding="utf-8")
source = (root / "lib" / "Xm" / "TreeView.c").read_text(encoding="utf-8")

assert "Boolean automatic_width;" in private
assert "Boolean automatic_height;" in private
assert "view->tree_view.automatic_width = ((XmTreeViewWidget)request)->core.width == 0;" in source

set_start = source.index("static Boolean SetValues(Widget old_widget")
set_end = source.index("\nstatic XtGeometryResult QueryGeometry", set_start)
set_values = source[set_start:set_end]
assert "if (!strcmp(args[i].name,XmNwidth))" in set_values
assert "view->tree_view.automatic_width = (Dimension)args[i].value == 0;" in set_values
assert "natural_metrics(view, &view->core.width, &height);" in set_values

query_start = source.index("static XtGeometryResult QueryGeometry(Widget widget")
query_end = source.index("\nWidget XmCreateTreeView", query_start)
query = source[query_start:query_end]
assert "if (!view->tree_view.automatic_width)" in query
assert "reply->width = view->core.width;" in query
assert "if (!view->tree_view.automatic_height)" in query
assert "reply->height = view->core.height;" in query
assert "data->value > 0 ? (Cardinal)data->value : 0" in source
assert "(unsigned short)extent,(unsigned short)extent" in source

print("TreeView QueryGeometry preserves explicit width and height")
