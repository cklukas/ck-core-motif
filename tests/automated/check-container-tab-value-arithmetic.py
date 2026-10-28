#!/usr/bin/env python3
"""Ensure Container tab values do not multiply in integer arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("static  XmTabList\nGetDumbTabList(")
body = source[start:source.index("\n}\n", start)]
assert "(float)tab_size * (float)(i + 1)" in body
assert "tab_size * (i + 1)" not in body
print("Container tab values convert before multiplying")
