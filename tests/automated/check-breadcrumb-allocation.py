#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BreadcrumbBar.c").read_text()
start = source.index("void XmBreadcrumbBarSetSegments")
end = source.index("Cardinal XmBreadcrumbBarGetSegmentCount", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(*bar->breadcrumb.items)" in region
assert "if (widget == NULL || !XmIsBreadcrumbBar(widget))" in region
assert "if (!bar->breadcrumb.items)" in region
assert "if (!bar->breadcrumb.items[i].label)" in region
assert "XtFree((char *)bar->breadcrumb.items);" in region
assert region.count("bar->breadcrumb.current = -1;") >= 3
assert "widget != NULL && XmIsBreadcrumbBar(widget)" in source
print("BreadcrumbBar segment allocation guards present")
