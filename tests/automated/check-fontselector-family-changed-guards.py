#!/usr/bin/env python3
"""Check FontSelector family-change callback state and cleanup guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("FamilyChanged(Widget w")
body = source[start:source.index("/*\tFunction Name: SizeChanged", start)]

assert "fsw == NULL || w == NULL" in body
assert "XmFontS_font_info(fsw) == NULL" in body
assert "current_font == NULL" in body
assert "if (str == NULL)" in body
assert "XtFree((XtPointer) str);" in body
assert body.index("if (str == NULL)") < body.index("XrmStringToQuark(str)")

print("FontSelector family-change callback is guarded and leak-safe")
