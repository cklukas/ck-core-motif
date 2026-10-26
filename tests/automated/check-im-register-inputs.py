#!/usr/bin/env python3
"""Check XmImRegister validates its widget before parent traversal."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImRegister(Widget w,")
region = source[start:]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("_XmWidgetToAppContext(w)")
assert region.index("if (w == NULL)") < region.index("XtParent(w)")
print("PASS XmImRegister validates null widgets")
