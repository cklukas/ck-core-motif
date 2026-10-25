#!/usr/bin/env python3
"""Check bounded XmBreadcrumbBar text drawing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text(encoding="utf-8")
start = source.index("static void draw_item")
end = source.index("static void Redisplay", start)
region = source[start:end]

assert "bounded_text_length(separator)" in region
assert "bounded_text_length(label)" in region
assert "*cursor > INT_MAX - 4" in region
print("PASS breadcrumb text geometry is bounded")
