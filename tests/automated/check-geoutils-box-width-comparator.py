#!/usr/bin/env python3
"""Check GeoUtils uses a valid three-way box-width comparator."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text(encoding="utf-8")
start = source.index("boxWidthCompare(\n", source.index("static int boxWidthCompare"))
end = source.index("FitBoxesAveraging(", start)
region = source[start:end]
assert "return (width1 > width2) - (width1 < width2);" in region
assert "> (*((XmKidGeometry *) boxPtr2))->box.width) ;" not in region
print("PASS GeoUtils box-width qsort comparator provides three-way ordering")
