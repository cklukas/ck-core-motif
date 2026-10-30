#!/usr/bin/env python3
"""Check RatingControl color resources request a live redraw."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
body = source[start:end]
assert "Boolean colors_changed=" in body
assert "old->primitive.highlight_color!=rating->primitive.highlight_color" in body
assert "XSetForeground(XtDisplay(widget),rating->rating.gc" in body
assert "if(colors_changed&&rating->rating.gc)" in body
assert "redraw=True" in body

print("RatingControl color changes update its live drawing GC")
