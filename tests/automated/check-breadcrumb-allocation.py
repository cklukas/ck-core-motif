#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BreadcrumbBar.c").read_text()
start = source.index("void XmBreadcrumbBarSetSegments")
end = source.index("Cardinal XmBreadcrumbBarGetSegmentCount", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(*bar->breadcrumb.items)" in region
assert "widget == NULL || !XmIsBreadcrumbBar(widget)" in region
assert "count > (Cardinal)INT_MAX" in region
assert "if (!replacement) return;" in region
assert "if (!replacement[i].label)" in region
assert "free_item_array(replacement, i + 1);" in region
assert region.index("free_items(bar);") > region.index("if (!replacement[i].label)")
assert "bar->breadcrumb.items = replacement;" in region
assert "bar->breadcrumb.current = new_current;" in region
assert "widget != NULL && XmIsBreadcrumbBar(widget)" in source
print("BreadcrumbBar segment allocation guards present")
