#!/usr/bin/env python3
"""Check XmRegion GC application validates Xlib handles first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("_XmRegionSetGCRegion(")
start = source.index("void", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (dpy == NULL || gc == NULL)" in function
guard = function.index("dpy == NULL || gc == NULL")
assert guard < function.index("_XmRegionGetRectangles")
assert guard < function.index("XSetClipRectangles")
print("PASS XmRegion GC input guards")
