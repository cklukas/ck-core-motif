#!/usr/bin/env python3
"""Check BreadcrumbBar rejects a missing segment array for nonzero count."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text()
start = source.index("void XmBreadcrumbBarSetSegments")
end = source.index("Cardinal XmBreadcrumbBarGetSegmentCount", start)
function = source[start:end]
assert "count != 0 && segments == NULL" in function
assert "count > (Cardinal)INT_MAX" in function
print("BreadcrumbBar rejects NULL segment arrays for nonzero counts")
