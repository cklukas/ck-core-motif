#!/usr/bin/env python3
"""Check bounded XmBreadcrumbBar text drawing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text(encoding="utf-8")
item = source[source.index("static int item_width"):source.index("static void advance_cursor")]
draw_start = source.index("static void draw_item")
draw = source[draw_start:source.index("static void Redisplay", draw_start)]

assert "(unsigned long)label_width + 10UL" in item
assert "(unsigned long)width + (unsigned long)separator_width + 8UL" in item
assert "*cursor > INT_MAX - (int)separator_width - 4" in draw
print("PASS breadcrumb text geometry is bounded")
