#!/usr/bin/env python3
"""Check Calendar text coordinates are bounded before XmStringDraw."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Calendar.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
start = source.index("static void draw_text")
draw = source[start : source.index("static void draw_day", start)]
assert "bounded_position(x)" in draw
assert "bounded_position((long long)top" in draw
print("PASS Calendar draw coordinates are bounded before XmStringDraw")
