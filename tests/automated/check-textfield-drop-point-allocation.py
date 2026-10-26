#!/usr/bin/env python3
"""Check TextField drag/drop point allocation before coordinate writes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nStuff(Widget w")
end = source.index("\nstatic void", start + 20)
body = source[start:end]

assert "point = (XPoint *) XtMalloc(sizeof(XPoint));" in body
assert "if (point != NULL)" in body
assert "point->x = event->xbutton.x;" in body
assert "point->y = event->xbutton.y;" in body

print("TextField drag/drop point writes are allocation-safe")
