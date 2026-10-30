#!/usr/bin/env python3
"""Check Primitive trait getters validate widget and output state."""

from pathlib import Path


source = Path("lib/Xm/Primitive.c").read_text(encoding="utf-8")
start = source.rindex("GetDirection(Widget w)")
end = source.index("\n}", start)
direction = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmPrimitiveWidgetClass)" in direction
assert "return XmDEFAULT_DIRECTION" in direction

start = source.rindex("GetColors(Widget w")
end = source.index("\n}", start)
colors = source[start:end]
assert "w == NULL || color_data == NULL" in colors
assert "XtIsSubclass(w, xmPrimitiveWidgetClass)" in colors

start = source.rindex("GetUnitType(Widget w)")
end = source.index("\n}", start)
unit = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmPrimitiveWidgetClass)" in unit
assert "return XmPIXELS" in unit

print("Primitive trait getter guards present")
