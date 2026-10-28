#!/usr/bin/env python3
"""Ensure List keeps horizontal navigator values inside a valid range."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean\nSetHorizontalScrollbar(XmListWidget lw)")
body = source[start : source.index("\n/************************************************************************", start + 1)]
assert "if (lw->list.hmax < 1)" in body
assert "if (lw->list.hExtent < 1)" in body
assert "if (lw->list.hExtent > lw->list.hmax)" in body
assert "if (lw->list.hOrigin > lw->list.hmax - lw->list.hExtent)" in body
assert "lw->list.XOrigin = lw->list.hOrigin;" in body
print("PASS List horizontal navigator range is clamped after content shrink")
