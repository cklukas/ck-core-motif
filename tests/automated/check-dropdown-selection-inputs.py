#!/usr/bin/env python3
"""Check DropDown list-to-text conversion validates selected item inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("static Boolean\nSetTextFromList")
end = source.index("\n}\n", start)
body = source[start:end]

assert "if (count > 0 && items == NULL)" in body
assert "(size_t)count > UINT_MAX / sizeof(*parts)" in body
assert "(Cardinal)sizeof(*parts)" in body
assert "if (parts[i] == NULL)" in body
assert body.index("if (parts[i] == NULL)") < body.index("len = mbstowcs")
assert "separator > (size_t)UINT_MAX - total - len" in body
assert "XtMalloc((Cardinal)total)" in body

getter = source[source.index("XmDropDownGetValue(Widget w)") :]
assert "if (w == NULL || !XtIsSubclass(w, xmDropDownWidgetClass))" in getter

print("DropDown list-to-text conversion guards selected-item allocation bounds")
