#!/usr/bin/env python3
"""Check TreeView preferred-height multiplication is bounded."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/TreeView.c"
text = source.read_text(encoding="utf-8")

start = text.index("static XtGeometryResult QueryGeometry")
body = text[start:text.index("Widget XmCreateTreeView", start)]
assert "unsigned long height;" in body
assert "(unsigned long)view->tree_view.row_height" in body
assert "height > (unsigned long)(Dimension)-1" in body
assert "reply->height = height" in body
print("TreeView preferred height is bounded")
