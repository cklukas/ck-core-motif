#!/usr/bin/env python3
"""Check that zero row height selects measured geometry, not a one-pixel row."""
from pathlib import Path
source=(Path(__file__).parents[2]/"lib/Xm/TreeView.c").read_text()
assert "Offset(tree_view.row_height), XmRImmediate, (XtPointer)0" in source
assert "automatic_row_height = view->tree_view.row_height == 0" in source
assert "natural_metrics(view,&width,&view->tree_view.row_height)" in source
assert "MIN(MAX(tallest,1),USHRT_MAX)" in source
print("PASS TreeView automatic row height is measured and bounded")
