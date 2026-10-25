#!/usr/bin/env python3
"""Check FitBoxesAveraging handles empty and unrepresentable allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text(
    encoding="utf-8"
)
start = source.index("FitBoxesAveraging(", 10000)
body = source[start:source.index("FitBoxesProportional(", start)]
assert "numBoxes == 0" in body
assert "SIZE_MAX / sizeof(*sortedBoxes)" in body
assert "UINT_MAX / sizeof(*sortedBoxes)" in body
assert "if (sortedBoxes == NULL)" in body
assert "XtMalloc(\n        (Cardinal)((size_t)numBoxes *" in body

print("GeoUtils box-sorting allocation guards passed")
