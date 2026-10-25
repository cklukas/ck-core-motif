#!/usr/bin/env python3
"""Check that region shadow segment counts cannot wrap before allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("if (nrects <= 0")
end = source.index("\n    if (!(topSegms", start)
region = source[start:end]
assert "nrects <= 0" in region
assert "INT_MAX / 2" in region
assert "INT_MAX / ((long)shadow_thick * 2)" in region
assert "(long)nrects * (long)shadow_thick * 2" in region
assert "UINT_MAX / sizeof(XSegment)" in region
print("region shadow segment counts are checked before allocation")
