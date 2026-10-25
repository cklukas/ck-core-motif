#!/usr/bin/env python3
"""Check GeoUtils dimension traversal handles empty rows safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGeoGetDimensions(")
end = source.index("static Dimension \n_XmGeoStretchVertical", start)
body = source[start:end]
assert "geoSpec == NULL || geoSpec->boxes == NULL || geoSpec->layouts == NULL" in body
assert "if (numBoxes > 1 && layoutPtr->space_between != 0)" in body
assert "(unsigned long)(numBoxes - 1)" in body
assert "unsigned long   maxDimension" in body
assert "if ((unsigned long)rowW > maxDimension - extent)" in body
assert "fillWidth > maxDimension" in body
assert "betweenCount >" in body

print("GeoUtils dimension traversal guards passed")
