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
assert "if (numBoxes > 1)" in body
assert "(numBoxes - 1) * layoutPtr->space_between" in body

print("GeoUtils dimension traversal guards passed")
