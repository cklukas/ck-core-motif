#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/TreeView.c").read_text()
initialize_start = source.index("static void Initialize(Widget request")
initialize = source[initialize_start:source.index("static void Destroy", initialize_start)]
set_values_start = source.index("static Boolean SetValues(Widget old_widget")
set_values = source[set_values_start:source.index("static XtGeometryResult QueryGeometry", set_values_start)]
assert initialize.count("if (!view->tree_view.row_height)") == 1
assert "view->tree_view.row_height = 1;" in initialize
assert set_values.count("if (!view->tree_view.row_height)") == 1
assert "view->tree_view.row_height = 1;" in set_values
print("TreeView row-height guards present")
