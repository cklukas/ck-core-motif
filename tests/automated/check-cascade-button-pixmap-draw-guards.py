#!/usr/bin/env python3
"""Check XmCascadeButton validates cascade pixmap draw prerequisites."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeB.c").read_text()
start = source.index("static void \nDrawCascade(")
end = source.index("/*\n * redisplay the widget", start)
draw = source[start:end]

assert "CB_Cascade_height(cb) != 0" in draw
assert "XtIsRealized((Widget)cb)" in draw
assert "pixmap == XmUNSPECIFIED_PIXMAP" in draw
assert "cb->label.normal_GC == NULL" in draw
print("PASS CascadeButton pixmap draw guards")
