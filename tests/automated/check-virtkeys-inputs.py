#!/usr/bin/env python3
"""Check virtual-key initialization validates its display handle first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(encoding="utf-8")
start = source.index("_XmVirtKeysInitialize")
region = source[start:]
assert "if (widget == NULL || !XmIsDisplay(widget))" in region
assert region.index("if (widget == NULL") < region.index("xmDisplay = (XmDisplay) widget;")
assert region.index("xmDisplay = (XmDisplay) widget;") < region.index("dpy = XtDisplay(xmDisplay);")
print("PASS virtual-key initialization validates display widgets first")
