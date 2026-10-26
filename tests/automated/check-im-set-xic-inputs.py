#!/usr/bin/env python3
"""Check XmImSetXIC validates its widget before app-context lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImSetXIC(Widget widget,")
region = source[start:]
assert "if (widget == NULL)" in region
assert region.index("if (widget == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS XmImSetXIC validates null widgets")
