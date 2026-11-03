#!/usr/bin/env python3
"""Check that Sidebar title replacement preserves the old owned XmString."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c"
text = source.read_text(encoding="utf-8")
start = text.index("static Boolean SetValues(Widget old_widget")
end = text.index("\nstatic XtGeometryResult GeometryManager", start)
set_values = text[start:end]

assert "XmString replacement = sidebar->sidebar.title ?" in set_values
assert "if (replacement != NULL)" in set_values
assert set_values.index("if (replacement != NULL)") < set_values.index(
    "XmStringFree(old->sidebar.title);"
)
assert "sidebar->sidebar.title = old->sidebar.title;" in set_values
assert "if (sidebar->sidebar.title_label != NULL)" in set_values
assert "return title_changed;" in set_values

print("Sidebar title replacement retains the old owned XmString on failure")
