#!/usr/bin/env python3
"""Check NumberBox arrow vertices are bounded before XPoint conversion."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(
    encoding="utf-8"
)
assert "static short bounded_point(int value)" in source
start = source.index("static void Redisplay(Widget widget")
body = source[start : source.index("static void commit_text", start)]
assert "bounded_point((int)number->core.height / 2 - 2)" in body
assert "bounded_point((int)number->core.height / 2 + 2)" in body
assert "bounded_point((int)number->core.height - 5)" in body
print("PASS NumberBox arrow vertices are bounded for XPoint")
