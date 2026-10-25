#!/usr/bin/env python3
"""Check the display-scale helper split and declarations."""

from pathlib import Path


root = Path(__file__).parents[2]
display = (root / "lib" / "Xm" / "Display.c").read_text(encoding="utf-8")
header = (root / "lib" / "Xm" / "XmI.h").read_text(encoding="utf-8")
assert display.count("_XmGetGlobalDpiScale(Display *display, double *scale)") == 1
assert display.count("_XmGetDpiScale(Display *display, double *scale)") == 1
assert "extern Boolean _XmGetGlobalDpiScale(Display *display, double *scale);" in header
print("display-scale helper interface present")
