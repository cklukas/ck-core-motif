#!/usr/bin/env python3
"""Ensure Sidebar natural height accumulation saturates safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void natural(XmSidebarWidget sidebar")
body = source[start : source.index("\nstatic void layout", start)]
assert "unsigned long extent" in body
assert "extent = (unsigned long)button + 2 * gap +" in body
assert "*height = (Dimension)MIN(MAX(extent,1),USHRT_MAX)" in body
print("PASS Sidebar natural height accumulation saturates safely")
