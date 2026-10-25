#!/usr/bin/env python3
"""Check that legacy shadow rectangle-buffer growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("void _XmDrawShadow")
end = source.index("/************************************************************************", start + 1)
region = source[start:end]

assert "new_rects = (XRectangle *) XtMalloc" in region
assert "new_rects = (XRectangle *) XtRealloc" in region
assert region.count("if (new_rects == NULL) return;") == 2
assert "rects = new_rects;" in region
print("PASS legacy shadow rectangle allocation is guarded")
