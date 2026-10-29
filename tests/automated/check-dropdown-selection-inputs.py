#!/usr/bin/env python3
"""Check DropDown list-to-text conversion validates selected item inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("static Boolean\nSetTextFromList")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (count > 0 && items == NULL)" in body
assert "if (ptr == NULL)" in body
assert body.index("if (ptr == NULL)") < body.index("len = mbstowcs")

getter = source[source.index("XmDropDownGetValue(Widget w)") :]
assert "if (w == NULL || !XtIsSubclass(w, xmDropDownWidgetClass))" in getter

print("DropDown list-to-text conversion guards missing selected item data")
