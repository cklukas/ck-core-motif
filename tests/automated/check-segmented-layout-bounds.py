#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/SegmentedControl.c").read_text()
start = source.index("static void layout")
end = source.index("\nstatic XtGeometryResult QueryGeometry", start)
region = source[start:end]
assert "if (x >= available)" in region
assert "width = available - x;" in region
assert "if (x < available && width < available - x)" in region
assert "x = available;" in region
assert "if (x + width > available) width = available - x;" not in region
print("SegmentedControl layout bounds present")
