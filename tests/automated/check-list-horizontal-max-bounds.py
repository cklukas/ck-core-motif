#!/usr/bin/env python3
"""Ensure List horizontal maximum arithmetic is checked before assignment."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nHorizontalMax")
body = source[start : source.index("\nstatic Boolean\nSetHorizontalScrollbar", start)]
assert "2LL * (long long)lw->list.BaseX" in body
assert "if (value < 1)" in body
assert "if (value > INT_MAX)" in body
assert source.count("lw->list.hmax = HorizontalMax(lw);") >= 2
print("PASS List horizontal maximum arithmetic is bounded")
