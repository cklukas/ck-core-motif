#!/usr/bin/env python3
"""Check RowActions does not wrap its X11 Position layout accumulator."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
assert "static Boolean at_start_edge" in source
assert "_XmGetLayoutDirection((Widget)actions)" in source
start = source.index("static void layout(")
body = source[start:source.index("static void natural(", start)]
assert "total >= actions->core.width ? 0" in body
assert "x > SHRT_MAX" in body
assert "Position placement" in body
print("PASS RowActions layout clamps large placements")
