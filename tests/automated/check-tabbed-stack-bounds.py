#!/usr/bin/env python3
"""Check legacy tabbed-stack list allocations respect Xt width."""

from pathlib import Path


source = Path("lib/Xm/TabList.c").read_text()

assert source.count("UINT_MAX / sizeof(XmTabAttributeRec)") >= 2
assert "XtMalloc((Cardinal)(sizeof(XmTabAttributeRec) *" in source
assert "(Cardinal)((size_t)newAllocated * sizeof(XmTabAttributeRec))" in source

print("tabbed-stack list allocation widths are guarded")
