#!/usr/bin/env python3
"""Check BreadcrumbBar bounds long-path coordinates at the Xt draw boundary."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
start = source.index("static void draw_item")
draw = source[start : source.index("static void Redisplay", start)]
assert "bounded_position(*cursor)" in draw
assert "bounded_position(text_x)" in draw
assert "bounded_position(text_y)" in draw
print("PASS BreadcrumbBar draw coordinates are bounded before XmStringDraw")
