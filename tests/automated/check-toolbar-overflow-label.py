#!/usr/bin/env python3
"""Check Toolbar updates its realized overflow label in SetValues."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text()
initialize_start = source.index("static void Initialize(Widget request")
initialize = source[initialize_start:
                    source.index("static void Destroy(", initialize_start)]
assert "? toolbar->toolbar.overflow_label : \"\"" in initialize
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult GeometryManager", start)
body = source[start:end]
assert "old->toolbar.overflow_label != toolbar->toolbar.overflow_label" in body
assert "XmStringCreateLocalized" in body
assert "XmNlabelString" in body
assert "XtVaSetValues(toolbar->toolbar.overflow_button" in body
assert body.index("XtVaSetValues") < body.index("layout(toolbar)")
print("PASS Toolbar accepts null overflow labels at construction and on updates")
