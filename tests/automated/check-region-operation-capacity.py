#!/usr/bin/env python3
"""Check that region operations validate doubled rectangle capacity."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("long max_rects = MAX(reg1->numRects, reg2->numRects);")
end = source.index("\n    if (! (newReg->rects", start)
region = source[start:end]
assert "LONG_MAX / 2" in region
assert "UINT_MAX / (2 * sizeof(XmRegionBox))" in region
assert "newReg->size = max_rects * 2;" in region
print("region operation capacity growth is checked")
