#!/usr/bin/env python3
"""Check highlight rectangle origins use bounded X11 coordinates."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()
highlight = source[source.index("void XmeDrawHighlight") : source.index("/******************************XmeDrawSeparator")]
assert "ClampDrawCoordinate((long long)x + draw_width - thickness)" in highlight
assert "ClampDrawCoordinate((long long)y + draw_height - thickness)" in highlight
assert "(short) ((int) x + draw_width - thickness)" not in highlight
assert "(short) ((int) y + draw_height - thickness)" not in highlight
print("PASS highlight rectangle origins are clamped")
