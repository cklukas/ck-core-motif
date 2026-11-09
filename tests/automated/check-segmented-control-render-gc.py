#!/usr/bin/env python3
"""Check modern SegmentedControl rendering guards temporary GCs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(
    encoding="utf-8"
)

focus_start = source.index("void _XmSegmentedControlDrawFocus(")
focus_end = source.index("\nBoolean _XmSegmentedControlDraw(", focus_start)
focus = source[focus_start:focus_end]
focus_create = "gc = XCreateGC(XtDisplay(child), XtWindow(child), GCForeground|GCLineWidth, &values);"
assert focus_create in focus and "if (!gc) return;" in focus
assert focus.index(focus_create) < focus.index("if (!gc) return;") < focus.index("XDrawRectangle")

draw_start = source.index("Boolean _XmSegmentedControlDraw(")
draw_end = source.index("\nstatic void Initialize", draw_start)
draw = source[draw_start:draw_end]
draw_create = "gc = XCreateGC(display, window, GCForeground, &values);"
assert draw_create in draw and "if (!gc) return True;" in draw
assert draw.index(draw_create) < draw.index("if (!gc) return True;") < draw.index("XFillRectangle")

print("PASS SegmentedControl temporary render GCs are guarded")
