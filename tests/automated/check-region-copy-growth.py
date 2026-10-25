#!/usr/bin/env python3
"""Check region-copy growth preserves the destination on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
assert "#include <stdint.h>" in source
start = source.index("static void\nmiRegionCopy(")
start = source.rfind("\n", 0, start) + 1
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "XmRegionBox *new_rects;" in region
assert "SIZE_MAX / sizeof(XmRegionBox)" in region
assert "new_rects = (XmRegionBox *)XtRealloc" in region
assert "if (new_rects == NULL)" in region
assert region.index("new_rects =") < region.index("dstrgn->rects = new_rects")
assert "(size_t) rgn->numRects * sizeof(XmRegionBox)" in region
print("PASS region-copy transactional growth")
