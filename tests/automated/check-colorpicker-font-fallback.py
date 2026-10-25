#!/usr/bin/env python3
"""Check ColorPicker text has a drawable fallback font."""
from pathlib import Path

source = Path("lib/Xm/ColorPicker.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(XtDisplay(widget), "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(XtDisplay(widget), picker->color_picker.font)" in source
print("PASS ColorPicker installs and releases a fallback text font")
