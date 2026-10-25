#!/usr/bin/env python3
"""Check public item-list widgets bound Cardinal-sized allocations."""

from pathlib import Path


breadcrumb = Path("lib/Xm/BreadcrumbBar.c").read_text()
segmented = Path("lib/Xm/SegmentedControl.c").read_text()

assert "UINT_MAX / sizeof(*bar->breadcrumb.items)" in breadcrumb
assert "(Cardinal)count, sizeof(*bar->breadcrumb.items)" in breadcrumb
assert "UINT_MAX / sizeof(*copy)" in segmented
assert "(Cardinal)count, sizeof(*copy)" in segmented

print("Cardinal item-list allocation bounds are guarded")
