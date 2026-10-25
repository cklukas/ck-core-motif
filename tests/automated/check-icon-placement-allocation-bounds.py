#!/usr/bin/env python3
"""Check checked size arithmetic for Mwm icon-placement arrays."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmIPlace.c").read_text(
    encoding="utf-8"
)
assert "size_t rowCount;" in source
assert "rowCount > SIZE_MAX / sizeof(int)" in source
assert "rowBytes > UINT_MAX" in source
assert "totalPlaces > (size_t)INT_MAX" in source
assert "placeBytes > UINT_MAX" in source
assert "XtMalloc ((Cardinal)placeBytes)" in source
assert "pWS->IPData.totalPlaces * sizeof (IconInfo)" not in source
print("Mwm icon-placement array sizes are checked before allocation")
