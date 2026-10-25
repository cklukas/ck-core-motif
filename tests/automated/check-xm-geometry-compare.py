#!/usr/bin/env python3
"""Check Xm geometry comparison correctness and null safety."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text(encoding="utf-8")
start = source.index("XmCompareXtWidgetGeometryToWidget")
end = source.index("XmCompareXtWidgetGeometry(XtWidgetGeometry", start)
widget_body = source[start:end]
assert "if (geom == NULL || widget == NULL)" in widget_body
assert "geom->width != XtWidth(widget)" in widget_body
assert "geom->width != XtHeight(widget)" not in widget_body
start = end
body = source[start:source.index("/************************************************************", start)]
assert "if (geom1 == NULL || geom2 == NULL)" in body
print("Xm geometry comparisons validate inputs and compare width correctly")
