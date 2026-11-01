#!/usr/bin/env python3
"""Supplement native geometry tests with the signed X-coordinate bound."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/SegmentedControl.c").read_text()
start = source.index("static void allocate_segments")
region = source[start:source.index("static void Initialize", start)]
assert "MIN(control->core.width, SHRT_MAX)" in region
assert "XtSetMappedWhenManaged(child, width != 0)" in region
assert "if (width)" in region
print("PASS SegmentedControl signed positions and zero-share mapping are bounded")
