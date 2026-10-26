#!/usr/bin/env python3
"""Ensure dynamic Container tab-pool allocation has an explicit size bound."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("GetDumbTabList(")
start = source.index("static  XmTabList", start)
end = source.index("\n/************************************************************************\n* GetDynFirstColWidth", start)
region = source[start:end]

assert "size_t tab_bytes;" in region
assert "asked_num_tab > UINT_MAX / sizeof(*tab_pool)" in region
assert "tab_bytes = (size_t)asked_num_tab * sizeof(*tab_pool);" in region
assert "XtMalloc((Cardinal)tab_bytes)" in region

print("Container dynamic tab-pool allocation is explicitly bounded")
