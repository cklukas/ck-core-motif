#!/usr/bin/env python3
"""Check MWM icon-list growth computes byte counts without signed overflow."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmIconBox.c").read_text(
    encoding="utf-8"
)
assert "size_t places_bytes;" in source
assert "(size_t)pIBD->IPD.totalPlaces > SIZE_MAX / sizeof(IconInfo)" in source
assert "places_bytes = (size_t)pIBD->IPD.totalPlaces * sizeof(IconInfo);" in source
assert "XtMalloc((Cardinal)places_bytes)" in source
start = source.index("Boolean ExtendIconList")
end = source.index("} /* END OF FUNCTION ExtendIconList */", start)
region = source[start:end]

assert "size_t old_bytes, added_bytes, new_bytes;" in region
assert "(size_t)newSize > SIZE_MAX / sizeof(IconInfo)" in region
assert "(size_t)newSize > UINT_MAX / sizeof(IconInfo)" in region
assert "old_bytes = (size_t)pIBD->IPD.totalPlaces * sizeof(IconInfo);" in region
assert "added_bytes = (size_t)incr * sizeof(IconInfo);" in region
assert "new_bytes = (size_t)newSize * sizeof(IconInfo);" in region
assert "XtMalloc ((Cardinal)new_bytes)" in region
assert "if (old_bytes != 0)" in region
assert "memcpy (pTmp, pIBD->IPD.placeList, old_bytes);" in region
assert "memset (&pTmp[pIBD->IPD.totalPlaces], 0, added_bytes);" in region
print("PASS MWM icon-list growth uses checked size_t byte arithmetic")
