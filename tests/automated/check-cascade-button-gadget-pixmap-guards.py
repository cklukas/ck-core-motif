#!/usr/bin/env python3
"""Check XmCascadeButtonGadget validates cascade pixmap copies."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text()
start = source.index("static void \nDrawCascade(")
end = source.index("/*\n * redisplay the widget", start)
draw = source[start:end]

assert "XtIsRealized(XtParent(cb))" in draw
assert "CBG_Cascade_height(cb) == 0" in draw
assert "pixmap == XmUNSPECIFIED_PIXMAP" in draw
assert "LabG_NormalGC(cb) == NULL" in draw
print("PASS CascadeButtonGadget pixmap guards")
