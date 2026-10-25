#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BreadcrumbBar.c").read_text()
assert "#include <limits.h>" in source
start = source.index("static int item_width")
end = source.index("\nstatic Cardinal visible_start", start)
region = source[start:end]
assert "INT_MAX" in region
assert "static void advance_cursor" in region
assert "*cursor = INT_MAX" in region
query = source[source.index("static XtGeometryResult QueryGeometry"):]
assert "if (width >= INT_MAX - item)" in query
print("BreadcrumbBar size bounds present")
