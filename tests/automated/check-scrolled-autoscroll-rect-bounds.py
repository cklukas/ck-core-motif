#!/usr/bin/env python3
"""Check ScrolledWindow auto-scroll rectangles clamp XRectangle fields."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
assert "static unsigned short\nscw_bounded_extent(long long value)" in source
h_start = source.index("GetHorRects(\nWidget")
v_start = source.index("GetVertRects(\nWidget")
h = source[h_start:v_start]
v = source[v_start:source.index("UpdateAutoDrag(", v_start)]
for region in (h, v):
    assert "scw_bounded_position" in region
    assert "scw_bounded_extent" in region
assert "(*hrect)[0].x = 0 -" not in h
assert "(*vrect)[0].y = 0 -" not in v
print("PASS ScrolledWindow auto-scroll rectangle fields are bounded")
