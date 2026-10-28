#!/usr/bin/env python3
"""Check shared shadow drawing clamps X11 coordinate conversions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "static short ClampDrawCoordinate(long long value);" in source
helper = source[source.index("static short\nClampDrawCoordinate") : source.index("static void\nDrawSimpleShadow")]
assert "SHRT_MIN" in helper and "SHRT_MAX" in helper
shadow = source[source.index("DrawSimpleShadow (Display") : source.index("/****************************XmeDrawShadows")]
assert shadow.count("ClampDrawCoordinate") >= 10
assert "(short) (ix +" not in shadow
assert "(short) (iy +" not in shadow
print("PASS shared shadow coordinates are clamped")
