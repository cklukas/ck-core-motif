#!/usr/bin/env python3
"""Check XmDragStart validates widget and event inputs before lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text(encoding="utf-8")
start = source.index("XmDragStart(\n        Widget")
end = source.index("static void \nDragCancel", start)
body = source[start:end]

assert "numArgs != 0 && args == NULL" in body
assert "dd = (XmDisplay) XmGetXmDisplay(XtDisplay(w));" in body
assert body.index("w == NULL") < body.index("XtDisplay(w)")
assert body.index("dd == NULL") < body.index("_XmWidgetToAppContext")
assert "w == NULL || event == NULL ||" in body
assert body.index("w == NULL || event == NULL ||") < body.index("_XmWidgetToAppContext(w)")
assert body.index("w == NULL || event == NULL ||") < body.index("event->type")

print("PASS XmDragStart validates widget, event, and argument inputs")
