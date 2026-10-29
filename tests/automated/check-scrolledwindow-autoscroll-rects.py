#!/usr/bin/env python3
"""Check ScrolledWindow auto-scroll rectangles use signed spans."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()

assert "int hwidth = (int)sw->core.width" in source
assert "int vheight = (int)sw->core.height" in source
assert "scw_bounded_extent(hwidth > 2 ? hwidth : 2)" in source
assert "scw_bounded_extent(vheight > 2 ? vheight : 2)" in source

print("ScrolledWindow auto-scroll rectangles are bounded")
