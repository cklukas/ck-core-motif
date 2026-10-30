#!/usr/bin/env python3
"""Check Stepper live color resources update its drawing GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text()
start = source.rindex("static Boolean SetValues")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
assert "old->primitive.foreground != s->primitive.foreground" in body
assert "XSetForeground(XtDisplay(widget), s->stepper.gc" in body
assert "XSetBackground(XtDisplay(widget), s->stepper.gc" in body
assert "&& s->stepper.gc" in body
assert "redraw_needed = True" in body

print("Stepper color changes update its live drawing GC")
