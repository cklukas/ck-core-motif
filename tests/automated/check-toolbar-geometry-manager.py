#!/usr/bin/env python3
"""Keep Toolbar's child geometry-manager result semantics precise."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(
    encoding="utf-8"
)
start = source.index("static XtGeometryResult GeometryManager(Widget child")
body = source[start:source.index("static XtGeometryResult QueryGeometry", start)]
assert "return XtGeometryAlmost;" in body
assert "return request->request_mode & XtCWQueryOnly ? XtGeometryYes : XtGeometryDone;" in body
assert body.index("return XtGeometryAlmost;") < body.index("return request->request_mode")
print("Toolbar completes matching child geometry requests")
