#!/usr/bin/env python3
"""Check ColorPicker value text uses active font metrics."""
from pathlib import Path

source = Path("lib/Xm/ColorPicker.c").read_text(encoding="utf-8")
assert "XmStringExtent(picker->color_picker.render_table, value_string" in source
assert "XmStringDraw(display, window, picker->color_picker.render_table" in source
assert "XmFontListCopy" in source
print("PASS ColorPicker text uses measured font baseline")
