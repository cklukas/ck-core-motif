#!/usr/bin/env python3
"""Check GeoUtils fill spacing handles empty and zero-spec rows."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGeoCalcFill(", 10000)
end = source.index("static void \nSegmentFill", start)
body = source[start:end]
assert "unsigned long   totalSpecSpace" in body
assert "if (pEndSpace == NULL || pBetweenSpace == NULL)" in body
assert "if (numBoxes == 0)" in body
assert "if (totalSpecSpace == 0)" in body
assert "(unsigned long)betweenSpec" in body

print("GeoUtils fill-spacing guards passed")
