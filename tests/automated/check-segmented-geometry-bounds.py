#!/usr/bin/env python3
"""Check SegmentedControl preferred width is bounded."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/SegmentedControl.c"
text = source.read_text(encoding="utf-8")

start = text.index("static XtGeometryResult QueryGeometry")
body = text[start:text.index("Boolean XmSegmentedControlSetItems", start)]
assert "uint64_t width = 0;" in body
assert "(unsigned long)child_width" in body
assert "width > SHRT_MAX" in body
assert "reply->width = (Dimension)width" in body
print("SegmentedControl preferred width is bounded")
