#!/usr/bin/env python3
"""Check TabBox handles a missing internal canvas safely."""

from pathlib import Path


source = Path("lib/Xm/TabBox.c").read_text()

initialize_start = source.index("Initialize(Widget request")
initialize = source[initialize_start:source.index("Realize(Widget widget", initialize_start)]
assert "XmTabBox__canvas(st) == NULL" in initialize
assert initialize.index("XtCreateManagedWidget(\"canvas\"") < initialize.index("XmTabBox__canvas(st) == NULL")
assert initialize.index("XmTabBox__canvas(st) == NULL") < initialize.index("XmTabBox__bitmap(st)")

resize_start = source.index("Resize(Widget widget")
resize = source[resize_start:source.index("Redisplay(Widget widget", resize_start)]
assert "XmTabBox__canvas(tab) == NULL" in resize
assert resize.index("XmTabBox__canvas(tab) == NULL") < resize.index("Layout(tab)")

draw_start = source.index("DrawTab(XmTabBoxWidget tab")
draw = source[draw_start:source.index("#define XiCvtDone", draw_start)]
assert "canvas == NULL" in draw

print("TabBox canvas allocation failures are null-safe")
