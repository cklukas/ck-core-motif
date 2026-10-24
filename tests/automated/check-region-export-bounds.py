#!/usr/bin/env python3
"""Check that region rectangle export fits allocation and Xlib count types."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("_XmRegionGetRectangles(")
end = source.index("\n/************************************************************************\n *\n *  _XmRegionSetGCRegion", start)
region = source[start:end]
assert "count > (long)INT_MAX" in region
assert "UINT_MAX / sizeof(XRectangle)" in region
assert "*rects = NULL;" in region
print("region rectangle export checks allocation and Xlib count bounds")
