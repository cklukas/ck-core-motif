#!/usr/bin/env python3
"""Check scaled-image lookup tables have one matching cleanup each."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("\n\n\n/*** COLOR CACHE NOW ***/", start)
region = source[start:end]
cleanup = region[region.index("cleanup:"):]
for name in ("x", "y", "width", "height"):
    assert cleanup.count(f"XtFree((char *)table.{name});") == 1, name
assert "XtFree((char *)table.y);\n    XtFree((char *)table.width);" in cleanup
assert "XtFree((char *)table.y);\n    XtFree((char *)table.y);" not in cleanup
print("PASS ImageCache scaled-table cleanup")
