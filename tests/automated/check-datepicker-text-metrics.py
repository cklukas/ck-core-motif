#!/usr/bin/env python3
"""Check DatePicker text uses active font metrics."""
from pathlib import Path

source = Path("lib/Xm/DatePicker.c").read_text(encoding="utf-8")
assert "XmStringExtent(picker->date_picker.render_table, value" in source
assert "((int)picker->core.height - (int)height) / 2" in source
assert "XmStringDraw(XtDisplay(widget)" in source
print("PASS DatePicker text uses render-table metrics")
