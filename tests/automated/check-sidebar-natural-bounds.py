#!/usr/bin/env python3
"""Ensure Sidebar natural height accumulation saturates safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void natural(XmSidebarWidget sidebar")
body = source[start : source.index("\nstatic void layout", start)]
assert "unsigned long total_height" in body
assert "total_height > USHRT_MAX" in body
assert "total_height ? total_height : 1UL" in body
print("PASS Sidebar natural height accumulation saturates safely")
