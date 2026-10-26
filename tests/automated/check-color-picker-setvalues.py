#!/usr/bin/env python3
"""Check ColorPicker applies all SetValues changes before returning."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ColorPicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
context = source[start:end]
required = (
    "Boolean redraw = False;",
    "Boolean colors_changed =",
    "XSetForeground(XtDisplay(widget), picker->color_picker.gc,",
    "if (new_table)",
    "picker->color_picker.render_table = old->color_picker.render_table;",
    "redraw = True;",
    "return redraw;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("ColorPicker SetValues contract missing: " + ", ".join(missing))
if "return True;" in context:
    raise SystemExit("ColorPicker SetValues must not return early for one resource")

print("ColorPicker SetValues check passed")
