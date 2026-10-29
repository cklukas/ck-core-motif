#!/usr/bin/env python3
"""Check ScrolledWindow scrollbar and viewport positions are bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
assert "static Position\nscw_bounded_position(long long value)" in source
assert "*vsbX = (HasVSB) ? scw_bounded_position" in source
assert "*hsbY = (HasHSB) ? scw_bounded_position" in source
assert "*x = scw_bounded_position(MAX(0" in source
assert "*y = scw_bounded_position(MAX(0" in source
compute_start = source.index("ComputeLocations(\nXmScrolledWindowWidget")
compute = source[compute_start:source.index("CheckKids(", compute_start)]
assert compute.count("scw_bounded_position") >= 18
assert "*newx = (HasVSB) ? (vsb_width +" not in compute
assert "*newy = (HasHSB) ? (hsb_height +" not in compute
print("PASS ScrolledWindow positions clamp before X11 conversion")
