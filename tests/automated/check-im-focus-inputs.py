#!/usr/bin/env python3
"""Check input-method focus/value APIs validate their widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
for signature, marker in (
    ("XmImSetFocusValues(Widget w,", "_XmWidgetToAppContext(w)"),
    ("XmImSetValues(Widget w,", "_XmWidgetToAppContext(w)"),
    ("XmImUnsetFocus(Widget w)", "_XmWidgetToAppContext(w)"),
):
    start = source.index(signature)
    region = source[start:]
    assert "if (w == NULL)" in region
    assert region.index("if (w == NULL)") < region.index(marker)
print("PASS input-method focus/value APIs validate null widgets")
