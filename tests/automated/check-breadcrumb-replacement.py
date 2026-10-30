#!/usr/bin/env python3
"""Ensure BreadcrumbBar replacement preserves state on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text(
    encoding="utf-8"
)
start = source.index("void XmBreadcrumbBarSetSegments")
end = source.index("\nCardinal XmBreadcrumbBarGetSegmentCount", start)
body = source[start:end]

assert "XmBreadcrumbBarItem *replacement = NULL;" in body
assert "free_item_array(replacement, i + 1);" in body
assert body.index("if (!replacement)") < body.index("free_items(bar);")
assert "bar->breadcrumb.items = replacement;" in body
print("PASS BreadcrumbBar replacement is transactional")
