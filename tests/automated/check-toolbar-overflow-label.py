#!/usr/bin/env python3
"""Check Toolbar updates its realized overflow label in SetValues."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult GeometryManager", start)
body = source[start:end]
assert "old->toolbar.overflow_label != toolbar->toolbar.overflow_label" in body
assert "XmStringCreateLocalized" in body
assert "XmNlabelString" in body
assert "XtVaSetValues(toolbar->toolbar.overflow_button" in body
assert body.index("XtVaSetValues") < body.index("layout(toolbar)")
print("PASS Toolbar overflow label updates on resource changes")
