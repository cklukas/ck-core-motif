#!/usr/bin/env python3
"""Ensure miRegionOp preserves the destination when scratch allocation fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("static void\nmiRegionOp")
end = source.index("/*======================================================================", start)
region = source[start:end]

assert "oldNumRects = newReg->numRects;" in region
assert "oldSize = newReg->size;" in region
failure = region.index("XtMalloc ((Cardinal) (sizeof(XmRegionBox) * newReg->size))")
restore = region.index("newReg->rects = oldRects;", failure)
assert "newReg->numRects = oldNumRects;" in region[restore:]
assert "newReg->size = oldSize;" in region[restore:]
print("PASS region operation allocation failure preserves destination state")
