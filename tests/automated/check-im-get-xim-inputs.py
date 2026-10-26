#!/usr/bin/env python3
"""Check XmImGetXIM validates its widget before app-context lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImGetXIM(Widget w)")
region = source[start:]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("_XmWidgetToAppContext(w)")
print("PASS XmImGetXIM validates null widgets")
