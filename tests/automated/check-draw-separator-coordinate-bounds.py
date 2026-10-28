#!/usr/bin/env python3
"""Check separator centers and endpoints use bounded X11 coordinates."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()
start = source.index("void XmeDrawSeparator")
separator = source[start : source.index("/* only shadowed stuff", start)]
assert separator.count("ClampDrawCoordinate") >= 12
assert "center = y + height / 2" not in separator
assert "center = x + width / 2" not in separator
assert "(short) ((int) x +" not in separator
assert "(short) ((int) y +" not in separator
print("PASS separator coordinates are clamped")
