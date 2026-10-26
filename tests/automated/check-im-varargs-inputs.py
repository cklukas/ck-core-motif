#!/usr/bin/env python3
"""Check input-method varargs setters validate their widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
for signature in ("XmImVaSetFocusValues(Widget w,", "XmImVaSetValues(Widget w,"):
    start = source.index(signature)
    region = source[start:]
    assert "if (w == NULL)" in region
    assert region.index("if (w == NULL)") < region.index("_XmWidgetToAppContext(w)")
print("PASS input-method varargs setters validate null widgets")
