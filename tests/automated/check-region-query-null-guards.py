#!/usr/bin/env python3
"""Check XmRegion geometry queries and mutations tolerate NULL inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()

def function(name: str, next_name: str) -> str:
    start = source.index(name + "(")
    start = source.index("\n{", start) + 2
    end = source.index(next_name + "(", start)
    return source[start:end]

clear = function("_XmRegionClear", "_XmRegionIsEmpty")
assert "if (r == NULL)" in clear

empty = function("_XmRegionIsEmpty", "_XmRegionEqual")
assert "r == NULL" in empty

equal = function("_XmRegionEqual", "_XmRegionPointInRegion")
assert "if (r1 == NULL || r2 == NULL)" in equal
assert "return (r1 == r2);" in equal

point = function("_XmRegionPointInRegion", "_XmRegionOffset")
assert "r == NULL || ISEMPTY(r)" in point

offset = function("_XmRegionOffset", "_XmRegionGetExtents")
assert "if (r == NULL)" in offset

extents = function("_XmRegionGetExtents", "_XmRegionGetNumRectangles")
assert "if (r == NULL || rect == NULL)" in extents

count = function("_XmRegionGetNumRectangles", "_XmRegionGetRectangles")
assert "return (r == NULL) ? 0L : r->numRects;" in count

print("PASS XmRegion query and mutation NULL guards")
