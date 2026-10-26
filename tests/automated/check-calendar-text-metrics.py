#!/usr/bin/env python3
"""Check Calendar labels use measured font baselines."""
from pathlib import Path

source = Path("lib/Xm/Calendar.c").read_text(encoding="utf-8")
assert "XmStringExtent(calendar->calendar.render_table, value" in source
assert "top + ((int)height - (int)text_height) / 2" in source
assert "XmStringDraw(XtDisplay((Widget)calendar)" in source
print("PASS Calendar text uses render-table metrics")
