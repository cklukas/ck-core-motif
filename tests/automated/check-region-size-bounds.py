#!/usr/bin/env python3
"""Check that region rectangle allocations reject invalid sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("_XmRegionCreateSize(")
end = source.index("\nXmRegion\n_XmRegionCreate(", start)
region = source[start:end]
assert "#include <limits.h>" in source
assert "size <= 0" in region
assert "UINT_MAX / sizeof(XmRegionBox)" in region
print("region rectangle allocation sizes are checked")
