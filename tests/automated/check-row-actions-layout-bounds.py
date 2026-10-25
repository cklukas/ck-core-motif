#!/usr/bin/env python3
"""Check RowActions does not wrap its X11 Position layout accumulator."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("static void layout(")
body = source[start:source.index("static void natural(", start)]
assert "long x = 0" in body
assert "x > SHRT_MAX" in body
assert "Position placement" in body
print("PASS RowActions layout clamps large placements")
