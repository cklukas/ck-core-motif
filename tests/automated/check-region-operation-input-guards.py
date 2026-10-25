#!/usr/bin/env python3
"""Check XmRegion operations define safe NULL source/destination behavior."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()

def body(name: str, next_name: str) -> str:
    start = source.index(name + "(")
    start = source.index("\n{", start) + 2
    end = source.index(next_name + "(", start)
    return source[start:end]

intersect = body("_XmRegionIntersect", "_XmRegionIntersectRectWithRegion")
assert "if (newReg == NULL)" in intersect
assert "reg1 == NULL || reg2 == NULL" in intersect
assert intersect.index("newReg == NULL") < intersect.index("ISEMPTY")

intersect_rect = body("_XmRegionIntersectRectWithRegion", "_XmRegionUnion")
assert "rect == NULL || dest == NULL" in intersect_rect
assert "source == NULL" in intersect_rect

union = body("_XmRegionUnion", "_XmRegionUnionRectWithRegion")
assert "if (newReg == NULL)" in union
assert "if (reg1 == NULL)" in union
assert "if (reg2 == NULL)" in union

union_rect = body("_XmRegionUnionRectWithRegion", "_XmRegionSubtract")
assert "rect == NULL || dest == NULL" in union_rect

subtract = body("_XmRegionSubtract", "_XmRegionShrink")
assert "if (regD == NULL)" in subtract
assert "if (regM == NULL)" in subtract
assert "if (regS == NULL)" in subtract

shrink = body("_XmRegionShrink", "CreateLeftShadow")
assert "r == NULL || (!dx && !dy)" in shrink

print("PASS XmRegion operation input guards")
