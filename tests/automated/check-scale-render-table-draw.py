#!/usr/bin/env python3
"""Ensure XmScale's non-Xft value path preserves render-table text."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Scale.c").read_text()
start = source.index("#else\n    /* Keep the legacy XFontStruct metrics")
end = source.index("#endif\n}", start)
draw = source[start:end]

required = (
    "if (sw->scale.font_list)",
    "XmStringCreateLocalized(buffer)",
    "XmStringDraw(XtDisplay(sw), XtWindow(sw), sw->scale.font_list",
    "XmStringFree(value)",
    "XDrawImageString(XtDisplay(sw), XtWindow(sw)",
)
missing = [item for item in required if item not in draw]
if missing:
    raise SystemExit("XmScale render-table draw contract missing: " + ", ".join(missing))

print("PASS XmScale value labels use render-table drawing when available")
