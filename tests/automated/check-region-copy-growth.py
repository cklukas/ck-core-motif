#!/usr/bin/env python3
"""Check XmRegion copy growth preserves storage on realloc failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("static void\nmiRegionCopy(")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "new_rects = (XmRegionBox *)XtRealloc(" in region
assert "if (dstrgn->rects)" not in region
assert "if (new_rects == NULL)" in region
assert region.index("if (new_rects == NULL)") < region.index("dstrgn->rects = new_rects")
assert "SIZE_MAX / sizeof(XmRegionBox)" in region
assert "UINT_MAX / sizeof(XmRegionBox)" in region
assert "dstrgn->rects = (XmRegionBox *)\n\t\t       XtRealloc" not in region
print("PASS XmRegion copy-growth transaction")
