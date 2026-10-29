#!/usr/bin/env python3
"""Check DropDown geometry negotiation rejects unsafe partial requests."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("static XtGeometryResult\nGeometryManager")
body = source[start:source.index("static void\nQueryChildSize", start)]
assert "*result = (XtWidgetGeometry){0};" in body
assert "request->request_mode & ~(CWWidth | CWHeight | XtCWQueryOnly)" in body
assert "if (c_height != r_height)" in body
assert "w->core.width = old_width;" in body
assert "w->core.height = old_height;" in body
assert "QueryChildSize(XmDropDown_label(cbw), label);" in source
assert "QueryChildSize(XmDropDown_text(cbw), text);" in source
assert "QueryChildSize(XmDropDown_arrow(cbw), arrow);" in source
print("PASS DropDown negotiation guards and request-mode handling")
